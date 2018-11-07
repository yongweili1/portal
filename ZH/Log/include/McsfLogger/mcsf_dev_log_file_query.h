#ifdef _MSC_VER
#pragma once
#endif

#ifndef __mcsf_dev_log_file_query_h__
#define __mcsf_dev_log_file_query_h__

#include <string>

#include "mcsf_logviewer_interface_common.h"

#ifdef SWIG
#define MCSF_LOGVIEWER_INTERFACE
#endif

namespace Mcsf { 

namespace LogViewerInterface {

    class ICompleteHandler
    {
    public:
        virtual ~ICompleteHandler() {}
        virtual int CallHandler(const std::string & sLogQueryPath)
        {
            sLogQueryPath;
            return 0; 
        }
    };

class MCSF_LOGVIEWER_INTERFACE DevLogFileQuery
{
public:
    enum
    {
        log_type_svc,
        log_type_dev,
    };

    DevLogFileQuery();

    DevLogFileQuery(const std::string &sRoot, bool bCompressed, int iLogType);

    ~DevLogFileQuery();

    bool SetAttribute(const std::string &sRoot, bool bCompressed, int iLogType);

    bool RegCompleteHandler(ICompleteHandler *pHandler);

    bool StartQuery(unsigned long long nStartTime, unsigned long long nEndTime);

    void StopQuery();

private:
    void Run(unsigned long long nStartTime, unsigned long long nEndTime);
    std::string CompressDir(const std::string &sDir);

private:
    std::string m_sRoot;
    bool m_bStopTag;
    ICompleteHandler *m_pCompleteHandler;
    std::string m_sBaseLogPath;
    bool m_bCompressed;
};

}}
#endif