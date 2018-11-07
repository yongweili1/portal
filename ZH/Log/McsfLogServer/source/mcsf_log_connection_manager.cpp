#include "stdafx.h"
#include "mcsf_log_connection_manager.h"
#include <algorithm>
#include <boost/bind.hpp>

NS_BEGIN

void connection_manager::start(connection_ptr c)
{
	_connections.insert(c);
	c->start();
}

void connection_manager::stop(connection_ptr c)
{
	std::cout<< "connection destroy"<<std::endl;
	_connections.erase(c);
	c->stop();
}

void connection_manager::stop_all()
{
	std::for_each(_connections.begin(), _connections.end(),
		boost::bind(&connection::stop, _1));
	_connections.clear();
}

NS_END
