#ifndef mcsf_log_file_proxy_h__
#define mcsf_log_file_proxy_h__

#include "mcsf_log_common.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>
#include <fstream>

class  FileProxy
{
public:
	FileProxy(const std::string& filename)
		: _filename(filename)
		, _ready(false)
	{
	}

	~FileProxy()
	{		
		try
		{
			_file_stream.close();
		}
		catch(std::exception e)
		{
			std::cout <<"file close failed <<" << e.what() << std::endl;
		}
	}

	bool Initialize()
	{
		try
		{
			std::string s_pathname = _filename;
			boost::filesystem::path pathname(s_pathname);
			pathname = boost::filesystem::system_complete(pathname);
			if(!boost::filesystem::exists(pathname))
			{
				std::string s_realpath = s_pathname.substr(0
					, s_pathname.find_last_of('/'));
				boost::filesystem::path real_path(s_realpath);
				real_path = boost::filesystem::system_complete(real_path);
				if(!boost::filesystem::exists(real_path))
				{
					boost::filesystem::create_directories(real_path);
				}
			}
			_file_stream.open(s_pathname.c_str(), std::ofstream::app 
				| std::ofstream::binary);
			if(_file_stream.is_open() && _file_stream.good())
			{
				_ready = true;
				return true;
			}
		}
		catch(std::exception e)
		{
			std::cout<<"file open failed!" << e.what() <<std::endl;
		}
		_ready = false;
		return false;
	}

	bool Ready()
	{
		return _ready;
	}

	void Write(const char* rawdata, size_t length)
	{
		try
		{
			if(length == 0 || NULL == rawdata)
				return ;

			_file_stream.write(rawdata, length);
			_file_stream.flush();
		}
		catch (std::exception e)
		{
			_file_stream.close();
			_ready = false;
			std::cout << "file write failed <<" << e.what() << std::endl;
		}
	}

private:
	std::string		_filename;
	std::ofstream	_file_stream;
	bool			_ready;
};

typedef boost::shared_ptr<FileProxy> FileProxyPtr;














































//class FileManager: private boost::noncopyable
//{
//public:
//	void Write(const std::string& filename, const char* rawdata, size_t length);
//	void Write(const std::string& filename
//		, const char* trimlog, size_t trim_length
//		, const char* log, size_t length);
//	void Remove(const std::string& filename);
//	~FileManager();
//
//	static FileManager* instance();
//	static void release_instance();
//private:
//	FileProxyPtr get_file(const std::string& filename);
//private:
//	std::map<std::string, FileProxyPtr> _files;
//	static boost::mutex _mutex;
//	static FileManager* _instance;
//};

//void 
//	FileManager::Write(const std::string& filename, const char* rawdata, size_t length)
//{
//	boost::mutex::scoped_lock lock(_mutex);
//	FileProxyPtr file = get_file(filename);
//	if(NULL != file)
//	{
//		file->write(rawdata, length);
//		file->write("\n", 1);//TODO remove this line if we use database
//		//file->flush();
//	}
//}
//
//void 
//	FileManager::Write(const std::string& filename
//	, const char* trimlog, size_t trim_length
//	, const char* log, size_t length)
//{
//	boost::mutex::scoped_lock lock(_mutex);
//	FileProxyPtr file = get_file(filename);
//	if(NULL != file)
//	{
//		file->write(trimlog, trim_length);
//		file->write(log, length);
//		file->flush();
//	}
//}
//
//FileProxyPtr 
//	FileManager::get_file(const std::string& filename)
//{
//	std::map<string, FileProxyPtr>::iterator ret = _files.find(filename);
//	if(ret != _files.end())
//	{
//		return ret->second;
//	}
//
//	FileProxyPtr new_file = FileProxyPtr(new FileProxy(filename, *this));
//	if(new_file->initialize())
//	{
//		_files.insert(pair<std::string, FileProxyPtr>(filename, new_file));
//		return new_file;
//	}
//	return FileProxyPtr(); 
//}
//
//void 
//	FileManager::Remove(const std::string& filename)
//{
//	_files.erase(filename);
//	std::cout<<"FileManager file erase " << filename << std::endl;
//}
//
//
//FileManager::~FileManager()
//{
//	_files.clear();
//}
//
//FileManager* FileManager::_instance = NULL;
//boost::mutex FileManager::_mutex;
//
//FileManager* FileManager::instance()
//{
//	FileManager* tmp = _instance;
//	_ReadWriteBarrier();
//	if(NULL == tmp)
//	{
//		boost::mutex::scoped_lock lock(_mutex);
//		tmp = _instance;
//		if(NULL == tmp)
//		{
//			tmp = new FileManager();
//			_ReadWriteBarrier();
//			_instance = tmp;
//		}
//	}
//	return tmp;
//}
//
//void 
//	FileManager::release_instance()
//{
//	boost::mutex::scoped_lock lock(_mutex);
//	delete _instance;
//	_instance = NULL;
//}


#endif // mcsf_log_file_proxy_h__