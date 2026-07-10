#ifndef WORLDIMPORT_H
#define WORLDIMPORT_H

#include <cstdint>
#include <cstddef>
#include <string>

// Parse an already-decompressed level.dat NBT blob. Extracts the world seed
// (Data>WorldGenSettings>seed for 1.16+, or Data>RandomSeed for older) and
// the version name (Data>Version>Name, e.g. "1.21"). Returns true if a seed
// was found. Pure/Qt-free so it can be unit tested from a console harness.
bool parseLevelDatNbt(const uint8_t *data, size_t len,
                      int64_t *seed, std::string *version);

// gunzip helper (zlib). Returns empty on failure.
std::string gunzipBytes(const uint8_t *data, size_t len);

#endif // WORLDIMPORT_H
