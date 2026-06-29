/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/Types.h"

#include <array>

#include <cbor.h>

#include <cstddef>
#include <type_traits>

namespace se_oss {
/**
 * Keys used/encoded in the CBOR map.
 */
enum class CborLogKeys : uint8_t
{
    TIMESTAMP = 1U, /**< Timestamp of the log record. */
    LOG_LEVEL = 2U, /**< Log level. */
    CONTEXT_TAG = 3U, /**< Tag of the log context. */
    LOGGER_TAG = 4U, /**< Tag of the logger instance. */
    MESSAGE_STRING = 5U, /**< The format string as ASCII string. */
    MESSAGE_ID = 6U, /**< ID of the format string (for usage with string replacement). */
    VALUES = 7U /**< Array of argument values. */
};

constexpr uint8_t toUint(CborLogKeys key) { return static_cast<uint8_t>(key); }

/**
 * Formatter that serializes log records to Concise Binary Object Representation (CBOR).
 *
 * This formatter produces compact binary logs suitable for transmission or storage.
 */
class CborFormatter
{
public:
    /**
     * Formats a log record into CBOR.
     *
     * @tparam TFormat The type of the format string/ID.
     * @tparam Values The types of the arguments.
     * @param buffer The buffer to write into.
     * @param bufferSize The available size in the buffer.
     * @param record The log record to format.
     * @param formatString The format string or ID.
     * @param values The arguments to serialize.
     * @return The number of bytes written, or 0 on failure.
     */
    template<typename TFormat, typename... Values>
    static size_t
    format(void* buffer, std::size_t bufferSize, const LogRecord& record, TFormat formatString, const Values&... values)
    {
        if (buffer == nullptr) {
            return 0U;
        }

        auto* byteBuffer = static_cast<uint8_t*>(buffer);
        CborEncoder encoder;
        CborEncoder mapEncoder;

        size_t nValues = sizeof...(Values);

        cbor_encoder_init(&encoder, byteBuffer, bufferSize, 0);
        cbor_encoder_create_map(&encoder, &mapEncoder, 5U + (nValues > 0U ? 1U : 0U));

        encodeRecord(mapEncoder, record);
        encodeFormatString(mapEncoder, formatString);

        serializeValues(mapEncoder, std::forward<const Values&>(values)...);

        auto result = cbor_encoder_close_container(&encoder, &mapEncoder);

        if (result != CborNoError) {
            return 0U;
        } else {
            return cbor_encoder_get_buffer_size(&encoder, byteBuffer);
        }
    }

private:
    template<typename... Values>
    static void serializeValues(CborEncoder& encoder, const Values&... values)
    {
        CborEncoder arrayEncoder;
        size_t nValues = sizeof...(Values);

        cbor_encode_uint(&encoder, toUint(CborLogKeys::VALUES));
        cbor_encoder_create_array(&encoder, &arrayEncoder, nValues);
        // Workaround for missing fold expression in C++14
        std::array<CborError, sizeof...(Values)> errors[] {(encodeValue(arrayEncoder, values))...};
        (void)errors;
        cbor_encoder_close_container(&encoder, &arrayEncoder);
    }

    static void serializeValues(CborEncoder&)
    {
        // no value to serialize
    }

    template<typename T>
    static std::enable_if_t<std::is_unsigned<T>::value && !std::is_same<T, bool>::value, CborError>
    encodeValue(CborEncoder& encoder, T value)
    {
        return cbor_encode_uint(&encoder, value);
    }

    template<typename T>
    static std::enable_if_t<std::is_signed<T>::value && !std::is_floating_point<T>::value, CborError>
    encodeValue(CborEncoder& encoder, T value)
    {
        return cbor_encode_int(&encoder, value);
    }

    template<typename T>
    static std::enable_if_t<std::is_same<T, bool>::value, CborError> encodeValue(CborEncoder& encoder, T value)
    {
        return cbor_encode_boolean(&encoder, value);
    }

    template<typename T>
    static std::enable_if_t<std::is_same<T, float>::value, CborError> encodeValue(CborEncoder& encoder, T value)
    {
        return cbor_encode_float(&encoder, value);
    }

    template<typename T>
    static std::enable_if_t<std::is_same<T, double>::value, CborError> encodeValue(CborEncoder& encoder, T value)
    {
        return cbor_encode_double(&encoder, value);
    }

    template<typename T>
    static std::enable_if_t<std::is_same<T, const char*>::value || std::is_same<T, char*>::value, CborError>
    encodeValue(CborEncoder& encoder, T value)
    {
        return cbor_encode_text_stringz(&encoder, value);
    }

    static void encodeFormatString(CborEncoder& encoder, const char* formatString)
    {
        cbor_encode_uint(&encoder, toUint(CborLogKeys::MESSAGE_STRING));
        cbor_encode_text_stringz(&encoder, formatString);
    }

    static void encodeFormatString(CborEncoder& encoder, uint32_t formatStringId)
    {
        cbor_encode_uint(&encoder, toUint(CborLogKeys::MESSAGE_ID));
        cbor_encode_uint(&encoder, formatStringId);
    }

    static void encodeRecord(CborEncoder& encoder, const LogRecord& record)
    {
        cbor_encode_uint(&encoder, toUint(CborLogKeys::TIMESTAMP));
        cbor_encode_uint(&encoder, record.timestamp);

        cbor_encode_uint(&encoder, toUint(CborLogKeys::LOG_LEVEL));
        cbor_encode_uint(&encoder, toUint(record.metadata.level));

        cbor_encode_uint(&encoder, toUint(CborLogKeys::CONTEXT_TAG));
        cbor_encode_uint(&encoder, record.metadata.contextTag);

        cbor_encode_uint(&encoder, toUint(CborLogKeys::LOGGER_TAG));
        cbor_encode_uint(&encoder, record.metadata.loggerTag);
    }
};
}  // namespace se_oss
