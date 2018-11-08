#include "mcsf_share_memory_receiver.h"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace Mcsf {

    SMFileReceiver::SMFileReceiver( const std::string& sName
        ) : m_sSHMKey( sName )
    {

    }    
    
    SMFileReceiver::~SMFileReceiver()
    {
    }

    boost::shared_array<char> SMFileReceiver::GetRawData( size_t& size , bool bIsSyncCmd)
    {
        using namespace boost::interprocess;
  
        try {
            shared_memory_object shm(open_only, m_sSHMKey.c_str(), read_only);

            mapped_region region(shm, read_only);
            size = region.get_size();
            boost::shared_array<char> pData(new char[size]);

            memcpy(pData.get(), region.get_address(), size);
            return pData;
        }
        catch (...) {
            return nullptr;
        }
    }

    void SMFileReceiver::Destory(bool bIsSyncCmd)
    {
        if (!m_sSHMKey.empty())
        {
            boost::interprocess::shared_memory_object::remove(m_sSHMKey.c_str());
        }
    }


}

