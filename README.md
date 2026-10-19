
# Brain-Inspired Multi-Agent Navigation

**GPS of the Brain and Parallel HNA* experiments on Recast/Detour navigation meshes.**

Developed by **Vahid Rahmani**, this repository preserves a C++ research prototype for multi-agent path planning. The research combines brain-inspired spatial representations with hierarchical navigation, using Recast/Detour as the foundation for geometry processing, navigation-mesh queries, and interactive visualization.

The central question is how agents can plan through complex environments using useful spatial abstractions: a structured representation of locations, a hierarchy of connected regions, and local paths that can be assembled into longer routes.

## Research focus

### GPS of the Brain

The project explores the **GPS of the Brain** approach to spatial navigation. The demo contains grid-like spatial-point generation, mappings between those points and navigation polygons, and visualization routines for inspecting the resulting representation.

The implementation exposes **GPS Subdivision**, a grid-size control, and **GPS POINTS** in the navigation test tool. Experimental routines read and write intermediate point and polygon mappings such as `GPS.txt`, `gpsID.txt`, and `gpsIDok.txt`.

These components provide a testbed for investigating brain-inspired spatial organization in a navigable virtual environment. The repository is an engineering research prototype; it does not establish a validated biological model of navigation.

### Parallel HNA*

The second research direction is **Parallel HNA*** for hierarchical path planning. The custom navigation implementation groups polygons into clusters, builds higher-level graphs, represents connections between regions, and stores local paths and traversal costs.

The source also contains threading experiments around endpoint-to-graph connections and local search preparation. Several alternative threading blocks are commented out, so the exact parallel execution strategy must be checked in the selected code path before running measurements.

The intended benefit is to reduce repeated low-level search work across navigation queries. This README does not claim a measured speedup, optimality guarantee, or GPU implementation.

### Multi-agent context

The research targets environments in which multiple agents need routes through shared geometry. The repository also includes DetourCrowd for crowd navigation experiments.

Individual route planning, crowd movement, and coordinated collision-free multi-agent planning are separate capabilities. The presence of crowd utilities does not establish that the custom GPS/HNA* experiments implement a complete coordinated multi-agent solver.

## Implementation overview

| Component | Role |
| --- | --- |
| Recast | Generate walkable navigation meshes from geometry |
| Detour | Query polygon connectivity and compute navigation paths |
| Custom hierarchical navigation | Build clusters, abstract graphs, and local path connections |
| METIS | Partition the graph during hierarchy construction |
| GPS experiment tools | Generate spatial points, map them to polygons, and visualize results |
| SDL / OpenGL / GLUT | Interactive application and experimental rendering |
| DetourCrowd | Bundled crowd-navigation utilities |

The broad workflow is to load geometry, build a navigation mesh, construct the selected spatial abstraction, choose start and goal positions, and inspect the resulting paths and experimental logs.

## Repository layout

| Path | Contents |
| --- | --- |
| `HierarchicalPathfinding/` | Custom navigation source and graph structures |
| `RecastDemo/Source/NavMeshTesterTool.cpp` | GPS experiments, path-query tools, and visualization |
| `RecastDemo/Source/Sample_SoloMesh.cpp` | Solo-mesh construction and hierarchy integration |
| `RecastDemo/Source/main.cpp` | Application entry point and menus |
| `Recast/`, `Detour/` | Navigation-mesh generation and queries |
| `DetourCrowd/`, `DetourTileCache/` | Crowd and tile-cache utilities |
| `DebugUtils/` | Debug rendering helpers |
| `Detour2/`, `DebugUtils2/` | Additional experimental copies of navigation components |
| `Docs/`, `Doxyfile` | Bundled documentation resources |
| `RecastDemo/premake4.lua` | Legacy build-generation configuration |
| `liB/`, `liB2015/` | Historical binary libraries |

## Getting started

```bash
git clone https://github.com/rahmani-vahid/recastnavigation-master_New.git
cd recastnavigation-master_New
```

**Build status:** this is a historical research snapshot. A clean build and runtime session have not been validated for this README. The checked-in Premake configuration needs review against the modified application.

The source references a C++11-capable compiler, SDL 1.2, OpenGL/GLU, GLUT, METIS, and Windows-specific concurrency headers (`ppl.h`, `agents.h`, and `windows.h`). The METIS call uses a legacy interface; substituting a newer library may require source changes.

Before compiling:

1. Replace machine-specific absolute include paths, including the METIS path in `RecastDemo/Include/Navigation.h`.
2. Check the duplicate navigation and graph headers and select consistent include directories.
3. Configure compilation of `HierarchicalPathfinding/Source/Navigation.cpp` and linkage to a compatible METIS library.
4. Check the architecture and compiler compatibility of committed binary libraries, or rebuild dependencies.
5. Review source wildcards and exclude backup or alternative implementations that should not be compiled together.
6. Supply the runtime assets and experiment-specific data needed by the chosen demo mode.

The historical build entry point is `RecastDemo/premake4.lua`. Use a compatible Premake 4 installation to inspect its available actions after correcting the configuration. No ready-to-run cross-platform build command is claimed here.

## Interactive workflow

Once a working build and its runtime assets are available:

1. Launch the demo from a working directory containing `DroidSans.ttf` and a `Meshes/` directory with OBJ geometry.
2. Choose a sample and mesh, configure navigation parameters, and select **Build**.
3. For hierarchical experiments, configure the hierarchy settings and select **Build Hierarchy**.
4. For GPS experiments, inspect **GPS Subdivision**, adjust the grid-size control, and select **GPS POINTS**.
5. Use the navigation test tool to set start and goal positions and inspect the paths.

The application uses `W/A/S/D` for camera movement and the right mouse button for rotation. In the navigation test tool, `Shift` + left click sets the start and left click sets the goal.

GPS routines contain environment-specific coordinate ranges, fixed-size buffers, and intermediate text-file dependencies. Inspect those settings before applying them to another mesh. Generating GPS points alone is not a complete end-to-end experiment.

## Evaluation and reproducibility

The code contains pathfinding experiments, timing instrumentation, and text-file logging. This snapshot does not provide a standalone automated test suite or a reproducible benchmark procedure.

A useful comparison should hold the mesh and start/goal queries fixed and record:

- Graph construction time and memory use.
- Query latency and throughput.
- Path length, validity, and failure rate against a baseline.
- Hierarchy depth, cluster size, and GPS grid settings.
- Agent count, thread configuration, compiler, and hardware.

Some output files are written in append mode. Use separate working directories for separate runs and inspect the active experiment code before interpreting logs as benchmark results.

## Credits and license

Research extensions and experiments are maintained by [Vahid Rahmani](https://github.com/rahmani-vahid).

This is a modified research version of Recast Navigation, originally authored by **Mikko Mononen**. Recast/Detour and the other bundled components retain their original authorship and notices.

See [License.txt](License.txt) for the bundled Recast license. Preserve that notice and consult the applicable licenses for third-party dependencies. This README does not replace or extend the existing license terms.
