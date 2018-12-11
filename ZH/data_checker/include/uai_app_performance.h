//////////////////////////////////////////////////////////////////////////
/// \defgroup uAI
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2018
///  All rights reserved.
///
///  \author  qiangqiang.zhou     mailto:qiangqiang.zhou@united-imaging.com
///
///  \file    uAI App 
///  \brief   performance test
///
///  \version 1.0
///  \date    June 15, 2018
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef UAI_APP_PERFORMANCE_H_
#define UAI_APP_PERFORMANCE_H_

#include <Windows.h>

#define TEST_PERFORMANCE_INIT \
    LARGE_INTEGER BeginTime; \
    LARGE_INTEGER EndTime; \
    LARGE_INTEGER Frequency; \
    double dTime(0); \
    std::string sDes("");\
    QueryPerformanceFrequency(&Frequency);

#define TEST_PERFORMANCE_BEGIN \
    QueryPerformanceCounter(&BeginTime);

#define TEST_PERFORMANCE_END(descript) \
    QueryPerformanceCounter(&EndTime); \
    dTime = (double)(EndTime.QuadPart - BeginTime.QuadPart)/Frequency.QuadPart; \
    sDes = (std::string)descript;\
    printf("\n[%.2f] ms \tFOR [%s]\n", dTime*1000,sDes.c_str());

#endif