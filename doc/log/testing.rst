Testing
=======

``Logger`` is final and logs through a non-virtual template, so it cannot be mocked.
Tests observe logging one level down instead, at the sink the logger's context writes to.
The log library ships header-only test support for this in ``log/test``, available to your
unit tests through the ``se_oss::log_test_support`` target:

.. code-block:: cmake
    :caption: CMakeLists.txt

    target_link_libraries(my_app_tests PRIVATE se_oss::log_test_support GTest::gmock)

.. list-table::
   :header-rows: 1
   :widths: 25 55

   * - Header
     - Purpose
   * - ``CapturingLogger.h``
     - One fixture member that bundles a ``CapturingLogSink``, a ``LogContext`` with a fixed
       timestamp and a ``Logger`` to hand to the class under test.
   * - ``CapturingLogSink.h``
     - ``ILogSink`` that records each message's level and formatted text.
   * - ``FixedTimeProvider.h``
     - ``ITimeProvider`` with a fixed, settable time, for building a ``LogContext`` by hand.
       Declare it before the context, which keeps a reference to it.
   * - ``LogSinkMock.h``
     - Google Mock of ``ILogSink``, for pure call expectations.

.. code-block:: cpp
    :caption: my_component_test.cpp

    #include "CapturingLogger.h"

    se_oss::CapturingLogger log;                                   // name "test", tag 0, timestamp always 0

    MyComponent component {log.logger()};
    component.run();

    EXPECT_TRUE(log.contains(se_oss::LogLevel::DEBUG, "status changed to 5"));
    EXPECT_EQ(1U, log.count());
    EXPECT_THAT(log.sink().records()[0].text, HasSubstr("[test]"));
    log.context().setLogFilterLevel(se_oss::LogLevel::OFF);       // silence the rest of the test

The recorded text is what the formatter produced, timestamp, level and logger name included, so
match on it with ``contains()`` or ``HasSubstr``. With a deferred (``AtomicBuffer``) log
configuration ``contains()``, ``count()`` and ``sink()`` drain the context first, so no
``distributeMessages()`` call is needed in tests.
