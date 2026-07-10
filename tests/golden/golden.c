// Golden tests for cubiomes-viewer's cubiomes library integration.
// Each case pins a seed, version, dimension and a small biome-ID grid so
// that library upgrades cannot silently change generation results.
//
// Usage:
//   golden            run all cases, exit 1 on mismatch
//   golden --dump     print current generation results as C table rows
//                     (used to record new reference data)

#include "generator.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

enum { GRID_N = 4, GRID_STEP = 64 };

typedef struct
{
    const char *name;
    int mc;
    int dim;
    uint64_t seed;
    int x, y, z;        // block position of grid origin, sampled at scale 1
    int expect[GRID_N * GRID_N]; // biome ids, row-major, -1 = not recorded
} GoldenCase;

// Reference data recorded with cubiomes @ b12a532 (xpple fork).
// Grids sample every GRID_STEP blocks at scale 1:1.
static GoldenCase g_cases[] =
{
    { "1.18 overworld spawn region", MC_1_18, DIM_OVERWORLD, 3055396342337931721ULL,
      0, 63, 0, { 4, 4, 4, 5, 4, 4, 4, 5, 4, 4, 16, 16, 12, 4, 5, 46 } },
    { "1.21.3 overworld", MC_1_21_3, DIM_OVERWORLD, 3055396342337931721ULL,
      0, 63, 0, { 4, 4, 4, 5, 4, 4, 4, 5, 4, 4, 16, 16, 12, 4, 5, 46 } },
    { "1.21.4 pale garden", MC_1_21_4, DIM_OVERWORLD, 3055396342337931721ULL,
      640, 63, 64, { 186, 175, 29, 7, 178, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29 } },
    { "1.21.11 overworld", MC_1_21_11, DIM_OVERWORLD, 3055396342337931721ULL,
      0, 63, 0, { 4, 4, 4, 5, 4, 4, 4, 5, 4, 4, 16, 16, 12, 4, 5, 46 } },
    { "26.1 overworld", MC_26_1, DIM_OVERWORLD, 3055396342337931721ULL,
      0, 63, 0, { 4, 4, 4, 5, 4, 4, 4, 5, 4, 4, 16, 16, 12, 4, 5, 46 } },
    { "26.2 pale garden relocated", MC_26_2, DIM_OVERWORLD, 3055396342337931721ULL,
      3584, 63, -3456, { 186, 186, 175, 174, 175, 186, 175, 174, 174, 175, 175, 174, 174, 175, 186, 174 } },
    { "26.2 sulfur caves", MC_26_2, DIM_OVERWORLD, 3055396342337931721ULL,
      1984, -16, -3008, { 187, 187, 187, 24, 0, 187, 24, 24, 24, 24, 24, 187, 24, 0, 0, 0 } },
    { "26.1 same area no sulfur", MC_26_1, DIM_OVERWORLD, 3055396342337931721ULL,
      1984, -16, -3008, { 0, 0, 0, 24, 0, 24, 24, 24, 24, 24, 24, 0, 24, 0, 0, 0 } },
    { "26.2 dripstone caves", MC_26_2, DIM_OVERWORLD, 3055396342337931721ULL,
      3200, -16, -4096, { 174, 174, 174, 174, 174, 174, 27, 174, 155, 27, 4, 174, 27, 4, 4, 4 } },
    { "26.2 nether", MC_26_2, DIM_NETHER, 3055396342337931721ULL,
      0, 63, 0, { 8, 8, 8, 8, 8, 8, 171, 171, 8, 8, 171, 171, 8, 8, 171, 171 } },
};

static void sample_grid(const GoldenCase *c, int *out)
{
    Generator g;
    setupGenerator(&g, c->mc, 0);
    applySeed(&g, c->dim, c->seed);
    for (int j = 0; j < GRID_N; j++)
        for (int i = 0; i < GRID_N; i++)
            out[j * GRID_N + i] = getBiomeAt(&g, 1,
                c->x + i * GRID_STEP, c->y, c->z + j * GRID_STEP);
}

int main(int argc, char **argv)
{
    int dump = argc > 1 && strcmp(argv[1], "--dump") == 0;
    int failed = 0;
    size_t n = sizeof(g_cases) / sizeof(g_cases[0]);

    for (size_t k = 0; k < n; k++)
    {
        const GoldenCase *c = &g_cases[k];
        int got[GRID_N * GRID_N];
        sample_grid(c, got);

        if (dump)
        {
            printf("    { \"%s\", /* mc=%s dim=%d */\n      ", c->name,
                mc2str(c->mc), c->dim);
            printf("{ ");
            for (int i = 0; i < GRID_N * GRID_N; i++)
                printf("%d%s", got[i], i + 1 < GRID_N * GRID_N ? ", " : " } },\n");
            continue;
        }

        if (c->expect[0] == -1)
        {
            printf("SKIP  %-32s (no reference data)\n", c->name);
            continue;
        }
        if (memcmp(got, c->expect, sizeof(got)) != 0)
        {
            failed++;
            printf("FAIL  %-32s\n", c->name);
            for (int i = 0; i < GRID_N * GRID_N; i++)
                if (got[i] != c->expect[i])
                    printf("      [%d] expected %d (%s) got %d (%s)\n", i,
                        c->expect[i], biome2str(c->mc, c->expect[i]),
                        got[i], biome2str(c->mc, got[i]));
        }
        else
        {
            printf("OK    %-32s\n", c->name);
        }
    }

    if (!dump)
        printf("%s\n", failed ? "GOLDEN TESTS FAILED" : "golden tests passed");
    return failed ? 1 : 0;
}
