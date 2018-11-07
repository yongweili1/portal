//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfLogClient
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2011
///  All rights reserved.
///  \author        xiaotao.yang 
///  \email         mailto:xiaotao.yang@united-imaging.com
///  \file          mcsf_log_xml_parse.h
///  \brief         parse the log client xml configuration file
///  \date          September. 20, 2011
///
///   \revised by  xiaotao.yang  mailto:xiaotao.yang@united-imaging.com
///   \date   Nov.20, 2011
///  \{
//////////////////////////////////////////////////////////////////////////
#include "../stdafx.h"

#include "mcsf_log_xml_parse.h"

namespace Mcsf
{

    /////////////////////////////////////////////////////////////////
    ///	\brief: constructor
    /////////////////////////////////////////////////////////////////
    CLogXmlParse::CLogXmlParse()
    {
        try
        {
            m_pConfigXmlDocument = new TiXmlDocument();
            m_pXmlElement = NULL;
            m_pRootXmlElement = NULL;
        }
        catch (...)
        {
        }
    }

    /////////////////////////////////////////////////////////////////
    ///	\brief: destructor
    /////////////////////////////////////////////////////////////////
    CLogXmlParse::~CLogXmlParse()
    {
        try
        {
            if (NULL != m_pConfigXmlDocument)
            {
                delete m_pConfigXmlDocument;
                m_pConfigXmlDocument = NULL;
            }
        }
        catch (...)
        {
        }
    }

    /////////////////////////////////////////////////////////////////
    ///	\brief: load the configuration file
    ///	\param:
    ///			[in] const std::string& strConfigFile: the configuration file
    ///	\return: true:  success to load configuration file,
    ///          false: fail to load configuration file.
    /////////////////////////////////////////////////////////////////
    bool CLogXmlParse::LoadConfigXmlFile(const std::string& strConfigFile)
    {
        try
        {
            if (strConfigFile.empty())
            {
                return false;
            }
            return m_pConfigXmlDocument->LoadFile(strConfigFile.c_str());
        }
        catch (std::exception &ex)
        {
            std::cout << "SW_LOGGER ERROR: "
                << "Failed CLogXmlParse::LoadConfigXmlFile !" << ex.what()
                << std::endl;
            return false;
        }
        catch (...)
        {
            std::cout << "SW_LOGGER ERROR: "
                << "Failed CLogXmlParse::LoadConfigXmlFile!"
                << std::endl;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////
    ///	\brief: load the configuration file
    ///	\param:
    ///			[in] const std::string& strXmlElementName: the xml tag
    ///			[out] const std::string& strConfigFile: the content of the tag
    ///	\return: true: success to get tag content , false:fail to get tag content
    /////////////////////////////////////////////////////////////////
    bool CLogXmlParse::GetContentByXmlTag(const std::string& strXmlElementName,
        std::string& strXmlElementContent)
    {
        try
        {
            if (strXmlElementName.empty())
            {
                return false;
            }

            m_pRootXmlElement = m_pConfigXmlDocument->RootElement();
            if (NULL == m_pRootXmlElement)
            {
                return false;
            }
            m_pXmlElement = m_pRootXmlElement->FirstChildElement();
            while (m_pXmlElement)
            {
                std::string strElementName = m_pXmlElement->Value();
                if (0 == strXmlElementName.compare(strElementName))
                {
                    const char* strTempString = m_pXmlElement->GetText();
                    if (NULL == strTempString)
                    {
                        return false;
                    }

                    strXmlElementContent = strTempString;
                    return true;
                }
                m_pXmlElement = m_pXmlElement->NextSiblingElement();
            }

            return false;
        }
        catch (...)
        {
            std::cout << "SW_LOGGER ERROR: "
                << "Failed CLogXmlParse::GetContentByXmlTag"
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

            m_pRootXmlElement = m_pConfigXmlDocument->RootElement();
            if (!m_pRootXmlElement)
            {
                return false;
            }

            m_pXmlElement = m_pRootXmlElement->FirstChildElement();
            while (m_pXmlElement)
            {
                std::string strTempElemName = m_pXmlElement->Value();
                if (0 == strElemName.compare(strTempElemName))
                {
                    /// We find the element
                    /// then we retrieve its strProperty value
                    strPropertyValue = m_pXmlElement->Attribute(strProperty.c_str());
                    if (0 == strPropertyValue.length())
                    {
                        return false;
                    }

                    return true;
                }

                /// strElemName is not the same as m_pElem->Value()
                /// try its sibling
                m_pXmlElement = m_pXmlElement->NextSiblingElement();
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
}
