#include "worldimport.h"

#include <cstring>
#include <zlib.h>

std::string gunzipBytes(const uint8_t *data, size_t len)
{
    z_stream zs;
    std::memset(&zs, 0, sizeof(zs));
    // windowBits 15 + 32 lets zlib auto-detect gzip and zlib headers
    if (inflateInit2(&zs, 15 + 32) != Z_OK)
        return std::string();

    zs.next_in = const_cast<Bytef*>(data);
    zs.avail_in = (uInt) len;

    std::string out;
    char buf[16384];
    int ret;
    do {
        zs.next_out = reinterpret_cast<Bytef*>(buf);
        zs.avail_out = sizeof(buf);
        ret = inflate(&zs, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END)
        {
            inflateEnd(&zs);
            return std::string();
        }
        out.append(buf, sizeof(buf) - zs.avail_out);
    } while (ret != Z_STREAM_END);

    inflateEnd(&zs);
    return out;
}

namespace {

// NBT tag ids
enum {
    TAG_End = 0, TAG_Byte, TAG_Short, TAG_Int, TAG_Long, TAG_Float,
    TAG_Double, TAG_Byte_Array, TAG_String, TAG_List, TAG_Compound,
    TAG_Int_Array, TAG_Long_Array
};

struct Reader {
    const uint8_t *p;
    const uint8_t *end;
    bool ok = true;

    bool have(size_t n) { return ok && (size_t)(end - p) >= n; }
    uint8_t u8() { if (!have(1)) { ok = false; return 0; } return *p++; }
    int16_t i16() { if (!have(2)) { ok = false; return 0; } int16_t v = (p[0] << 8) | p[1]; p += 2; return v; }
    int32_t i32() { if (!have(4)) { ok = false; return 0; } int32_t v = ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3]; p += 4; return v; }
    int64_t i64() { if (!have(8)) { ok = false; return 0; } int64_t v = 0; for (int i = 0; i < 8; i++) v = (v << 8) | p[i]; p += 8; return v; }
    std::string str() {
        int16_t n = i16();
        if (n < 0 || !have(n)) { ok = false; return std::string(); }
        std::string s((const char*)p, n); p += n; return s;
    }
};

struct Found { bool seed = false; int64_t seedVal = 0; std::string version; };

// skip a payload of the given tag type
void skipPayload(Reader &r, int type);

void skipList(Reader &r)
{
    int etype = r.u8();
    int32_t n = r.i32();
    for (int32_t i = 0; i < n && r.ok; i++)
        skipPayload(r, etype);
}

void skipPayload(Reader &r, int type)
{
    switch (type)
    {
    case TAG_Byte:   r.u8(); break;
    case TAG_Short:  r.i16(); break;
    case TAG_Int:    r.i32(); break;
    case TAG_Long:   r.i64(); break;
    case TAG_Float:  r.i32(); break;
    case TAG_Double: r.i64(); break;
    case TAG_Byte_Array: { int32_t n = r.i32(); if (n < 0 || !r.have(n)) { r.ok = false; } else r.p += n; break; }
    case TAG_String: r.str(); break;
    case TAG_List:   skipList(r); break;
    case TAG_Int_Array:  { int32_t n = r.i32(); if (n < 0 || !r.have((size_t)n*4)) { r.ok = false; } else r.p += (size_t)n*4; break; }
    case TAG_Long_Array: { int32_t n = r.i32(); if (n < 0 || !r.have((size_t)n*8)) { r.ok = false; } else r.p += (size_t)n*8; break; }
    case TAG_Compound:
        for (;;) {
            int t = r.u8();
            if (!r.ok || t == TAG_End) break;
            r.str(); // name
            skipPayload(r, t);
        }
        break;
    default: r.ok = false; break;
    }
}

// walk a compound, extracting seed/version. parentName is the compound's name.
void walkCompound(Reader &r, const std::string &parentName, Found &f)
{
    for (;;)
    {
        int t = r.u8();
        if (!r.ok || t == TAG_End)
            break;
        std::string nm = r.str();
        if (!r.ok)
            break;

        if (t == TAG_Long && (nm == "seed" || nm == "RandomSeed"))
        {
            int64_t v = r.i64();
            if (r.ok && !f.seed) { f.seed = true; f.seedVal = v; }
        }
        else if (t == TAG_String && nm == "Name" && parentName == "Version")
        {
            std::string v = r.str();
            if (r.ok && f.version.empty()) f.version = v;
        }
        else if (t == TAG_Compound)
        {
            walkCompound(r, nm, f);
        }
        else
        {
            skipPayload(r, t);
        }
    }
}

} // namespace

bool parseLevelDatNbt(const uint8_t *data, size_t len,
                      int64_t *seed, std::string *version)
{
    Reader r{data, data + len};
    int t = r.u8();
    if (t != TAG_Compound)
        return false;
    r.str(); // root name (usually empty)
    Found f;
    walkCompound(r, std::string(), f);
    if (!f.seed)
        return false;
    if (seed) *seed = f.seedVal;
    if (version) *version = f.version;
    return true;
}
