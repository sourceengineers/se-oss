/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_FORMAT_CBORFORMATTER_H
#define SE_SE_LOG_FORMAT_CBORFORMATTER_H

#include "se-log/Types.h"

#include <cbor.h>

#include <cstddef>


namespace se_oss {



class CborFormatter
{
public:
    template<typename... Values>
    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, const char *const formatString, const Values& ...values)
    {
        (void) this;
        auto* byteBuffer = static_cast<uint8_t*>(buffer);
        CborEncoder encoder;
        CborEncoder mapEncoder;
        CborEncoder arrayEncoder;

        size_t nValues = sizeof...(Values);

        cbor_encoder_init(&encoder, byteBuffer, bufferSize, 0);
        cbor_encoder_create_map(&encoder, &mapEncoder, 4U + (nValues > 0U ? 1U : 0U));
        // cbor_encode_text_stringz(&mapEncoder, "ts");
        cbor_encode_uint(&mapEncoder, 1U);
        cbor_encode_uint(&mapEncoder, record.timestamp);

        // cbor_encode_text_stringz(&mapEncoder, "lvl");
        cbor_encode_uint(&mapEncoder, 2U);
        cbor_encode_uint(&mapEncoder, toUint(record.metadata.logLevel));

        // cbor_encode_text_stringz(&mapEncoder, "cmp");
        cbor_encode_uint(&mapEncoder, 3U);
        cbor_encode_uint(&mapEncoder, record.metadata.sourceId);

        // cbor_encode_text_stringz(&mapEncoder, "msg");
        cbor_encode_uint(&mapEncoder, 4U);
        cbor_encode_text_stringz(&mapEncoder, formatString);

        if (nValues > 0U) {
            // cbor_encode_text_stringz(&mapEncoder, "val");
            cbor_encode_uint(&mapEncoder, 5U);
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

    template<typename... Values>
    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, uint32_t formatStringId, const Values& ...values)
    {
        (void) this;
        auto* byteBuffer = static_cast<uint8_t*>(buffer);
        CborEncoder encoder;
        CborEncoder mapEncoder;
        CborEncoder arrayEncoder;

        size_t nValues = sizeof...(Values);

        cbor_encoder_init(&encoder, byteBuffer, bufferSize, 0);
        cbor_encoder_create_map(&encoder, &mapEncoder, 4U + (nValues > 0U ? 1U : 0U));
        // cbor_encode_text_stringz(&mapEncoder, "ts");
        cbor_encode_uint(&mapEncoder, 1U);
        cbor_encode_uint(&mapEncoder, record.timestamp);

        // cbor_encode_text_stringz(&mapEncoder, "lvl");
        cbor_encode_uint(&mapEncoder, 2U);
        cbor_encode_uint(&mapEncoder, toUint(record.metadata.logLevel));

        // cbor_encode_text_stringz(&mapEncoder, "cmp");
        cbor_encode_uint(&mapEncoder, 3U);
        cbor_encode_uint(&mapEncoder, record.metadata.sourceId);

        // cbor_encode_text_stringz(&mapEncoder, "msg");
        cbor_encode_uint(&mapEncoder, 4U);
        cbor_encode_uint(&mapEncoder, formatStringId);

        if (nValues > 0U) {
            // cbor_encode_text_stringz(&mapEncoder, "val");
            cbor_encode_uint(&mapEncoder, 5U);
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
};
} // namespace se

#endif // SE_SE_LOG_FORMAT_CBORFORMATTER_H