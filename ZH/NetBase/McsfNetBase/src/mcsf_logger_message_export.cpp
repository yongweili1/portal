#include "stdafx.h"

namespace Mcsf
{
    bool PackingMessageLogger::CreateLogger(const std::string& /*configName*/, const std::string& /*loggerName*/)
    {
        //return LogMessage::CreateLogger(configName, loggerName);
        return true;
    }

    void PackingMessageLogger::RelaseLogger(const std::string& /*loggerName*/)
    {
     //  LogMessage::RelaseLogger(loggerName);
    }
}