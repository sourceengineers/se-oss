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
add_subdirectory(<path>)
```


## Software Components

This repository contains the following software components:

### [log](doc/log/README.md)
