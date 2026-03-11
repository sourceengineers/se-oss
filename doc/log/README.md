# log

To use the log library link against `se::log`:

```cmake
target_link_libraries(my_application PRIVATE se::log)
```

## Configuration

### CMake Options

The logging library supports the following options in CMake:

| Option                         | Description                                                         | Default | Options                                                    |
|--------------------------------|---------------------------------------------------------------------|---------|------------------------------------------------------------|
| `SE_OSS_LOG_REPLACE_STRINGS`   | Replace format strings with IDs                                     | `OFF`   | `ON`, `OFF`                                                |
| `SE_OSS_LOG_SUPPORT_CBOR`      | Support CBOR binary encoding                                        | `OFF`   | `ON`, `OFF`                                                |
| `SE_OSS_LOG_EXAMPLES`          | Compile se-log examples                                             | `OFF`   | `ON`, `OFF`                                                |
| `SE_OSS_LOG_TESTS`             | Compile se-log tests                                                | `OFF`   | `ON`, `OFF`                                                |
| `SE_OSS_LOG_MAX_LEVEL`         | Log levels above here will be included in the output binary         | `TRACE` | `TRACE`, `DEBUG`, `WARN`, `INFO`, `WARN`, `ERROR`, `FATAL` |
| `SE_OSS_LOG_RELEASE_MAX_LEVEL` | Log levels above here will be included in the output release binary | `INFO`  | `TRACE`, `DEBUG`, `WARN`, `INFO`, `WARN`, `ERROR`, `FATAL` |


An option can be turned on in CMake by forcing the variable in the cache:

```cmake
set(SE_OSS_LOG_REPLACE_STRINGS ON CACHE BOOL "" FORCE)
set(SE_OSS_LOG_MAX_LEVEL "INFO" CACHE STRING "" FORCE)
```

#### `SE_OSS_LOG_SUPPORT_CBOR`

Whith this option enabled`se-oss` will fetch [intel/tinycbor](https://github.com/intel/tinycbor) (MIT License) as a dependency.

See the [Binary Formatter](#binary-serialization-cborformatter) for more information.


#### `SE_OSS_LOG_REPLACE_STRINGS`

This option enables string replacement in format calls. The format strings will
be replaced at compile time with `u32` hash values. This reduces the size of the
binary and improves performance. During the build process a table of all format
strings will be generated in the binary directory. The is used on the host side 
to interpret the log messages.

String only makes sense if a binary formatter is used (e.g. `CborFormatter`).

You will need to enable string replacement for your target in CMake:

```cmake
include(${se-oss_SOURCE_DIR}/cmake/resource_database.cmake)
se_create_resource_database(TARGET my_application)
```

### C++ Configuration

Besides the CMake options, the log library can be configured from C++ code. For 
that a configuration structure is provided. The following parameters can be set:

| Parameter          | Description                                               | Default           |
|--------------------|-----------------------------------------------------------|-------------------|
| Formatter          | Formatter used to format/serialize log records.           | `PrintfFormatter` |
| Buffer             | Buffer used to store log records between logger and sink. | `NoBuffer`        | 
| Max Message Length | Maximum length of a log message.                          | 128               |

If using any other configration than default, provide the configration once per 
binary in one source file. Here's an example for using the `PrintfFormatter` 
with a 1024 atomic byte buffer and a maximum message length of 128 bytes:

```c++
// main.cpp
#include "se-oss/log/Conf.h"

template <>
auto se_oss::logConf<>()
{
    return LogConf<PrintfFormatter, AtomicBuffer<1024>, 128>{};
}
```

### Default Configuration

The library provides a minimal logger writing messages to stdout.

```c++
// main.cpp
#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"

int main()
{
    auto logRegistry = std::make_unique<se_oss::LogRegistry<>>();
    se_oss::Logger& log = logRegistry->createOrGetLogger(se_oss::DefaultLogComponents::DEFAULT);

    LOG_TRACE(log, "A trace message");
    LOG_INFO(log, "You can use printf-style formatting %u", 42U);

    return 0;
}
```

Note: This logger is not thread-safe.

## Formatters

### Printf Formatting `PrintfFormatter`

Printf style formatting is the commonly known format syntax on embedded devices.
The log call

```c++
// main.cpp
int main()
{
    //...
    se_oss::Logger& log = logRegistry->createOrGetLogger(se_oss::DefaultLogComponents::DEFAULT);
    LOG_INFO(log, "State changed to %u", state);
    //...
}
```

leads to the output

```text
00001234 [default] INFO State changed to 42
```

### Binary Serialization `CborFormatter`

This formatter requires the `SE_OSS_LOG_SUPPORT_CBOR` CMake option.

This formatter requires a tool on the host to interpret the log message.

```c++
LOG_INFO(log, "State changed to %u", state);
```

leads to the output (here shown as hex string)

```text
A5011B000647B9899DD36F0202030004735374617465206368616E67656420746F2025750681182A
```

or formatted as CBOR message

```text
A5                                      # map(5)
   01                                   # unsigned(1)
   1B 000647B9899DD36F                  # unsigned(1767712063607663)
   02                                   # unsigned(2)
   02                                   # unsigned(2)
   03                                   # unsigned(3)
   00                                   # unsigned(0)
   04                                   # unsigned(4)
   73                                   # text(19)
      5374617465206368616E67656420746F202575 # "State changed to %u"
   06                                   # unsigned(6)
   81                                   # array(1)
      18 2A                             # unsigned(42)
```

The format string or its hash value (depending on `SE_OSS_LOG_REPLACE_STRINGS`) is 
serialized together with the arguments and additional record metada (timestamp, 
log level, source id).
