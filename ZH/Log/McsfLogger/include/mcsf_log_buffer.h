//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2012
///  All rights reserved.
/// 
///  \author  Sun Lei (mailto:lei.sun@united-imaging.com)
/// 
///  \file mcsf_log_buffer.h
/// 
///  \brief LogBuffer processor declaration
/// 
///  \version 1.0
/// 
///  \date    August/1/2012
//////////////////////////////////////////////////////////////////////////
#ifndef mcsf_log_buffer_h__
#define mcsf_log_buffer_h__

#include <set>
#include <string>
#include <deque>
#include "mcsf_log_common.h"
#include "mcsf_log_processor.h"

NS_BEGIN

class MCSF_LOG_API LogBuffer: public IProcessor
{
public:
	LogBuffer();

	/// \brief		process log data from processor node queue
	/// \param		rawdata, pointer of log data from processor node queue
	/// \return		a log data pointer, the processed result.
	/// copy rawdata to a 65535Bytes buffer.
	LogPtr Process(const LogPtr& rawdata);

	/// \brief		clean data in processor's buffer
	/// \param		
	/// \return		a log data pointer, the processed result.
	LogPtr Clean() ;

	void Dump(FileProxyPtr file);

private:
	CharArray _buffer;
	size_t _pos;
};

NS_END

#endif // mcsf_log_buffer_h__