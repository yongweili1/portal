//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfLogServer
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2012
///  All rights reserved.
///  \author        Bo Han 
///  \email         mailto:bo.han.ext@united-imaging.com
///  \file          mcsf_log_server_xml_parser.cpp
///  \brief         generic class for parsing XML-based configuration file
///  \date          August. 04, 2012
///
///   \revised by  
///   \date   
///  \{
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "mcsf_log_server_xml_parser.h"
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

namespace Mcsf {

	/////////////////////////////////////////////////////////////////
	///	\brief: constructor
	/////////////////////////////////////////////////////////////////
	CLogXmlParse::CLogXmlParse()
		try
		: m_pDoc(new TiXmlDocument)
		, m_pRootElem(NULL)
		, m_pElem(NULL)
	{
	} catch (...)
	{
	}

	/////////////////////////////////////////////////////////////////
	///	\brief: destructor
	/////////////////////////////////////////////////////////////////
	CLogXmlParse::~CLogXmlParse() 
	{
	}

	/////////////////////////////////////////////////////////////////
	///	\brief: load the configuration file
	///	\param:
	///			[in] const std::string& strConfigFile: the configuration file
	///	\return: true: load the server configuration file successfully 
	///			 false: fail to load the server configuration file
	/////////////////////////////////////////////////////////////////
	bool CLogXmlParse::LoadConfigXmlFile(const string &strConfigFile)
	{
		try 
		{
			if (strConfigFile.empty()) 
			{
				std::cerr << "Read Configuration File Failed" << std::endl;
				return false;
			}
		    return m_pDoc->LoadFile(strConfigFile.c_str());
		} 
		catch(...)
		{
			std::cerr << "SW_LOGGER ERROR"
				<< "CLogXmlParse::LoadFile Failed!"
				<< endl;

			return false;
		}
	}

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
    bool CLogXmlParse::GetContentByXmlPath(const std::string& strElemPath, std::string& strElemValue)
    {
        try
        {
            std::vector<std::string> fields;
            split(fields, strElemPath, is_any_of("/"));

            m_pRootElem = m_pDoc->RootElement();
            if(!m_pRootElem)
            {
                return false;
            }
            int iSize = (int)(fields.size());
            for(int i = 1; i < iSize; ++i)
            {
                m_pElem = m_pRootElem->FirstChildElement(fields[i].c_str());
                if(!m_pElem)
                    return false;
                m_pRootElem = m_pElem;
            }

            if(m_pElem->FirstChild())
            {
                strElemValue = m_pElem->GetText();
            }
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

	/////////////////////////////////////////////////////////////////
	///	\brief: retrieve the relative value of a given tag of the 
	///         configuration file
	///	\param:
	///			[in] const std::string& strElemName: the xml tag
	///			[out] const std::string& strElemValue: the value of the tag
	///	\return: true: retrieve the value of a given tag successfully
	///          false: fail to retrieve the value
	/////////////////////////////////////////////////////////////////
	bool CLogXmlParse::GetContentByXmlTag(const string &strElemName,
		string &strElemValue)
	{
		try
		{
			if (strElemName.empty()) 
			{
				return false;
			}

			m_pRootElem = m_pDoc->RootElement();
			if (!m_pRootElem) 
			{
				return false;
			}

			/// Get the first tag
			m_pElem = m_pRootElem->FirstChildElement();
			while (m_pElem) 
			{
				string strTempElemName = m_pElem->Value();
				if (0 == strElemName.compare(strTempElemName)) 
				{
					/// strElemName is the same as m_pElem->Value()
					if(m_pElem->FirstChild())
					{
						strElemValue = m_pElem->GetText();
					}
					/// Get the value of strElemName
					if (0 == strElemValue.length()) 
					{
						return false;
					}

					return true;
				}

				/// strElemName is not the same as m_pElem->Value()
				/// try its sibling
				m_pElem = m_pElem->NextSiblingElement();
			}

			return false;
		} 
		catch (...)
		{
			std::cerr << "SW_LOGGER ERROR"
				<< "CLogXmlParser::GetContentByXmlTag Failed!"
				<< endl;

			return false;
		}
	}

	/////////////////////////////////////////////////////////////////
	///	\brief: write the tag-value pair into the configuration file
	///	\param:
	///			[in] const std::string& strElemName: the xml tag
	///			[out] const std::string& strElemValue: the value of the tag
	///	\return: true: write the pair of successfully
	///          false: fail to write the pair
	/////////////////////////////////////////////////////////////////
	bool CLogXmlParse::WriteContentByXmlTag(const string &strElemName, 
		const string &strElemValue)
	{
		try 
		{
			/// Tag name is empty, failed
			if (strElemName.empty()) 
			{
				return false;
			}

			m_pRootElem = m_pDoc->RootElement();
			if (!m_pRootElem) 
			{
				return false;
			}

			m_pElem = m_pRootElem->FirstChildElement();
			while (m_pElem) 
			{
				string strTempElemName = m_pElem->Value();
				if (0 == strElemName.compare(strTempElemName)) 
				{
					TiXmlElement *pTempTXElem = m_pElem->ToElement();
					/// Conversion failed
					if (!pTempTXElem) 
					{
						return false;
					}

					m_pElem->Clear();
					TiXmlText *pTempTXElemContent = new TiXmlText(strElemValue.c_str());
					pTempTXElem->LinkEndChild(pTempTXElemContent);

					/// Save modification to configuration file
					return m_pDoc->SaveFile();
				}

				m_pElem = m_pElem->NextSiblingElement();
			}

			return false;
		} 
		catch (...)
		{
			std::cerr << "SW_LOGGER ERROR"
				<< "CLogXmlParse::WriteContentByXmlTag Failed!"
				<< std::endl;

			return false;
		}
	}

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
	bool CLogXmlParse::GetPropertyValueByXmlTag(const std::string &strElemName
		, const std::string &strProperty, std::string &strPropertyValue)
	{
		try
		{
			if (strElemName.empty() || strProperty.empty())
			{
				return false;
			}

			m_pRootElem = m_pDoc->RootElement();
			if (!m_pRootElem)
			{
				return false;
			}

			m_pElem = m_pRootElem->FirstChildElement();
			while (m_pElem) 
			{
				string strTempElemName = m_pElem->Value();
				if (0 == strElemName.compare(strTempElemName)) 
				{
					/// We find the element
					/// then we retrieve its strProperty value
					strPropertyValue = m_pElem->Attribute(strProperty.c_str());
					if (0 == strPropertyValue.length())
					{
						return false;
					}

					return true;
				}

				/// strElemName is not the same as m_pElem->Value()
				/// try its sibling
				m_pElem = m_pElem->NextSiblingElement();
			}

			return false;
		}
		catch (...)
		{
			std::cerr << "SW_LOGGER ERROR"
				<< "CLogXmlParse::GetPropertyValueByXmlTag Failed!"
				<< std::endl;

			return false;
		}
	}

    bool CLogXmlParse::GetLogHeaderInfo(std::string &strLogHeaderInfo)
    {
        bool bRetVal = false;
        strLogHeaderInfo = "";
        try
        {
            m_pRootElem = m_pDoc->RootElement();
            if(!m_pRootElem)
                return false;
            m_pElem = m_pRootElem->FirstChildElement();
            while(m_pElem)
            {
                strLogHeaderInfo += ",";
                strLogHeaderInfo += m_pElem->Value();
                strLogHeaderInfo += ":";
                strLogHeaderInfo += m_pElem->GetText();
                m_pElem = m_pElem->NextSiblingElement();
            }
            bRetVal = true;
        }
        catch (const std::exception &ex)
        {
            std::cout << "exception with message:" << ex.what() << "in func[" << __FUNCTION__ << "]!" <<std::endl;
        }
        return bRetVal;
    }
} // namespace Mcsf
