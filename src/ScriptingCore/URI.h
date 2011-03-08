/**********************************************************\ 
Original Author: Dan Weatherford

Imported into FireBreath:    Oct 4, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Dan Weatherford and Facebook, Inc
\**********************************************************/

#pragma once
#ifndef H_FB_URI
#define H_FB_URI
#include <string>
#include <map>
#include <set>
#include <boost/cstdint.hpp>
#include "APITypes.h"

namespace FB {
    //////////////////////////////////////////////////////////////////////////////
    /// @class FB::URI
    ///
    /// @brief  Data structure for dealing with URI strings
    ///
    /// @code
    ///     FB::URI my_uri("http://frank@www.firebreath.org/some/path?myqData=asdf#fdsa");
    ///     // my_uri.protocol == "http"
    ///     // my_uri.login = "frank"
    ///     // my_uri.domain == "www.firebreath.org"
    ///     // my_uri.port == 0 (indicating default port)
    ///     // my_uri.path == "/some/path"
    ///     // my_uri.query_data["myqData"] == "asdf"
    ///     // my_uri.fragment == "fdsa"
    /// @endcode
    /// @since 1.4b1
    //////////////////////////////////////////////////////////////////////////////
    class URI {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public static std::string FB::URI::url_decode(const std::string& in)
        ///
        /// @brief  Decodes the given urlencoded URL
        ///
        /// @code
        /// std::string decoded = FB::URI::url_decode("http://www.firebreath.org/display/documentation/Mac%20Video%20Tutorial")
        /// @endcode
        ///
        /// @param            in    urlencoded URL
        /// @returns std::string    raw URL (not encoded)
        /// @since 1.4b1
        /// @see url_encode
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::string url_decode(const std::string& in);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public static std::string FB::URI::url_encode(const std::string& in)
        ///
        /// @brief  Encodes the given URL for transmission
        ///
        /// @code
        /// std::string encoded = FB::URI::url_encode("http://www.firebreath.org/display/documentation/Mac Video Tutorial")
        /// @endcode
        ///
        /// @param            in    urlencoded URL
        /// @returns std::string    raw URL (not encoded)
        /// @since 1.4b1
        /// @see url_decode
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::string url_encode(const std::string& in);

    public:
        /// Initializes an empty FB::URI object
        URI() : port(0) {}

        /// Initializes a FB::URI object by decoding the input URL
        URI(const std::string& str_uri);

        /// Compares two FB::URI objects
        bool operator==(const URI& right) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public std::string FB::URI::toString(bool include_domain_part = true) const
        ///
        /// @brief  converts the FB::URI object to a string
        ///
        /// @code
        /// FB::URI uri;
        /// uri.protocol = "http";
        /// uri.host = "www.firebreath.org";
        /// uri.path = "/applications";
        /// uri.query_data["user"] = "taxilian";
        /// uri.fragment = "bleh";
        /// std::string struri = uri.toString();
        /// // struri == "http://taxilian@www.firebreath.org/applications?user=taxilian#bleh"
        /// @endcode
        ///
        /// @param  include_domain_part bool    if false, only the path and later will be returned (no domain or http://, etc)
        /// @returns std::string
        /// @since 1.4b1
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string toString(bool include_domain_part = true) const;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public static  FB::URI FB::URI::fromString(const std::string& in_str)
        ///
        /// @brief  Returns a URI object from the given string
        ///
        /// @param  in_str  string to parse to create the new URI object
        /// @returns FB::URI
        /// @since 1.4b1
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static URI fromString(const std::string& in_str);

        // The protocol used. e.g. "http", "https", "file", "ftp", etc
        std::string protocol;
        // The login name in the URI (if any)
        std::string login;
        // The URI domain. e.g. "www.firebreath.org"
        std::string domain;
        // The port; a value of 0 indicates the default port for that protocol
        boost::uint16_t port;
        // The path of the request. e.g. "/some/filename.php"
        std::string path;
        // A std::map<string,string> of GET variables => values
        std::map<std::string, std::string> query_data;
        // The anchor fragment of the url (the part after # in /path/to/something.html#CoolStuff)
        std::string fragment;

    protected:
        typedef std::map<std::string, std::string> StringStringMap;
        static StringStringMap m_lhMap;

    public:
        //////////////////////////////////////////////////////////////////////////////
        /// Resets the list of valid localhost addresses
        /// @see isLocalhost
        /// @see registerValidLocalhost
        /// @since 1.4b1
        //////////////////////////////////////////////////////////////////////////////
        static void resetValidLocalhost();
        //////////////////////////////////////////////////////////////////////////////
        /// Registers a domain as a valid localhost address
        /// @param domain the domain to register as a valid localhost address (such as localhost.firebreath.org)
        /// @param ip optional IP address to indicate as a valid localhost. If none specified, default is loopback
        /// @see isLocalhost
        /// @see resetValidLocalhost
        /// @since 1.4b1
        //////////////////////////////////////////////////////////////////////////////
        static void registerValidLocalhost(std::string domain, std::string ip = "");

        //////////////////////////////////////////////////////////////////////////////
        /// Appends the given path component to the URI
        ///
        /// @code
        /// FB::URI uri("http://www.firebreath.org")
        /// uri.appendPathComponent("some");
        /// uri.appendPathComponent("path");
        /// // uri.path == "/some/path"
        /// @endcode
        ///
        /// @since 1.4b1
        //////////////////////////////////////////////////////////////////////////////
        void appendPathComponent(const std::string& pc);
        //////////////////////////////////////////////////////////////////////////////
        /// Returns the part of the path after the last /
        /// @since 1.4b1
        //////////////////////////////////////////////////////////////////////////////
        std::string filename() const; // last path component; if the URI ends in a trailing slash, returns the empty string.
        //////////////////////////////////////////////////////////////////////////////
        /// Returns true if the URI appears to be a localhost address
        /// @see resetValidLocalhost
        /// @see registerValidLocalhost
        /// @since 1.4b1
        //////////////////////////////////////////////////////////////////////////////
        bool isLocalhost() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public void FB::URI::parse_query_data(const std::string& in_str)
        ///
        /// @brief  Parses a urlencoded QueryString and stores the results in the URI
        ///
        /// @code
        /// FB::URI uri("http://www.firebreath.org");
        /// uri.parse_query_data("user=taxilian&firebreath=awesome");
        /// // uri.query_data["user"] == "taxilian"
        /// // uri.query_data["firebreath"] == "awesome"
        /// @endcode
        ///
        /// @param  in_str  urlencoded querystring (minus the ?)
        /// @returns void
        /// @since 1.4b1
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void parse_query_data(const std::string& in_str);
    };
};

#endif // H_FB_URI

