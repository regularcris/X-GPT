// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

namespace legacy
{

/// <summary>
/// Enumerates the type of data contained in the http request body.
/// </summary>
enum http_request_message_type
{
    /// <summary>
    /// No message.
    /// </summary>
    empty_message,

    /// <summary>
    /// The message is of type string.
    /// </summary>
    string_message,

    /// <summary>
    /// The message is of type vector, and acts as a memory buffer.
    /// </summary>
    vector_message
};

class http_call_request_message_internal
{
public:
    http_call_request_message_internal();

    http_call_request_message_internal(_In_ xsapi_internal_string messageString);

    http_call_request_message_internal(_In_ xsapi_internal_vector<unsigned char> messageVector);

    _XSAPIIMP const xsapi_internal_string& request_message_string() const;

    _XSAPIIMP const xsapi_internal_vector<unsigned char>& request_message_vector() const;

    _XSAPIIMP http_request_message_type get_http_request_message_type() const;

private:
    xsapi_internal_vector<unsigned char> m_requestMessageVector;
    xsapi_internal_string m_requestMessageString;
    http_request_message_type m_httpRequestMessageType;
};

/// <summary>
/// Represents an http request message.
/// </summary>
class http_call_request_message
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    inline http_call_request_message();

    /// <summary>
    /// Internal function
    /// </summary>
    inline http_call_request_message(_In_ const http_call_request_message_internal* internalObj);

    /// <summary>
    /// The http request message if it is a string type.
    /// </summary>
    inline _XSAPIIMP const xsapi_internal_string& request_message_string() const;

    /// <summary>
    /// The http request message if it is a buffer.
    /// </summary>
    inline _XSAPIIMP const xsapi_internal_vector<unsigned char>& request_message_vector() const;

    /// <summary>
    /// The type of message.
    /// </summary>
    inline _XSAPIIMP http_request_message_type get_http_request_message_type() const;

private:
    xsapi_internal_vector<unsigned char> m_requestMessageVector;
    xsapi_internal_string m_requestMessageString;
    http_request_message_type m_httpRequestMessageType;
};

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END