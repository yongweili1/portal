////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2012
/// All rights reserved
///
/// \file   mcsf_swig_sharedarray_util.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2012/02/13
///
/// \brief  wrap class for swig generated code
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_SWIG_SHAREDARRAY_UTIL_H_
#define MCSF_SWIG_SHAREDARRAY_UTIL_H_

#include "boost/shared_array.hpp"
#include "boost/shared_ptr.hpp"

#include "mcsf_netbase_command_handler.h"
#include "mcsf_netbase_ibigdata_handler.h"

namespace Mcsf { namespace internal {
    
    class SwigSharedCharArrayUtil 
    {
    public:        
        static void* Get(boost::shared_array<char>* pArray)
        {
            return (void*)pArray->get();
        }

        static void Reset(boost::shared_array<char>* pArray, unsigned int iLen)
        {
            pArray->reset(new char[iLen]);
        }

        static void Destroy(boost::shared_array<char>* pArray)
        {
            delete pArray;
        }

        static int UseCount(boost::shared_array<char>* pArray)
        {
            return pArray->use_count();
        }

        static boost::shared_ptr<ICommandHandlerEx> New(ICommandHandlerEx* ptr)
        {
            return boost::shared_ptr<ICommandHandlerEx>(ptr);
        }

        static void Destory(boost::shared_ptr<ICommandHandlerEx>* ptr)
        {
            delete ptr;
        }

        static boost::shared_ptr<IDataHandlerEx> New(IDataHandlerEx* ptr)
        {
            return boost::shared_ptr<IDataHandlerEx>(ptr);
        }

        static void Destory(boost::shared_ptr<IDataHandlerEx>* ptr)
        {
            delete ptr;
        }

        static boost::shared_ptr<ICommandCallbackHandlerEx> New(ICommandCallbackHandlerEx* ptr)
        {
            return boost::shared_ptr<ICommandCallbackHandlerEx>(ptr);
        }

        static void Destory(boost::shared_ptr<ICommandCallbackHandlerEx>* ptr )
        {
            delete ptr;
        }

        static boost::shared_array<char> CopyToSharedArray(const std::string &strArray)
        {
            if (strArray.empty())
                return NULL;
            char *arr = new char[strArray.length()];
            memcpy(arr, &*strArray.begin(), strArray.length());
            return arr;
        }
    };
}}

#endif // MCSF_SWIG_SHAREDARRAY_UTIL_H_

