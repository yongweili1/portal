
#ifndef MCSF_LOG_CONNECTION_MANAGER_H
#define MCSF_LOG_CONNECTION_MANAGER_H

#include <set>
#include <boost/noncopyable.hpp>
#include "mcsf_log_connection.h"

NS_BEGIN

class connection_manager
  : private boost::noncopyable
{
public:
	void start(connection_ptr c);
	void stop(connection_ptr c);
	void stop_all();

private:
	std::set<connection_ptr> _connections;
};

NS_END 

#endif // MCSF_LOG_CONNECTION_MANAGER_H
