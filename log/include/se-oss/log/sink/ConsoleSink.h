/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ConsoleWriter.h"
#include "FilteredSink.h"

namespace se_oss {

using ConsoleSink = FilteredSink<ConsoleWriter>;

}  // namespace se_oss
