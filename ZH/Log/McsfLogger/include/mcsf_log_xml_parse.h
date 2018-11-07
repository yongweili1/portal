//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfLogClient
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2011
///  All rights reserved.
///  \author        LAI Dandan 
///  \email         mailto:dandan.lai@united-imaging.com
///  \file          mcsf_log_xml_parse.cpp
///  \brief         parse the log client xml configuration file
///  \date          September. 20, 2011
///
///   \revised by  fei.wang  mailto:fei.wang@united-imaging.com
///   \date   Nov.20, 2011
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef MCSF_LOG_XML_PARSE_H_
#define MCSF_LOG_XML_PARSE_H_

#if defined(MCSF_PLATFORM_WIN32)
#pragma warning(disable:4819)  // disable boost warnings
#endif

#include <string>
#include <iostream>
#include "boost/algorithm/string.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/format.hpp"
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"

//#include "mcsf_log_common.h"

namespace Mcsf
{
    /////////////////////////////////////////////////////////////////
    ///	\brief: the class of parse xml configuration file
    /////////////////////////////////////////////////////////////////

    class MCSF_LOG_API CLogXmlParse
    {
    public:

        /////////////////////////////////////////////////////////////////
        ///	\brief: constructor
        /////////////////////////////////////////////////////////////////
        CLogXmlParse();

        /////////////////////////////////////////////////////////////////
        ///	\brief: destructor
        /////////////////////////////////////////////////////////////////
        ~CLogXmlParse();

        /////////////////////////////////////////////////////////////////
        ///	\brief: load the configuration file
        ///	\param:
        ///			[in] const std::string& strConfigFile: the configuration file
        ///	\return: true: success to load configuration file , false:fail to load configuration file
        /////////////////////////////////////////////////////////////////
        bool LoadConfigXmlFile(const std::string& strConfigFile);

        /////////////////////////////////////////////////////////////////
        ///	\brief: load the configuration file
        ///	\param:
        ///			[in] const std::string& strXmlElementName: the xml tag
        ///			[out] const std::string& strConfigFile: the content of the tag
        ///	\return: true: success to get tag content , false:fail to get tag content
        /////////////////////////////////////////////////////////////////
        bool GetContentByXmlTag(const std::string& strXmlElementName, std::string& strXmlElementContent);

		/////////////////////////////////////////////////////////////////
		///	\brief: retrieve the relative value of a property
		///          a given tag from the configuration file
		///	\param:
		///			[in] const std::string& strElemName: the xml tag
		///         [in] const std::string &strProperty: the property name
		///			[out] std::string& strPropertyValue: the value of the property
		///	\return: true: retrieve the value of a given property successfully
		///          false: fail to retrieve the value
		/////////////////////////////////////////////////////////////////
		bool GetPropertyValueByXmlTag(const std::string &strElemName
			, const std::string &strProperty
			, std::string &strPropertyValue);

    private:

        // the pointer points to the configuration file
        TiXmlDocument   *m_pConfigXmlDocument;

        // the pointer points to root xml element
        TiXmlElement    *m_pRootXmlElement;

        // the pointer points to element of xml file
        TiXmlElement    *m_pXmlElement;
    };
}

#endif // MCSF_LOG_XML_PARSE_H_