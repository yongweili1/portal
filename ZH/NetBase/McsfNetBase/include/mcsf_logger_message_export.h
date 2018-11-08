//////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United Imaging health care Inc., 2011
///  All rights reserved.
///
/// \file    mcsf_logger_message_export.h
///
/// \author  Li Yongwei(yongwei.li@united-imaging.com)
///
/// \date    2011-10-13
///
/// \brief   interface capsulation for c# calling
//////////////////////////////////////////////////////////////////////
#ifndef  _MCSF_LOGGER_MESSAGE_EXPORT_H_
#define _MCSF_LOGGER_MESSAGE_EXPORT_H_

#include <string>

namespace Mcsf
{
    class PackingMessageLogger
    {
    public:
        static bool CreateLogger(const std::string& configName, const std::string& loggerName);
        static void RelaseLogger(const std::string& loggerName);
    };
}

#endif