////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_netbase_isync_result.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/07
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_NETBASE_ISYNC_RESULT_H_
#define MCSF_NETBASE_ISYNC_RESULT_H_

#include <string>

namespace Mcsf {
    class ISyncResult 
    {
    public:
         
        /// Get the return result that serialized in string. mainly used in C# invoke 
        /// according to swig generated code.
        /// 
        /// \return  std::string the serialized result.
        virtual std::string GetSerializedObject() = 0;
        virtual void SetSerializedObject( const std::string& sResult ) = 0;
        virtual int GetCallResult() = 0;
        virtual ~ISyncResult() {}
    };
}

#endif // MCSF_NETBASE_ISYNC_RESULT_H_
