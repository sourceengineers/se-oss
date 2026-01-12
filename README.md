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
independently and is contained in its own directory. Packaging a single software
component is thus easy. Every directory follows a common structure:

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

## Contributing

### Semantic Versioning

The goal is to use [Semantic Versioning](https://semver.org/) on a component level.
In summary:

> Given a version number MAJOR.MINOR.PATCH, increment the:
> 1. MAJOR version when you make incompatible API changes
> 2. MINOR version when you add functionality in a backward compatible manner
> 3. PATCH version when you make backward compatible bug fixes
> 
> Additional labels for pre-release and build metadata are available as extensions to the MAJOR.MINOR.PATCH format.

### Commit Messages

This repository uses [Conventional Commits v1.0.0](https://www.conventionalcommits.org/en/v1.0.0).

In summary a commit message consists of:

```text
<type>[optional scope]: <description>
```

where `<type>` is one of the following:

- `feat`: _Introduces a new feature to the codebase (this correlates with MINOR in Semantic Versioning)_ ([Conventional Commits v1.0.0](https://www.conventionalcommits.org/en/v1.0.0))
- `fix`: _Patches a bug in your codebase (this correlates with PATCH in Semantic Versioning)_ ([Conventional Commits v1.0.0](https://www.conventionalcommits.org/en/v1.0.0))
- `build`: Changes to build scripts (e.g., CMake scripts)
- `chore`: Routine tasks such as dependency dependencies or containers
- `ci`: Changes to the CI configuration files and scripts
- `docs`: Changes in the documentation
- `style`: File formatting, no functional changes
- `refactor`: Refactoring production code
- `test`: Adding or updating tests

Furthermore, must breaking changes use an exclamation mark `!` after the type.

For example:

- `feat(log): add support for compile-time string replacement`
- `feat(log)!: remove trace log level from api`
- `fix(log): resolve buffer deadlock issue`
- `docs(readme): add contribution guidelines`
