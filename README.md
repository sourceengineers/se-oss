# Source Engineers Open Source Software `se-oss`

## Setup

Fetch using CMake:

```cmake
FetchContent_Declare(
        se_oss 
        GIT_REPOSITORY https://github.com/sourceengineers/se-oss.git 
        GIT_TAG        v1.0.0
)
FetchContent_MakeAvailable(se_oss)
```

or if you prefer submodules:

```shell
git submodule add https://github.com/sourceengineers/se-oss.git <path>
git submodule update --init
```

and in the `CMakeLists.txt`:

```cmake
add_subdirectory(<path>/se-oss)
```


## Software Components

This repository contains the following software components:

- [log](doc/log/README.md): Logging library.


### Structure

The repository is organized into software components. Each component can be used
independently and is contained in its own directory. Every directory follows a
common structure:

- `cmake` (optional): CMake functions and macros.
- `examples`: Example usage of the component.
- `include`: Public header files.
- `src`: Source files.
- `test`: Unit tests.
- `tools`: Python scripts and other tools.

### CMake Targets and Options

By default, all components are disabled. In your project you can enable them by
setting the corresponding option:

```cmake
# CMakeLists.txt or CMakePresets.json
set(SE_OSS_COMPONENT_<COMPONENT> ON CACHE BOOL "" FORCE)
set(SE_OSS_COMPONENT_LOG ON CACHE BOOL "" FORCE)
```

The executable must then be linked to each component used:

```cmake
target_link_libraries(
        <target>
        PRIVATE
        se_oss::<component>
        se_oss::log
)
```

Each target supports a basic set of options

The CMake options follow a naming convention:

| Option                                                              | Description                              |
|---------------------------------------------------------------------|------------------------------------------|
| `SE_OSS_COMPONENT_<COMPONENT>`<br/>e.g. `SE_OSS_COMPONENT_LOG`      | Provide library for selected component.  |
| `SE_OSS_TESTS`                                                      | Enable tests on all components.          |
| `SE_OSS_EXAMPLES`                                                   | Enable examples on all components.       |
| `SE_OSS_BENCHMARKS`                                                 | Enable benchmarks on all components.     |
| `SE_OSS_<COMPONENT>_TESTS`<br/>e.g. `SE_OSS_LOG_TESTS`              | Enable tests for a given component.      |
| `SE_OSS_<COMPONENT>_EXAMPLES`<br/>e.g. `SE_OSS_LOG_EXAMPLES`        | Enable examples for a given component.   |
| `SE_OSS_<COMPONENT>_BENCHMARKS`<br/>e.g. `SE_OSS_LOG_BENCHMARKS`    | Enable benchmarks for a given component. |
| `SE_OSS_<COMPONENT>_<OPTION>`<br/>e.g. `SE_OSS_LOG_REPLACE_STRINGS` | Set a component specific options.        |

## Running tests and examples

When this repository is opened as top level CMake project, you can run the following
commands to build and run all tests and examples:

```shell
cmake --workflow --preset ci-example
cmake --workflow --preset ci-test
```
