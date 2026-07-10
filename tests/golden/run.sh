#!/bin/sh
# Build and run the golden tests (Linux/macOS).
# Usage: tests/golden/run.sh [--dump]
set -e
root="$(cd "$(dirname "$0")/../.." && pwd)"
cu="$root/cubiomes"

mkdir -p "$root/build"
cc -std=gnu2x -O2 -fwrapv -I"$cu" "$root/tests/golden/golden.c" \
    "$cu"/biomenoise.c "$cu"/biomes.c "$cu"/finders.c "$cu"/generator.c \
    "$cu"/layers.c "$cu"/noise.c "$cu"/quadbase.c "$cu"/terrainnoise.c \
    "$cu"/util.c "$cu"/xradv.c "$cu"/features/stronghold.c \
    -lm -o "$root/build/golden"

exec "$root/build/golden" "$@"
