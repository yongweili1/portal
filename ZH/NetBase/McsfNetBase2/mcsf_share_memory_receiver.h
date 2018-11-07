////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_share_memory_receiver.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/05
///
/// \brief   create a shared memory object to receive raw data in same PC
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_SMFILERECEIVER_H
#define _MCSF_SMFILERECEIVER_H

#include <string>
#include "mcsf_blobdata_interface.h"


namespace Mcsf {
    
     ///  \class SMFileReceiver
     ///  \brief receive raw data by shared memory
    class SMFileReceiver : public IDataReceiver
    {
    public:
        SMFileReceiver(const std::string& sName);
         ~SMFileReceiver();
        /// get data by share memory
        /// 
        /// \param   size         the len of raw data
        /// \return  void*        the start addr of raw data
        boost::shared_array<char> GetRawData( size_t& size , bool bIsSyncCmd = false);
    private:
        void Destory(bool bIsSyncCmd);

        std::string m_sSHMKey;              /// the underly mmap file name
    };
}

#endif
