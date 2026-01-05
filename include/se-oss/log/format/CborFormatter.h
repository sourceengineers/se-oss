/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_FORMAT_CBORFORMATTER_H
#define SE_SE_LOG_FORMAT_CBORFORMATTER_H

#include "se-oss/log/Types.h"

#include <cbor.h>

#include <cstddef>


namespace se_oss {

enum class CborLogKeys : uint8_t
{
    TIMESTAMP = 1U,
    LOG_LEVEL = 2U,
    SOURCE_ID = 3U,
    MESSAGE_STRING = 4U,
    MESSAGE_ID = 5U,
    VALUES = 6U
};

constexpr uint8_t toUint(CborLogKeys key) { return static_cast<uint8_t>(key); }

class CborFormatter
{
public:
    template<typename TFormat, typename... Values>
    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, TFormat formatString, const Values& ...values)
    {
        (void) this;
        auto* byteBuffer = static_cast<uint8_t*>(buffer);
        CborEncoder encoder;
        CborEncoder mapEncoder;
        CborEncoder arrayEncoder;

        size_t nValues = sizeof...(Values);

        cbor_encoder_init(&encoder, byteBuffer, bufferSize, 0);
        cbor_encoder_create_map(&encoder, &mapEncoder, 4U + (nValues > 0U ? 1U : 0U));

        encodeRecord(&mapEncoder, record);
        encodeFormatString(&mapEncoder, formatString);

        if (nValues > 0U) {
            cbor_encode_uint(&mapEncoder, toUint(CborLogKeys::VALUES));
            cbor_encoder_create_array(&mapEncoder, &arrayEncoder, nValues);
            // todo: c++14 workaround
            (encodeValue(&arrayEncoder, values), ...);
            cbor_encoder_close_container(&mapEncoder, &arrayEncoder);
        }

        auto result = cbor_encoder_close_container(&encoder, &mapEncoder);

        if (result != CborNoError) {
            return 0U;
        } else {
            return cbor_encoder_get_buffer_size(&encoder, byteBuffer);
        }
    }

private:

    template<typename T>
    std::enable_if_t<std::is_unsigned<T>::value && !std::is_same<T,bool>::value, CborError> encodeValue(CborEncoder *encoder, T value)
    {
        return cbor_encode_uint(encoder, value);
    }
    template<typename T>
    std::enable_if_t<std::is_signed<T>::value && !std::is_floating_point<T>::value, CborError> encodeValue(CborEncoder *encoder, T value)
    {
        return cbor_encode_int(encoder, value);
    }
    template<typename T>
    std::enable_if_t<std::is_same<T,bool>::value, CborError> encodeValue(CborEncoder *encoder, T value)
    {
        return cbor_encode_boolean(encoder, value);
    }

    template<typename T>
    std::enable_if_t<std::is_same<T,float>::value, CborError> encodeValue(CborEncoder *encoder, T value)
    {
        return cbor_encode_float(encoder, value);
    }

    template<typename T>
    std::enable_if_t<std::is_same<T,double>::value, CborError> encodeValue(CborEncoder *encoder, T value)
    {
        return cbor_encode_double(encoder, value);
    }

    template<typename T>
    std::enable_if_t<std::is_same<T,const char*>::value || std::is_same<T,char*>::value, CborError> encodeValue(CborEncoder *encoder, T value)
    {
        return cbor_encode_text_stringz(encoder, value);
    }

    static void encodeFormatString(CborEncoder *encoder, const char* formatString)
    {
        cbor_encode_uint(encoder, toUint(CborLogKeys::MESSAGE_STRING));
        cbor_encode_text_stringz(encoder, formatString);
    }

    static void encodeFormatString(CborEncoder *encoder, uint32_t formatStringId)
    {
        cbor_encode_uint(encoder, toUint(CborLogKeys::MESSAGE_ID));
        cbor_encode_uint(encoder, formatStringId);
    }

    static void encodeRecord(CborEncoder *encoder, LogRecord record)
    {
        cbor_encode_uint(encoder, toUint(CborLogKeys::TIMESTAMP));
        cbor_encode_uint(encoder, record.timestamp);

        cbor_encode_uint(encoder, toUint(CborLogKeys::LOG_LEVEL));
        cbor_encode_uint(encoder, toUint(record.metadata.level));

        cbor_encode_uint(encoder, toUint(CborLogKeys::SOURCE_ID));
        cbor_encode_uint(encoder, record.metadata.sourceId);
    }
};
} // namespace se

#endif // SE_SE_LOG_FORMAT_CBORFORMATTER_H