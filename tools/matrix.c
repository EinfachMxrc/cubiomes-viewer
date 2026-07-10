// probe cubiomes for per-version structure + cave-biome support
#include "generator.h"
#include "finders.h"
#include "util.h"
#include <stdio.h>

static const int versions[] = {
    MC_1_18, MC_1_19_2, MC_1_20, MC_1_21_1, MC_1_21_3,
    MC_1_21_4, MC_1_21_5, MC_1_21_9, MC_1_21_11, MC_26_1, MC_26_2
};

static const struct { int id; const char *name; } structs[] = {
    { Village, "Village" }, { Desert_Pyramid, "Desert_Pyr" },
    { Jungle_Temple, "Jungle_Tmp" }, { Swamp_Hut, "Swamp_Hut" },
    { Monument, "Monument" }, { Mansion, "Mansion" },
    { Outpost, "Outpost" }, { Ancient_City, "Ancient_City" },
    { Trail_Ruins, "Trail_Ruins" }, { Trial_Chambers, "Trial_Ch" },
    { Ruined_Portal, "Ruined_Portal" }, { Ocean_Ruin, "Ocean_Ruin" },
    { Shipwreck, "Shipwreck" }, { Treasure, "Treasure" },
    { End_City, "End_City" }, { Fortress, "Fortress" },
    { Bastion, "Bastion" },
};

static const struct { int id; const char *name; } caves[] = {
    { dripstone_caves, "dripstone" }, { lush_caves, "lush" },
    { deep_dark, "deep_dark" }, { sulfur_caves, "sulfur_caves" },
};

int main(void)
{
    printf("### Structures\n\n");
    printf("| Structure |");
    for (size_t v = 0; v < sizeof(versions)/sizeof(*versions); v++)
        printf(" %s |", mc2str(versions[v]));
    printf("\n|---|");
    for (size_t v = 0; v < sizeof(versions)/sizeof(*versions); v++)
        printf("---|");
    printf("\n");
    for (size_t s = 0; s < sizeof(structs)/sizeof(*structs); s++)
    {
        printf("| %s |", structs[s].name);
        for (size_t v = 0; v < sizeof(versions)/sizeof(*versions); v++)
        {
            StructureConfig sc;
            int ok = getStructureConfig(structs[s].id, versions[v], &sc);
            printf(" %s |", ok ? "yes" : "-");
        }
        printf("\n");
    }

    printf("\n### Cave biomes (overworld, exists in version)\n\n");
    printf("| Biome |");
    for (size_t v = 0; v < sizeof(versions)/sizeof(*versions); v++)
        printf(" %s |", mc2str(versions[v]));
    printf("\n|---|");
    for (size_t v = 0; v < sizeof(versions)/sizeof(*versions); v++)
        printf("---|");
    printf("\n");
    for (size_t c = 0; c < sizeof(caves)/sizeof(*caves); c++)
    {
        printf("| %s |", caves[c].name);
        for (size_t v = 0; v < sizeof(versions)/sizeof(*versions); v++)
            printf(" %s |", biomeExists(versions[v], caves[c].id) ? "yes" : "-");
        printf("\n");
    }
    return 0;
}
