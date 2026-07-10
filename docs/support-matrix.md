# Version Support Matrix

Which Minecraft versions this build supports and what generation features
are available per version. Backed by the [xpple/cubiomes](https://github.com/xpple/cubiomes)
library (submodule commit pinned in the repo) and verified by the golden
tests in [`tests/golden`](../tests/golden).

Regenerate the structure/biome tables below with:

```
gcc -std=gnu2x -O2 -fwrapv -Icubiomes tools/matrix.c \
    cubiomes/*.c cubiomes/features/stronghold.c -lm -o build/matrix
./build/matrix
```

## Versions selectable in the viewer

| Version | Selectable | Biomes | Caves | Structures | Locate | Search | Notes |
|---|---|---|---|---|---|---|---|
| ≤ 1.17 | yes¹ | full | n/a | full | full | full | legacy, unchanged from upstream |
| 1.18 | yes | full | dripstone, lush | full | full | full | |
| 1.19 | yes | full | + deep_dark | full | full | full | |
| 1.20 | yes | full | full | + trail ruins | full | full | |
| 1.21.1 | yes¹ | full | full | + trial chambers | full | full | |
| 1.21.3 | yes¹ | full | full | full | full | full | pale garden (parent 1.21) |
| 1.21.4 (Winter Drop) | yes | full | full | full | full | full | pale garden placement |
| 1.21.5 | yes¹ | full | full | full | full | full | pale garden fix |
| 1.21.6 | — | — | — | — | — | — | no name in lib, gap skipped² |
| 1.21.9 | yes¹ | full | full | full | full | full | |
| 1.21.11 | yes | full | full | full | full | full | 1.21 head |
| 26.1 | yes | full | full | full | full | full | Chaos Cubed pre |
| 26.2 | yes (default) | full | + **sulfur_caves** | full | full | full | Chaos Cubed release |

¹ Hidden from the version dropdown unless "experimental versions" is enabled
(these are superseded patch versions between named releases). The current
world's version is always shown.

² `MC_1_21_6` exists in the library enum but has no assigned name string
(`mc2str` returns `"?"`); the dropdown skips such gaps rather than showing
an unusable entry.

## Structures (`getStructureConfig` succeeds)

| Structure | 1.18 | 1.19.2 | 1.20 | 1.21.1 | 1.21.3 | 1.21.4 | 1.21.5 | 1.21.9 | 1.21.11 | 26.1 | 26.2 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| Village | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Desert Pyramid | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Jungle Temple | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Swamp Hut | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Monument | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Mansion | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Outpost | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Ancient City | - | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Trail Ruins | - | - | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Trial Chambers | - | - | - | yes | yes | yes | yes | yes | yes | yes | yes |
| Ruined Portal | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Ocean Ruin | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Shipwreck | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Buried Treasure | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| End City | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Fortress | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| Bastion | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |

## Cave biomes (`biomeExists`, overworld)

| Biome | 1.18 | 1.19.2 | 1.20 | 1.21.1 | 1.21.3 | 1.21.4 | 1.21.5 | 1.21.9 | 1.21.11 | 26.1 | 26.2 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| dripstone_caves | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| lush_caves | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| deep_dark | - | yes | yes | yes | yes | yes | yes | yes | yes | yes | yes |
| sulfur_caves | - | - | - | - | - | - | - | - | - | - | **yes** |

## Known limitations

- **Sulfur Caves generation** uses game-extracted biome-tree data
  (`tables/btree262.h`) from the xpple fork, not estimated parameters.
  Geysers (the surface indicator for sulfur caves) are terrain features,
  not a separate biome, so they are not shown as map markers.
- **26.x structures**: no new structure types were added in 26.1/26.2, so
  the existing structure finders apply unchanged.
- macOS builds are untested (no macOS CI runner yet).
