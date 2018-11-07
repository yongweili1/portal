//c++  header
#include "stdafx.h"


namespace Mcsf{
    ICommunicationProxy* CommunicationProxyFactory::Init( const std::string& /*sConfigFile*/ )
    {
//         try
//         {
//             CommunicationProxy* m_pCommProxy = new CommunicationProxy();
//             ContaineeConfigurator config( sConfigFile );
// 
//             //set containee name 
//             std::string sContainerName = config.GetComProxyName();
//             m_pCommProxy->SetName( sContainerName );
//             //set channel id
//             m_pCommProxy->SetSenderChannelId( config.GetEventSendChannel() );
// 
//             ChannelIdMap idMap;
//             config.GetEventListenChannels( idMap );
// 
//             std::string sSystemDispatcherAddr = config.GetSystemDispathAddress();
//             if( !sSystemDispatcherAddr.empty() )
//             {
//                 m_pCommProxy->CheckCastToSystemDispatcher( sSystemDispatcherAddr );
//                 m_pCommProxy->SubscribeEvent( idMap );
//             }
// 
//             std::string sListenAddr = config.GetListenAddress();
//             m_pCommProxy->StartListener( sListenAddr );
// 
//             return m_pCommProxy;
// 
//         }
//         catch(std::runtime_error e)
//         {
//             // TODO:add exception handle
//             ACE_DEBUG((LM_DEBUG,"%s\r\n",e.what()));
//         }

        return 0;
    }

}
