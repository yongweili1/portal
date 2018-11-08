//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2012
///  All rights reserved.
/// 
///  \author  Sun Lei (mailto:lei.sun@united-imaging.com)
/// 
///  \file mcsf_log_buffer.cpp
/// 
///  \brief LogBuffer processor definition
/// 
///  \version 1.0
/// 
///  \date    August/1/2012
//////////////////////////////////////////////////////////////////////////
#include "mcsf_log_buffer.h"
#include <boost/make_shared.hpp>
#include <iostream>
NS_BEGIN

static const size_t LOG_BUFFER_SIZE = 65536;
//static const size_t BUFFER_T_LEN = 2;//sizeof(std::size_t);

LogBuffer::LogBuffer()
    : _buffer(new char[LOG_BUFFER_SIZE])
    , _pos(0)
{
    is_loop_runing = true;
}

/// \brief		clean data in processor's buffer
/// \param		
/// \return		a log data pointer, the processed result.
LogPtr LogBuffer::Clean()
{
    try {
        if (_pos > 0)
        {
            boost::shared_ptr<RawLog> ret(new RawLog());
            ret->_data = _buffer;
            ret->_length = _pos;
            _buffer.reset(new char[LOG_BUFFER_SIZE]);
            _pos = 0;
            return ret;
        }
    }
    catch (...)
    {
        ;
    }
    return LogPtr();
}

void LogBuffer::Dump(FileProxyPtr file)
{
    file->Write(_buffer.get(), _pos);
}

/// \brief		process log data from processor node queue, copy log data to 
///				a bigger buffer
/// \param		rawdata, pointer of log data from processor node queue
/// \return		a log data pointer, the processed result.
LogPtr LogBuffer::Process(const LogPtr& logdata)
{
    LogPtr ret = LogPtr();
    try
    {
        if (NULL != logdata && 0 != logdata->_length && logdata->_length <= LOG_BUFFER_SIZE)
        {
            std::size_t log_len = logdata->_length;
            if (_pos > 0 && _buffer && log_len + _pos > LOG_BUFFER_SIZE)
            {
                ret = boost::make_shared<RawLog>();
                ret->_data = _buffer;
                ret->_length = _pos;
                _pos = 0;
            }

            if (0 == _pos || !_buffer)
            {
                _buffer.reset(new char[LOG_BUFFER_SIZE]);
                _pos = 0;
            }
            memcpy(_buffer.get() + _pos, logdata->_data.get(), logdata->_length);
            _pos += logdata->_length;
        }
    }
    catch (...)
    {
        _buffer.reset();
        _pos = 0;
    }
    return ret;
}
NS_END
