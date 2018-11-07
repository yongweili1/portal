////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_sync_result.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/05
///
/// \brief  represent an synchronous command result 
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_SYNC_RESULT_H
#define _MCSF_SYNC_RESULT_H

namespace Mcsf 
{
    class SyncResult : public ISyncResult
    {
        friend class CommunicationProxy;
    public:
        SyncResult() : m_sResult("")
        {
            // default failed
            m_iResult = -1;
        }

        /// get the response marshal result
        virtual std::string GetSerializedObject() 
        {
            return m_sResult;
        }

        /// set the response result
        virtual void SetSerializedObject( const std::string& sResult )
        {
            m_sResult = sResult;
        }

        /// get the call result
        virtual int GetCallResult() {
            return m_iResult;
        }

        /// set call result
        void SetCallResult(int iret)
        {
            m_iResult = iret;
        }

    private:
        int m_iResult;              /// the request call result
        std::string m_sResult;      /// marshaled response result
    };
}

#endif