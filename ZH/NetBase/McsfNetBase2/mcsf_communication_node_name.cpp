#include "mcsf_communication_node_name.h"

namespace 
{
    static const char* kpDelimiter = "@";
}

namespace Mcsf
{
    std::string CommunicationNodeName::CreateCommunicationProxyName(
        const std::string& sAppName, 
        const std::string& sTypeName, 
        const std::string& sInstanceIndex )
    {
        if( std::string::npos != sAppName.find(kpDelimiter) ||
            std::string::npos != sTypeName.find(kpDelimiter) ||
            std::string::npos != sInstanceIndex.find(kpDelimiter) )
        {
            return "";
        }

        if( sAppName.empty() )
        {
            return "";
        }
        std::string sProxyName;
        sProxyName = sAppName + kpDelimiter + sTypeName + kpDelimiter + sInstanceIndex;
        return sProxyName;
    }

    std::string CommunicationNodeName::GetPeerCommunicationProxyName( 
        const std::string& sCommunicationProxyName, const std::string& sPeerType )
    {
        if( sPeerType.empty() )
        {
            return "";
        }

        if( std::string::npos == sCommunicationProxyName.find( kpDelimiter ) )
        {
            //NTB_LOG_ERROR_WITH_THIS("communication proxy name is wrong, not including @");
            return "";
        }

        std::string sAppName;
        std::string sTypeName;
        std::string sIndex;
        if( !ParseCommunicationProxyName( sCommunicationProxyName, sAppName, sTypeName, sIndex ) )
        {
            return "";
        }

        return CreateCommunicationProxyName( sAppName, sPeerType, sIndex );
    }

    bool CommunicationNodeName::ParseCommunicationProxyName( 
        const std::string& sCommunicationProxyName,
        std::string& sAppName,
        std::string& sTypeName,
        std::string& sInstanceIndex )
    {
        sAppName.clear();
        sTypeName.clear();
        sInstanceIndex.clear();

        if( sCommunicationProxyName.empty() )
        {
            return false;
        }

        size_t indexAPP = sCommunicationProxyName.find( kpDelimiter, 0 );
        if( indexAPP == std::string::npos )
        {
            //NTB_LOG_ERROR_WITH_THIS("communication proxy name is wrong, not including @");
            return false;
        }

        sAppName = sCommunicationProxyName.substr( 0, indexAPP );

        size_t indexType = sCommunicationProxyName.find( kpDelimiter, indexAPP+1 );
        if( indexType == std::string::npos )
        {
            sAppName.clear();
            return false;
        }
        sTypeName = sCommunicationProxyName.substr( indexAPP+1, indexType-indexAPP-1 );

        // three "@" 
        if( sCommunicationProxyName.find( kpDelimiter, indexType+1 ) != std::string::npos )
        {
            //NTB_LOG_ERROR_WITH_THIS("communication proxy name is wrong, format has three @");
           sAppName.clear();
            sTypeName.clear();
            return false;
        }

        sInstanceIndex = sCommunicationProxyName.substr( indexType+1 );
        // like "app@type@"
        return true;
    }
}
