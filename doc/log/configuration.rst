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
   * - ``SE_OSS_LOG_REPLACE_STRINGS``
     - Replace format strings with IDs
     - ``OFF``
     - ``ON``, ``OFF``
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
   * - ``SE_OSS_LOG_MAX_LEVEL``
     - Log levels above here will be included in the output binary
     - ``TRACE``
     - ``TRACE``, ``DEBUG``, ``WARN``, ``INFO``, ``WARN``, ``ERROR``, ``FATAL``

An option can be turned on in CMake by forcing the variable in the cache:

.. code-block:: cmake
    :caption: CMakeLists.txt

    set(SE_OSS_LOG_REPLACE_STRINGS ON CACHE BOOL "" FORCE)
    set(SE_OSS_LOG_MAX_LEVEL "INFO" CACHE STRING "" FORCE)

``SE_OSS_LOG_SUPPORT_CBOR``
^^^^^^^^^^^^^^^^^^^^^^^^^^^

With this option enabled ``se-oss`` will fetch `intel/tinycbor <https://github.com/intel/tinycbor>`_ (MIT License) as a dependency.
You will also need to configure the logger to use the ``CborFormatter`` as described in the C++ Configuration below.

See :ref:`Binary Formatter` for more information.

``SE_OSS_LOG_REPLACE_STRINGS``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With this option enabled format strings will be replaced with a hash value at compile time.
That reduces binary size and log bandwidth requirements.

See :ref:`string_replacement` for details.


C++ Configuration
-----------------

Besides the CMake options, the log library can be configured from C++ code. For
that a configuration structure is provided. The following parameters can be set:

.. list-table::
   :header-rows: 1
   :widths: 25 50 25

   * - Parameter
     - Description
     - Default
   * - Formatter
     - Formatter used to format/serialize log records.
     - ``PrintfFormatter``
   * - Buffer
     - Buffer used to store log records between logger and sink.
     - ``ImmediateBuffer``
   * - Max Message Length
     - Maximum length of a log message.
     - 128

If using any other configration than default, provide the configration once per
binary in one source file. Here's an example for using the ``PrintfFormatter``
with a 1024 atomic byte buffer and a maximum message length of 128 bytes:

.. code-block:: c++
    :caption: main.cpp

    #include "se-oss/log/Conf.h"

    template <>
    auto se_oss::logConf<>()
    {
        return LogConf<PrintfFormatter<TimeFormat::ISO8601>, AtomicBuffer<1024>, 128>{};
    }

Default Configuration
^^^^^^^^^^^^^^^^^^^^^

The default configuration provides a minimal logger writing messages to ``stdout``. See :ref:`minimal_setup`.
