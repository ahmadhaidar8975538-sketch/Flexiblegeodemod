# Flexible Generator

Early prototype for a hybrid Geometry Dash layout generator mod.

## Current state

- Modular C++ generation core.
- Configurable length, seed, density, and difficulty.
- Randomized obstacle-pattern selection.
- Basic structural validation (bounds, spacing, and object count).
- A small Geode menu hook to confirm the mod loads.

## Important limitation

This is a **foundation prototype**, not a finished in-editor generator.
The current validator checks structural constraints only. It does not prove
that a generated sequence is physically completable by a player. Direct
Geometry Dash editor object placement, preview UI, undo integration, and
game-physics-aware playability checks are not implemented yet.

## Build

Install the Geode SDK that matches your Geometry Dash / Geode version, set
`GEODE_SDK` to its directory, then run:

```sh
geode build
```

The project follows the official Geode example-mod CMake layout. A successful
build has not been verified in this environment because the Geode SDK and
target game toolchain are not installed here.
