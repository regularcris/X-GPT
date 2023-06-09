/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* URI parsing implementation
*
* For the latest on this and related APIs, please see: https://github.com/Microsoft/cpprestsdk
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#pragma once

#include <locale>

namespace xbox { namespace services { namespace cppresturi {

namespace web { namespace details { namespace uri_parser
{

bool validate(const xsapi_internal_string& encoded_string)
{
    const char* scheme_begin = nullptr;
    const char* scheme_end = nullptr;
    const char* uinfo_begin = nullptr;
    const char* uinfo_end = nullptr;
    const char* host_begin = nullptr;
    const char* host_end = nullptr;
    int port_ptr = 0;
    const char* path_begin = nullptr;
    const char* path_end = nullptr;
    const char* query_begin = nullptr;
    const char* query_end = nullptr;
    const char* fragment_begin = nullptr;
    const char* fragment_end = nullptr;

    // perform a parse, but don't copy out the data
    return inner_parse(
        encoded_string.c_str(),
        &scheme_begin,
        &scheme_end,
        &uinfo_begin,
        &uinfo_end,
        &host_begin,
        &host_end,
        &port_ptr,
        &path_begin,
        &path_end,
        &query_begin,
        &query_end,
        &fragment_begin,
        &fragment_end);
}

bool parse(const xsapi_internal_string &encoded_string, uri_components &components)
{
    const char* scheme_begin = nullptr;
    const char* scheme_end = nullptr;
    const char* host_begin = nullptr;
    const char* host_end = nullptr;
    const char* uinfo_begin = nullptr;
    const char* uinfo_end = nullptr;
    int port_ptr = 0;
    const char* path_begin = nullptr;
    const char* path_end = nullptr;
    const char* query_begin = nullptr;
    const char* query_end = nullptr;
    const char* fragment_begin = nullptr;
    const char* fragment_end = nullptr;

    if (inner_parse(
        encoded_string.c_str(),
        &scheme_begin,
        &scheme_end,
        &uinfo_begin,
        &uinfo_end,
        &host_begin,
        &host_end,
        &port_ptr,
        &path_begin,
        &path_end,
        &query_begin,
        &query_end,
        &fragment_begin,
        &fragment_end))
    {
        if (scheme_begin)
        {
            components.m_scheme.assign(scheme_begin, scheme_end);

            // convert scheme to lowercase
            std::transform(components.m_scheme.begin(), components.m_scheme.end(), components.m_scheme.begin(), [](char c) {
                return (char)tolower(c);
            });
        }
        else
        {
            components.m_scheme.clear();
        }

        if (uinfo_begin)
        {
            components.m_user_info.assign(uinfo_begin, uinfo_end);
        }

        if (host_begin)
        {
            components.m_host.assign(host_begin, host_end);

            // convert host to lowercase
            std::transform(components.m_host.begin(), components.m_host.end(), components.m_host.begin(), [](char c) {
                return (char)tolower(c);
            });
        }
        else
        {
            components.m_host.clear();
        }

        if (port_ptr)
        {
            components.m_port = port_ptr;
        }
        else
        {
            components.m_port = 0;
        }

        if (path_begin)
        {
            components.m_path.assign(path_begin, path_end);
        }
        else
        {
            // default path to begin with a slash for easy comparison
            components.m_path = "/";
        }

        if (query_begin)
        {
            components.m_query.assign(query_begin, query_end);
        }
        else
        {
            components.m_query.clear();
        }

        if (fragment_begin)
        {
            components.m_fragment.assign(fragment_begin, fragment_end);
        }
        else
        {
            components.m_fragment.clear();
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool inner_parse(
            const char* encoded,
            const char** scheme_begin, const char** scheme_end,
            const char** uinfo_begin, const char** uinfo_end,
            const char** host_begin, const char** host_end,
            _Out_ int * port,
            const char** path_begin, const char** path_end,
            const char** query_begin, const char** query_end,
            const char** fragment_begin, const char** fragment_end)
{
    *scheme_begin = nullptr;
    *scheme_end = nullptr;
    *uinfo_begin = nullptr;
    *uinfo_end = nullptr;
    *host_begin = nullptr;
    *host_end = nullptr;
    *port = 0;
    *path_begin = nullptr;
    *path_end = nullptr;
    *query_begin = nullptr;
    *query_end = nullptr;
    *fragment_begin = nullptr;
    *fragment_end = nullptr;

    const char*p = encoded;

    // IMPORTANT -- A uri may either be an absolute uri, or an relative-reference
    // Absolute: 'http://host.com'
    // Relative-Reference: '//:host.com', '/path1/path2?query', './path1:path2'
    // A Relative-Reference can be disambiguated by parsing for a ':' before the first slash

    bool is_relative_reference = true;
    const char*p2 = p;
    for (;*p2 != '/' && *p2 != '\0'; p2++)
    {
        if (*p2 == ':')
        {
            // found a colon, the first portion is a scheme
            is_relative_reference = false;
            break;
        }
    }

    if (!is_relative_reference)
    {
        // the first character of a scheme must be a letter
        if (!isalpha(*p))
        {
            return false;
        }

        // start parsing the scheme, it's always delimited by a colon (must be present)
        *scheme_begin = p++;
        for (;*p != ':'; p++)
        {
            if (!is_scheme_character(*p))
            {
                return false;
            }
        }
        *scheme_end = p;

        // skip over the colon
        p++;
    }

    // if we see two slashes next, then we're going to parse the authority portion
    // later on we'll break up the authority into the port and host
    const char*authority_begin = nullptr;
    const char*authority_end = nullptr;
    if (*p == '/' && p[1] == '/')
    {
        // skip over the slashes
        p += 2;
        authority_begin = p;

        // the authority is delimited by a slash (resource), question-mark (query) or octothorpe (fragment)
        // or by EOS. The authority could be empty ('file:///C:\file_name.txt')
        for (;*p != '/' && *p != '?' && *p != '#' && *p != '\0'; p++)
        {
            // We're NOT currently supporting IPv6, IPvFuture or username/password in authority
            if (!is_authority_character(*p))
            {
                return false;
            }
        }
        authority_end = p;

        // now lets see if we have a port specified -- by working back from the end
        if (authority_begin != authority_end)
        {
            // the port is made up of all digits
            const char*port_begin = authority_end - 1;
            for (;isdigit(*port_begin) && port_begin != authority_begin; port_begin--)
            { }

            if (*port_begin == ':')
            {
                // has a port
                *host_begin = authority_begin;
                *host_end = port_begin;

                //skip the colon
                port_begin++;

                *port = utility::conversions::scan_string<int>(utility::string_t(port_begin, authority_end), std::locale::classic());
            }
            else
            {
                // no port
                *host_begin = authority_begin;
                *host_end = authority_end;
            }

            // look for a user_info component
            const char*u_end = *host_begin;
            for (;is_user_info_character(*u_end) && u_end != *host_end; u_end++)
            { }

            if (*u_end == '@')
            {
                *host_begin = u_end+1;
                *uinfo_begin = authority_begin;
                *uinfo_end = u_end;
            }
            else
            {
                uinfo_end = uinfo_begin = nullptr;
            }
        }
    }

    // if we see a path character or a slash, then the
    // if we see a slash, or any other legal path character, parse the path next
    if (*p == '/' || is_path_character(*p))
    {
        *path_begin = p;

        // the path is delimited by a question-mark (query) or octothorpe (fragment) or by EOS
        for (;*p != '?' && *p != '#' && *p != '\0'; p++)
        {
            if (!is_path_character(*p))
            {
                return false;
            }
        }
        *path_end = p;
    }

    // if we see a ?, then the query is next
    if (*p == '?')
    {
        // skip over the question mark
        p++;
        *query_begin = p;

        // the query is delimited by a '#' (fragment) or EOS
        for (;*p != '#' && *p != '\0'; p++)
        {
            if (!is_query_character(*p))
            {
                return false;
            }
        }
        *query_end = p;
    }

    // if we see a #, then the fragment is next
    if (*p == '#')
    {
        // skip over the hash mark
        p++;
        *fragment_begin = p;

        // the fragment is delimited by EOS
        for (;*p != '\0'; p++)
        {
            if (!is_fragment_character(*p))
            {
                return false;
            }
        }
        *fragment_end = p;
    }

    return true;
}

}}}

} } }