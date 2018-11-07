////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_share_memory_sender.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/05
///
/// \brief  create a shared memory object to send raw data in same PC
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_SMFILESENDER_H
#define _MCSF_SMFILESENDER_H

#include <string>
#include "mcsf_blobdata_interface.h"


namespace Mcsf
{
    
     ///  \class SMFileSender
     ///  \brief send raw data by shared memory
    class SMFileSender : public IDataSender
    {
    public:
        SMFileSender();
        virtual ~SMFileSender();
         
        /// map \a pBuffer content to a shared memory object
        /// 
        /// \param   pBuffer          the start addr of send raw data
        /// \param   iLen        *    the len of send raw data
        /// \return  bool        *    return true if success
        bool SendRawData( boost::shared_array<char> pBuffer, size_t iLen );
        virtual void SetTimeOut( int iMillSecond );;

        virtual void SetName(const std::string& sName )
        {
            m_sSHMKey = sName;
        }

        std::string GetName() const {
            return m_sSHMKey;
        }

        virtual int GetSendType() const 
        {
            return send_data_by_sharememory;
        }

        void DestoryRawData( );

        void CloseDataFile();

        std::string Dump();

    private:
        std::string m_sSHMKey;          /// The key to identify different share-memory
        

        bool m_bIsClosed;
    }; 

}

#endif
