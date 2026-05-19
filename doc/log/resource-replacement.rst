.. _string_replacement:

String Replacement
==================

Log format strings can take up a lot of space on the target non-volatile memory or hit the throughput limit on the communication interface.
The solution provided here replaces format strings with their hashes at compile time.

.. code-block:: c++
    :caption: UserLogConf.h

    // [..]
    #define SE_OSS_LOG_REPLACE_STRINGS
    // [..]

This option enables string replacement in format calls. The format strings will
be replaced at compile time with ``u32`` hash values. This reduces the size of the
binary and improves performance. During the build process a table of all format
strings will be generated in the binary directory. The is used on the host side
to interpret the log messages.

String only makes sense if a binary formatter is used (e.g. ``CborFormatter``).

You will need to enable string replacement for your target in CMake:

.. code-block:: cmake

    include(${se-oss_SOURCE_DIR}/cmake/resource_database.cmake)
    se_create_resource_database(TARGET my_application)


..  figure:: res/resource-replacement.drawio.svg
    :alt: Log Filter Points
    :scale: 120%
    :align: center

    Resource replacement in the build process.

