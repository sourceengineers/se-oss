Formatter/Serializer
====================




Printf Formatting
-----------------

Printf-style formatting, the commonly known format syntax on embedded devices, is supported by ``PrintfFormatter``.
The goal is to provide backwards compatability and familiar syntax.

The log call can look something like this:

.. code-block:: c++
    :caption: main.cpp

    LOG_INFO(log, "State changed to %u", state);

This leads to the output:

.. code-block:: text

    2026-05-05T12:38:17.677Z I [default] -- State changed to 42

Time Format Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^

The time format of the ``PrintfFormatter`` can be selected as template parameter:

.. code-block:: c++
    :caption: main.cpp

    #include "se-oss/log/Conf.h"

    template <>
    auto se_oss::logConf<>()
    {
        return LogConf<PrintfFormatter<TimeFormat::ISO8601>, AtomicBuffer<1024>, 128>{};
    }

These time formats are supported:

+----------------+-----------------------------------------------------+-------------------------------+
| TimeFormat     | Description                                         | Example Output                |
+================+=====================================================+===============================+
| ``NONE``       | No timestamp                                        | ``_``                         |
+----------------+-----------------------------------------------------+-------------------------------+
| ``DECIMAL``    | Timestamp in decimal format                         | ``1768902731209138``          |
+----------------+-----------------------------------------------------+-------------------------------+
| ``DECIMAL_8``  | Trailing 8 timestamp digits in decimal format       | ``31209138``                  |
+----------------+-----------------------------------------------------+-------------------------------+
| ``DECIMAL_10`` | Trailing 10 timestamp digits in decimal format      | ``2731209138``                |
+----------------+-----------------------------------------------------+-------------------------------+
| ``HEX``        | Timestamp in hex format                             | ``648CEC2EF99B2``             |
+----------------+-----------------------------------------------------+-------------------------------+
| ``HEX_8``      | Trailing 8 timestamp digits in hex format           | ``C2EF99B2``                  |
+----------------+-----------------------------------------------------+-------------------------------+
| ``ISO8601``    | Timestamp in human-readable date time format        | ``2026-01-20T09:52:11.209Z``  |
+----------------+-----------------------------------------------------+-------------------------------+


.. _Binary Formatter:

Binary Serialization using CBOR
-------------------------------

.. note::
    This formatter requires the ``SE_OSS_LOG_SUPPORT_CBOR`` CMake option.

Printf-style is well suited for human readable output such as serial interfaces but formatting (especially floating points) can be costly.
In complex applications tooling is used on the host system to parse and filter log messages.
The goal of binary serialization is to provide a machine readable message format.
This has the following advantages:

* Metadata such as log level, tags and timestamp are preserved in a binary structure
* Value formatting can be deferred from the target to the host system. The target just sends the format string and values as a structure.
* CBOR serialization is often computationally cheaper than printf

.. code-block:: c++
    :caption: main.cpp

    #include "se-oss/log/Conf.h"

    template <>
    auto se_oss::logConf<>()
    {
        return LogConf<CborFormatter, AtomicBuffer<1024>, 128>{};
    }

A change of formatter does not impact the log calls. So, the call stays is the same as before:

.. code-block:: c++
    :caption: main.cpp

    LOG_INFO(log, "State changed to %u", state);

The output is now binary message (here shown as hex string):

.. code-block:: text
    :caption: Console Output

    A6011B00065113C4EAFFA0020203000418FF05735374617465206368616E67656420746F2025750781182A

or as structured CBOR message:

.. code-block:: text
    :caption: CBOR Structured

    A6                                      # map(6)
       01                                   # unsigned(1)
       1B 00065113C4EAFFA0                  # unsigned(1777995210227616)
       02                                   # unsigned(2)
       02                                   # unsigned(2)
       03                                   # unsigned(3)
       00                                   # unsigned(0)
       04                                   # unsigned(4)
       18 FF                                # unsigned(255)
       05                                   # unsigned(5)
       73                                   # text(19)
          5374617465206368616E67656420746F202575 # "State changed to %u"
       07                                   # unsigned(7)
       81                                   # array(1)
          18 2A                             # unsigned(42)

The message can also be viewed as JSON-like structure.

.. code-block:: text
    :caption: CBOR Diagnostic Format

    {
        1: 1777995210227616,
        2: 2,
        3: 0,
        4: 255,
        5: "State changed to %u",
        7: [
            42
        ]
    }


.. note::
    This formatter requires a tool on the host to interpret the log message.



Custom Formatter
----------------


.. code-block:: cpp
    :caption: main.cpp

    class MyFormatter
    {
    public:
        template<typename... Values>
        static size_t format(
            void* buffer,
            std::size_t bufferSize,
            const se_oss::LogRecord& record,
            const char* const formatString,
            const Values&... values
        )
        {
            se_oss::LogStringBuffer string {buffer, bufferSize};
            string.appendTime("%y%m%dT%H%M%S", record.timestamp);
            string.append("| %s | ", toString(record.metadata.level));
            string.append("%s | ", record.loggerName);
            string.append(formatString, std::forward<const Values>(values)...);
            string.endLine();
            return string.length();
        }
    };

    template<>
    auto se_oss::logConf<>()
    {
        return LogConf<MyFormatter, AtomicBuffer<LOG_BUFFER_SIZE>, LOG_MAX_MESSAGE_LENGTH> {};
    }

