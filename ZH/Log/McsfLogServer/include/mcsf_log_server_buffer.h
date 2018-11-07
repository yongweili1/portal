#ifndef mcsf_log_server_buffer_h__
#define mcsf_log_server_buffer_h__
#include <boost/shared_array.hpp>
#include "mcsf_log_file.h"

#define DEFAULT_BUFFER_SIZE 65535
namespace Mcsf{namespace Log{

class log_server_buffer
{
public:
	log_server_buffer()
		: p_(0)
	{
		try
		{
			buffer_.reset(new char[DEFAULT_BUFFER_SIZE]);
		}
		catch(const std::bad_alloc& ex)
		{
			std::cout << "fail to alloc memory with message" << ex.what() << std::endl;
		}
		catch(...)
		{
			std::cout << "fail to alloc memory with unknown exception!" << std::endl;
		}
	}

	char* getp()
	{
		return buffer_.get() + p_;
	}

	std::size_t psize()
	{
		return DEFAULT_BUFFER_SIZE - p_ ;
	}

	void write(std::size_t received_length, boost::uint64_t &deltaTime)
	{
		p_ += received_length;
		std::size_t terminator_pos = 0;
		bool found = find_terminator(terminator_pos);
		if(found)
		{
			std::size_t pos = terminator_pos + 1;
			LogFile::instance()->Write(buffer_, pos, deltaTime);
            p_ = p_ - pos;//erase these been wrote
			if(p_ > 0)
				memcpy(buffer_.get(), buffer_.get() + pos, p_);
		}
		else
		{
            if(psize() <= 0)
            {
                p_ = 0;
            }
		}
	}

private:
	bool find_terminator(std::size_t& ret)
	{
		char* logdata = buffer_.get();
		ret = p_;
		while(ret > 1)
		{
			--ret;
			if(LOG_TERMINATOR == logdata[ret - 1] && '\n' == logdata[ret])
			{
				return true;
			}
		} 
		return false;
	}


private:
	boost::shared_array<char> buffer_;
	std::size_t p_;
};

}}
#endif // mcsf_log_server_buffer_h__