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
    :caption: UserLogConf.h

    // [..]
    using Formatter = PrintfFormatter<TimeFormat::ISO8601>;
    // [..]

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


----


.. _Binary Formatter:

Binary Serialization using CBOR
-------------------------------

.. note::
    This formatter requires the ``SE_OSS_LOG_SUPPORT_CBOR`` CMake option. se-oss will then use the `tinycbor`_ library by Intel (MIT license).

.. _tinycbor: https://github.com/intel/tinycbor

Printf-style is well suited for human readable output such as serial interfaces but formatting (especially floating points) can be costly.
In complex applications tooling is used on the host system to parse and filter log messages.
The goal of binary serialization is to provide a machine readable message format.
This has the following advantages:

* Metadata such as log level, tags and timestamp are preserved in a binary structure
* Value formatting can be deferred from the target to the host system. The target just sends the format string and values as a structure.
* CBOR serialization is often computationally cheaper than printf

Concise Binary Object Representation (CBOR) is used for structured serialization because of its efficient encoding (e.g. in comparison to JSON) and well-defined encoding.
You can find further information about CBOR on the `official CBOR website`_ and the `CBOR book`_.

.. _official CBOR website: https://cbor.io/
.. _CBOR book: https://cborbook.com/

.. code-block:: c++
    :caption: UserLogConf.h

    // [..]
    using Formatter = CborFormatter;
    // [..]

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
    :emphasize-lines: 7-9

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

As you can see in the highlighted lines, the log arguments are not formatted on the target.
They are instead output as a array.

.. note::
    This formatter requires a tool on the host to interpret the log message.

CBOR Keys
^^^^^^^^^
se-log serializes messages as one map with the following keys as defined in ``CborLogKeys``:

.. list-table::
   :header-rows: 1

   * - Key
     - Value Type
     - Description
   * - ``1``
     - ``u64``
     - Timestamp, by default in microseconds since epoch
   * - ``2``
     - ``u8``
     - Log level
   * - ``3``
     - ``u8``
     - Context tag
   * - ``4``
     - ``u8``
     - Logger tag
   * - ``5``
     - ``string``
     - Log format message as string
   * - ``6``
     - ``u32``
     - Log format message ID for string replacement
   * - ``7``
     - ``any``
     - Array of log arguments


----


Custom Formatter
----------------

If the formatter and serializer above do not satisfy your needs you can implement your own formatter.
To do so, implement a class containing a static ``format`` method with the signature below.
Then select your formatter class in the log configuration.

.. code-block:: cpp
    :caption: UserLogConf.h
    :emphasize-lines: 12-18,33

    #pragma once

    #include "se-oss/log/buffer/AtomicBuffer.h"
    #include "se-oss/log/format/StringBuffer.h"

    namespace se_oss {

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
            LogStringBuffer string {buffer, bufferSize};
            string.appendTime("%y%m%dT%H%M%S", record.timestamp);
            string.append("| %s | ", toString(record.metadata.level));
            string.append("%s | ", record.loggerName);
            string.append(formatString, std::forward<const Values>(values)...);
            string.endLine();
            return string.length();
        }
    };

    namespace log_conf {

    // #define SE_OSS_LOG_REPLACE_STRINGS
    using Formatter = MyFormatter;
    using Buffer = AtomicBuffer<2048>;
    constexpr std::size_t MAX_MESSAGE_LENGTH {256};
    constexpr LogLevel MAX_LOG_LEVEL {LogLevel::TRACE};

    } // namespace log_conf
    } // namespace se_oss

