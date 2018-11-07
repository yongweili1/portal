//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfLogServer
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2012
///  All rights reserved.
///  \author        Bo Han 
///  \email         mailto:bo.han.ext@united-imaging.com
///  \file          mcsf_log_server_xml_parser.h
///  \brief         generic class for parsing XML-based configuration file
///  \date          August. 04, 2012
///
///   \revised by  
///   \date   
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef MCSF_LOG_XML_PARSER_H_
#define MCSF_LOG_XML_PARSER_H_

#ifdef _WIN32
#pragma warning(disable:4819)  // disable boost warnings
#endif

#include <string>
#include <boost/shared_ptr.hpp>
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"

namespace Mcsf {

	/////////////////////////////////////////////////////////////////
	///	\brief: generic class for parsing XML-based configuration file.
	/////////////////////////////////////////////////////////////////

	class CLogXmlParse
	{
	private:
		typedef boost::shared_ptr<TiXmlDocument> TiXmlDocPtr;
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
		///	\return: true: load the server configuration file successfully 
		///			 false: fail to load the server configuration file
		/////////////////////////////////////////////////////////////////
		bool LoadConfigXmlFile(const std::string &strConfigFile);

		/////////////////////////////////////////////////////////////////
		///	\brief: retrieve the relative value of a given tag of the 
		///         configuration file
		///	\param:
		///			[in] const std::string& strElemName: the xml tag
		///			[out] std::string& strElemValue: the value of the tag
		///	\return: true: retrieve the value of a given tag successfully
		///          false: fail to retrieve the value
		/////////////////////////////////////////////////////////////////
		bool GetContentByXmlTag(const std::string &strElemName,
			std::string &strElemValue);

		/////////////////////////////////////////////////////////////////
		///	\brief: write the tag-value pair into the configuration file
		///	\param:
		///			[in] const std::string& strElemName: the xml tag
		///			[out] const std::string& strElemValue: the value of the tag
		///	\return: true: write the pair of successfully
		///          false: fail to write the pair
		/////////////////////////////////////////////////////////////////
		bool WriteContentByXmlTag(const std::string &strElemName,
			const std::string &strElemValue);

        /////////////////////////////////////////////////////////////////
        ///	\brief: retrieve the relative value of a given tag of the 
        ///         configuration file
        ///	\param:
        ///			[in] const std::string& strElemPath: the xml tag path,
        ///                  such as Version/ProductInfo/SoftwareFullVersion
        ///			[out] const std::string& strElemValue: the value of the tag
        ///	\return: true: retrieve the value of a given tag successfully
        ///          false: fail to retrieve the value
        /////////////////////////////////////////////////////////////////
        bool GetContentByXmlPath(const std::string& strElemPath, std::string& strElemValue);
        
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

        /////////////////////////////////////////////////////////////////
        ///	\brief: traverse the xml file to get all node info
        ///	\param:
        ///			[out] const std::string& strLogHeaderInfo: 
        ///                     store the log header info
        ///	\return: true for success, while false for failure.
        /////////////////////////////////////////////////////////////////
        bool GetLogHeaderInfo(std::string &strLogHeaderInfo);

	private:

		// smart pointer pointing to the configuration file
		TiXmlDocPtr m_pDoc;

		// root element of XML configuration file
		TiXmlElement *m_pRootElem;

		// generic element of XML configuration file
		TiXmlElement *m_pElem;
	};
}

#endif // MCSF_LOG_XML_PARSER_H_
