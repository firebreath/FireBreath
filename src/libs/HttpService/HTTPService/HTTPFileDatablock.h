/*
 *  HTTPFileDatablock.h
 *  fbplugin
 *
 */

#pragma once
#ifndef H_HTTP_HTTPFILEDATABLOCK
#define H_HTTP_HTTPFILEDATABLOCK

#include "../Platform/windows_defs.h"
#include "HTTPCommon/HTTPDatablock.h"

#undef BOOST_HAS_RVALUE_REFS // dunno why this is needed, but it is
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace HTTP {
    class HTTPFileDatablock : public HTTPDatablock {
    public:
        HTTPFileDatablock(const std::string& fp) : mmfile(fp.c_str(), boost::interprocess::read_only), region(mmfile, boost::interprocess::read_only) {}
        virtual ~HTTPFileDatablock() {}
        
        virtual size_t size() const {
            return region.get_size();
        }
        virtual const char* data() const {
            return reinterpret_cast<const char*>(region.get_address());
        }
    protected:
        boost::interprocess::file_mapping mmfile;
        boost::interprocess::mapped_region region;
    };
};

#endif

