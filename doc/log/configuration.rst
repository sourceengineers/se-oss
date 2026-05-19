.. _configuration:

Configuration Details
=====================

CMake Options
-------------

The logging library supports the following options in CMake:

.. list-table::
   :header-rows: 1
   :widths: 25 40 10 25

   * - Option
     - Description
     - Default
     - Options
   * - ``SE_OSS_LOG_SUPPORT_CBOR``
     - Support CBOR binary encoding
     - ``OFF``
     - ``ON``, ``OFF``
   * - ``SE_OSS_LOG_EXAMPLES``
     - Compile se-log examples
     - ``OFF``
     - ``ON``, ``OFF``
   * - ``SE_OSS_LOG_TESTS``
     - Compile se-log tests
     - ``OFF``
     - ``ON``, ``OFF``

An option can be turned on in CMake by forcing the variable in the cache:

.. code-block:: cmake
    :caption: CMakeLists.txt

    set(SE_OSS_LOG_SUPPORT_CBOR ON CACHE BOOL "" FORCE)

``SE_OSS_LOG_SUPPORT_CBOR``
^^^^^^^^^^^^^^^^^^^^^^^^^^^

With this option enabled ``se-oss`` will fetch `intel/tinycbor <https://github.com/intel/tinycbor>`_ (MIT License) as a dependency.
You will also need to configure the logger to use the ``CborFormatter`` as described in the C++ Configuration below.

See :ref:`Binary Formatter` for details.

``SE_OSS_LOG_REPLACE_STRINGS``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With this option enabled format strings will be replaced with a hash value at compile time.
That reduces binary size and log bandwidth requirements.

See :ref:`string_replacement` for details.


C++ Configuration
-----------------

The logging behavior can be configured using a configuration header file.
Create a file called ``UserLogConf.h`` in your project and add the interface library called ``se_oss_log_conf`` to the CMakeLists `before` adding ``se-oss``.
If no ``se_oss_log_conf`` is provided the logger will fall back on the default configuration.

.. code-block:: cmake
    :caption: CMakeLists.txt
    :emphasize-lines: 2-3

    # [..]
    add_library(se_oss_log_conf INTERFACE)
    target_include_directories(se_oss_log_conf INTERFACE ${CMAKE_SOURCE_DIR}/src/conf)

    set(SE_OSS_COMPONENT_LOG ON CACHE BOOL "" FORCE)
    FetchContent_Declare( #[..]

The following parameters must be set in ``UserLogConf.h``:

.. list-table::
   :header-rows: 1
   :widths: 25 50 25

   * - Parameter
     - Description
     - Default
   * - ``SE_OSS_LOG_REPLACE_STRINGS``
     - Replace format strings with IDs
     - disabled
   * - ``log_conf::Formatter``
     - Formatter used to format/serialize log records.
     - ``PrintfFormatter``
   * - ``log_conf::Buffer``
     - Buffer used to store log records between logger and sink.
     - ``ImmediateBuffer``
   * - ``log_conf::MAX_MESSAGE_LENGTH``
     - Maximum length of a log message.
     - 128
   * - ``log_conf::MAX_LOG_LEVEL``
     - Log levels above here will be included in the output binary.
     - ``LogLevel::TRACE``

If using any other configration than default, provide the configration once per
binary in one source file. Here's an example for using the ``PrintfFormatter``
with a 2048 atomic byte buffer and a maximum message length of 256 bytes:

.. code-block:: c++
    :caption: UserLogConf.h

    #pragma once

    #include "se-oss/log/buffer/AtomicBuffer.h"
    #include "se-oss/log/format/PrintfFormatter.h"

    namespace se_oss {
    namespace log_conf {

    // #define SE_OSS_LOG_REPLACE_STRINGS
    using Formatter = PrintfFormatter<TimeFormat::ISO8601>;
    using Buffer = AtomicBuffer<2048>;
    constexpr std::size_t MAX_MESSAGE_LENGTH {256};
    constexpr LogLevel MAX_LOG_LEVEL {LogLevel::TRACE};

    } // namespace log_conf
    } // namespace se_oss


.. _default_conf:

Default Configuration
^^^^^^^^^^^^^^^^^^^^^
If not ``se_oss_log_conf`` is provided in the CMakeLists the logger falls back on the default configuration.
The default configuration provides a minimal logger writing messages to ``stdout``. See :ref:`minimal_setup`.
