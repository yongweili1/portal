
//////////////////////////////////////////////////////////////////////////
///
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2012
///  All rights reserved.
///
///  \author   LiYongWei (yongwei.li@united-imaging.com)
///
///  \file     mcsf_communication_node_name.h
///  \brief    create , parse , get peer communictaionproxy name
///
///
///  \version  1.0
///  \date     2012-1-15
/// 
//////////////////////////////////////////////////////////////////////////
#ifndef _MCSF_COMMUNICATION_NODE_NAME_H_
#define _MCSF_COMMUNICATION_NODE_NAME_H_

#include <string>
#include <vector>
#include "mcsf_netbase_export.h"

#ifdef SWIG
#define McsfNetBase_Export
#endif

namespace Mcsf {

    class McsfNetBase_Export CommunicationNodeName
    {
    public:
        /////////////////////////////////////////////////////////////////
        ///  \brief Create communication proxy name according three elements:
        ///    application name,type name ,application instance index
        ///
        ///  \param[in]     sAppName--application name,e.g. "Viewer"
        ///  \param[in]     sTypeName--type name,e.g. "FE"
        ///  \param[in]     sInstanceIndex--application instance index,e.g. "0001"
        ///      
        ///  \param[out]    None
        ///  \return        std::string, empty if fail
        ///   e.g.:Viewer@FE@0001
        ///
        ///  \pre \e  
        /////////////////////////////////////////////////////////////////
        static std::string CreateCommunicationProxyName(const std::string& sAppName, 
            const std::string& sTypeName="", const std::string& sInstanceIndex="");

        /////////////////////////////////////////////////////////////////
        ///  \brief Get peer communication proxy name according to the other name.
        ///    application name,type name ,application instance index
        ///
        ///  \param[in]     sCommunicationProxyName--communication proxy name,e.g. "Viewer@FE@0001"
        ///  \param[in]     sTypeName--type name,e.g. "BE"
        ///  
        ///  \param[out]    None
        ///  \return        std::string empty if fail
        ///   e.g.:Viewer@BE@0001
        ///
        ///  \pre \e  
        /////////////////////////////////////////////////////////////////
        static std::string GetPeerCommunicationProxyName(const std::string& sCommunicationProxyName, 
            const std::string& sPeerType);

        ////////////////////////////////////////////////////////////////////////////////
        ///  \brief Parse communication proxy name. Each part:
        ///    application name,type name ,application instance index
        ///    
        ///
        ///  \param[in]     sCommunicationProxyName--communication proxy name,e.g. "Viewer@FE@0001"
        ///
        ///  \param[out]     sAppName
        ///  \param[out]     sTypeName
        ///  \param[out]     sInstanceIndex
        ///  
        ///  \return        true if parsing success, else false
        ///
        ///  \pre \e  
        ////////////////////////////////////////////////////////////////////////////////
        static bool ParseCommunicationProxyName( const std::string& sCommunicationProxyName, 
            std::string& sAppName, std::string& sTypeName, std::string& sInstanceIndex );
    };

}

#endif