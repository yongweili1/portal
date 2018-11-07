////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_swig_string_util.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/22
///
/// \brief  wrapper for swig code use
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_SWIG_STRING_UTIL_H_
#define MCSF_SWIG_STRING_UTIL_H_

#include <string>

namespace Mcsf { namespace internal {
 
    class SwigStringUtil 
    {
    public:
        static int GetSize( std::string* pConent )  
        {
            return NULL == pConent ? 0 : pConent->size();
        }

        static void* StringAsArray( std::string* pConent )
        {
            return pConent->empty() ? NULL : &*(pConent->begin());
        }

        static void ResizeUninitialized(std::string* pConent, size_t newSize)
        {
            if (NULL != pConent)
            {
                pConent->resize( newSize );
            }
        }

        static std::string* NewStdString()
        {
            return new std::string("");
        }

        static void DestroyStdString( std::string* str )
        {
            if (NULL != str)
            {
                delete str;
            }
        }

        static const char* ToCStr( std::string* str )
        {
            return NULL == str ? "" : str->c_str();
        }

        static std::string ToStdString( const char* str )
        {
            return NULL == str ? "" : std::string(str);
        }
    };
}}

#endif // MCSF_SWIG_STRING_UTIL_H_
