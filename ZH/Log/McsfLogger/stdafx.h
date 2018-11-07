// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef mcsf_log_stdafx_h__
#define mcsf_log_stdafx_h__

#include "mcsf_log_common.h"
#include "mcsf_log_processor.h"

#include "mcsf_log_do_parsing.h"
#include "mcsf_log_socket_sender.h"
#include "mcsf_log_buffer.h"

#if defined MCSF_PLATFORM_WIN32
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif 

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <set>
#include <string>
#include <deque>

#include <boost/lexical_cast.hpp>
#include <iostream>

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
#include <sys/types.h>

#endif



#endif //mcsf_log_stdafx_h__



