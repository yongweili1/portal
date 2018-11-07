#ifndef mcsf_logviewer_interface_h__
#define mcsf_logviewer_interface_h__
#include <string>
#include <boost/shared_array.hpp>
#include <boost/filesystem.hpp>
#include <deque>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "mcsf_logviewer_interface_common.h"

//#define OPEN_LOGVIEWER_INTERFACE

#define INDEXED_LOG_SIZE 37
#define EXPECTED_LOG_NUM_PER_WRITE 600
extern bool open_file(const std::string& file_full_name
	, std::fstream::openmode mode, OUT std::fstream& fs);

namespace Mcsf {namespace LogViewerInterface{

typedef struct MCSF_LOGVIEWER_INTERFACE _level_item_
{
	char* name_;
	unsigned char order_;
}level_item_t;

typedef struct MCSF_LOGVIEWER_INTERFACE _source_item_
{
	char* name_;
	unsigned short order_;
}source_item_t;

typedef struct MCSF_LOGVIEWER_INTERFACE _srcfile_item_
{
	char* name_;
	unsigned short order_;
}srcfile_item_t;

typedef struct MCSF_LOGVIEWER_INTERFACE _function_item_
{
	char* name_;
	unsigned short order_;
}function_item_t;

typedef struct MCSF_LOGVIEWER_INTERFACE _uid_item_
{
	unsigned __int64 name_;
	unsigned int order_;
}uid_item_t;


class MCSF_LOGVIEWER_INTERFACE indexed_log
{
public:
	indexed_log(char* logstr)
	{
		log_ = logstr;
	}

	unsigned char level()
	{
		return log_[0];
	}

	unsigned __int64 timestamp()
	{
		unsigned __int64 ret = 0;
		memcpy(&ret, log_ + 1, 7);
		return ret;
	}

	unsigned short source()
	{
		unsigned short ret = 0;
		memcpy(&ret, log_ + 8, 2);
		return ret;
	}

	unsigned short mcnm()
	{
		unsigned short ret = 0;
		memcpy(&ret, log_ + 10, 2);
		return ret;
	}

	unsigned short pid()
	{
		unsigned short ret = 0;
		memcpy(&ret, log_ + 12, 2);
		return ret;
	}

	unsigned short pid_o()
	{
		unsigned short ret = 0;
		memcpy(&ret, log_ + 10, 2);
		return ret;
	}

	unsigned short tid()
	{
		unsigned short ret = 0;
		memcpy(&ret, log_ + 14, 2);
		return ret;
	}

	unsigned short tid_o()
	{
		unsigned short ret = 0;
		memcpy(&ret, log_ + 12, 2);
		return ret;
	}

	//unsigned int log_file()
	//{
	//	unsigned int ret = 0;
	//	memcpy(&ret, log_ + 14, 4);
	//	return ret;
	//}

	unsigned int offset()
	{
		unsigned int ret = 0;
		memcpy(&ret, log_ + 16, 4);
		return ret;
	}

	unsigned int offset_o()
	{
		unsigned int ret = 0;
		memcpy(&ret, log_ + 14, 4);
		return ret;
	}

	unsigned int src_file()
	{
		unsigned int ret = 0;
		memcpy(&ret, log_ + 20, 3);
		return ret;
	}

	unsigned int src_file_o()
	{
		unsigned int ret = 0;
		memcpy(&ret, log_ + 18, 3);
		return ret;
	}

	unsigned short line()
	{
		unsigned short ret = 0;
		memcpy(&ret, log_ + 23, 2);
		return ret;
	}

	unsigned short line_o()
	{
		unsigned short ret = 0;
		memcpy(&ret, log_ + 21, 2);
		return ret;
	}

	unsigned int func()
	{
		unsigned int ret = 0;
		memcpy(&ret, log_ + 25, 4);
		return ret ;
	}

	unsigned int func_o()
	{
		unsigned int ret = 0;
		memcpy(&ret, log_ + 23, 4);
		return ret ;
	}

	unsigned __int64 uid()
	{
		unsigned __int64 ret = 0;
		memcpy(&ret , log_ + 29, 8);
		return ret;
	}

	unsigned __int64 uid_o()
	{
		unsigned __int64 ret = 0;
		memcpy(&ret , log_ + 27, 8);
		return ret;
	}

private:
	char* log_;
	//|level|timestamp|source|pid|tid|offset|sourcefile|line|func|uid|
	//|1	|7		  |2	 |2  |2  |4     |3         |2   |4   |8  |
	//|0	|1		  |8	 |10 |12 |14    |18        |21  |23  |27 |35
};

//typedef struct MCSF_LOGVIEWER_INTERFACE _raw_log_
//{
//	char* level_;
//	unsigned __int64 timestamp_;
//	char* source_;
//	unsigned int pid_;
//	unsigned int tid_;
//	char* desc_;
//	char* file_;
//	char* line_;
//	char* function_;
//	unsigned __int64 uid_;
//};

typedef boost::shared_array<char> char_array_t;

enum parsing
{
	level_state,
	timestamp_state,
	source_state,
	mcnm_state,
	pid_state,
	tid_state,
	sourcefile_state,
	line_state,
	func_state,
	uid_state,
	desc_state,
	error_handling_state
};

class MCSF_LOGVIEWER_INTERFACE bin_data_t
{
public:
	bin_data_t()
		: data_(NULL), size_(0)
	{
	}

	bin_data_t(char* data, std::size_t size)
		: data_(data), size_(size)
	{
	}

	bool is_valid() const
	{
		return ((size_ > 0) && (NULL != data_));
	}

	//bool operator ==(const _bin_data_& rhs) const
	//{
	//	if(this->size_ != rhs.size_)
	//		return false;
	//	if(0 == memcmp(this->data_, rhs.data_, this->size_))
	//		return true;
	//	return false;
	//}

	bool operator < (const bin_data_t& rhs) const
	{
		std::size_t n = this->size_ < rhs.size_ ? this->size_ : rhs.size_;

		//when two string are partly alike, the compare result should be judged by string length
		//DIM 471584
		int str_cmp = memcmp(this->data_, rhs.data_, n);
		if (str_cmp < 0)
			return true;
		if (str_cmp > 0)
			return false;
		if (this->size_ < rhs.size_)
			return true;
		return false;
	}

	//bool operator =(const _bin_data_& rhs) 
	//{
	//	this->data_ = rhs.data_;
	//	this->size_ = rhs.size_;
	//}

	static bin_data_t clone(const bin_data_t& src)
	{
        bin_data_t ret ;
	    try
        {	
		    ret.size_ = src.size_;
		    if(ret.size_ > 0)
		    {
			    ret.data_ = new char[ret.size_];
			    memcpy(ret.data_, src.data_, ret.size_);
		    }
		    return ret;
        }
        catch(...)
        {
            std::cout << "unknown exception in func:" << __FUNCTION__ << "!" << std::endl; 
            return ret;
	    }
    }
	char* data_;
	std::size_t size_;
	int order_;
};

typedef std::map<bin_data_t, int> feild_index_t;

class MCSF_LOGVIEWER_INTERFACE log_str_to_int_map
{
public:
	void init(const std::string& file);
	
	int get_item_index(const bin_data_t& item)
	{
		if (item.is_valid())
		{
			feild_index_t::iterator it = map_.find(item);
			if(it != map_.end())
				return (*it).second;
				//return static_cast<int>(std::distance(map_.begin(), it));
		}
		return -1;
	}

	bin_data_t get_item(int index)
	{
		try
		{
			if (map_.size() > index)
			{
				//feild_index_t::iterator iter = map_.begin();
				//std::advance(iter, index);
				//return iter->first;
				return list_.at(index);
			}
			
			return bin_data_t();
			//return list_.at(index);
			//return bin_data_t::clone(list_.at(index));
		}
		catch (...)
		{
			return bin_data_t();	
		}
	}

	int new_item(const bin_data_t& item);
	void release()
	{
		//std::map<bin_data_t, int>::iterator it = map_.begin();
		//while(it != map_.end())
		//{
		//	delete (*it).first.data_;
		//	(*it).first.data_ = NULL;
		//	++it;
		//}

		std::vector<bin_data_t>::iterator lit = list_.begin();
		while(lit != list_.end())
		{
			if (lit->data_)
			{
				delete[] lit->data_;
				lit->data_ = NULL;
			}
			++lit;
		}
		map_.clear();
		list_.clear();
	}

	~log_str_to_int_map()
	{
		release();
	}
private:
	void sort(){}

private:
	feild_index_t map_;
	std::vector<bin_data_t> list_;
	std::string file_;
};


class MCSF_LOGVIEWER_INTERFACE bin_data_t_sort
{
public:
	bin_data_t_sort()
		: data_(NULL), size_(0)
	{
	}

	bin_data_t_sort(char* data, std::size_t size)
		: data_(data), size_(size)
	{
	}

	bool is_valid() const
	{
		return ((size_ > 0) && (NULL != data_));
	}

	//bool operator ==(const _bin_data_& rhs) const
	//{
	//	if(this->size_ != rhs.size_)
	//		return false;
	//	if(0 == memcmp(this->data_, rhs.data_, this->size_))
	//		return true;
	//	return false;
	//}

	bool operator < (const bin_data_t_sort& rhs) const
	{
		std::size_t n = this->size_ < rhs.size_ ? this->size_ : rhs.size_;

		//when two string are partly alike, the compare result should be judged by string length
		//DIM 471584
		int str_cmp = memcmp(this->data_, rhs.data_, n);
		if (str_cmp < 0)
			return true;
		if (str_cmp > 0)
			return false;
		if (this->size_ < rhs.size_)
			return true;
		return false;
	}

	//bool operator =(const _bin_data_& rhs) 
	//{
	//	this->data_ = rhs.data_;
	//	this->size_ = rhs.size_;
	//}

	static bin_data_t_sort* clonePtr(const bin_data_t_sort& src)
	{
		bin_data_t_sort *ret = new bin_data_t_sort();
		ret->size_ = src.size_;
		if(ret->size_ > 0)
		{
			ret->data_ = new char[ret->size_];
			memcpy(ret->data_, src.data_, ret->size_);
		}
		return ret;
	}

	static bin_data_t_sort clone(const bin_data_t_sort& src)
	{
		bin_data_t_sort ret;
		ret.size_ = src.size_;
		if(ret.size_ > 0)
		{
			ret.data_ = new char[ret.size_];
			memcpy(ret.data_, src.data_, ret.size_);
		}
		return ret;
	}

	char* data_;
	std::size_t size_;
	int order_;
};

struct compare 
{
	bool operator()(const bin_data_t_sort* lhs, const bin_data_t_sort* rhs)
	{
		std::size_t n = lhs->size_ < rhs->size_ ? lhs->size_ : rhs->size_;
		if(n != 0 && memcmp(lhs->data_, rhs->data_, n) < 0)
			return true;
		return false;
	}
};
typedef std::map<bin_data_t_sort*, int, compare> feild_index_t_sort;

class MCSF_LOGVIEWER_INTERFACE log_str_to_int_map_sort
{
public:
	void init(const std::string& file);
	
	int get_item_index(bin_data_t_sort* item)
	{
	    if(item->is_valid())
		{
		feild_index_t_sort::iterator it = map_.find(item);
		if(it != map_.end())
			return (*it).second;
		}
		
		return -1;
	}

	bin_data_t_sort get_item(int index)
	{
		try
		{
		if (map_.size() > index)
			{
			return *(list_.at(index));
			}
			return bin_data_t_sort();	
			//return bin_data_t::clone(list_.at(index));
		}
		catch (...)
		{
			return bin_data_t_sort();	
		}
	}

	int get_sorted_index(int index)
	{
		try
		{
			return list_.at(index)->order_;
		}
		catch (...)
		{
			init(dir_);
			return list_.at(index)->order_;
		}
	}

	int new_item(const bin_data_t_sort& item);
	void release()
	{
		//std::map<bin_data_t, int>::iterator it = map_.begin();
		//while(it != map_.end())
		//{
		//	delete (*it).first.data_;
		//	(*it).first.data_ = NULL;
		//	++it;
		//}

		std::vector<bin_data_t_sort*>::iterator lit = list_.begin();
		while(lit != list_.end())
		{
			if ((*lit)->data_)
			{
				delete (*lit)->data_;
				(*lit)->data_ = NULL;
			}
			++lit;
		}
		map_.clear();
		list_.clear();
	}

	~log_str_to_int_map_sort()
	{
		release();
	}
private:
	void sort()
	{
		int k = 0;
		for (auto i = map_.begin(); i != map_.end(); i++, k++)
		{
			i->first->order_ = k;
		}
	}
private:
	feild_index_t_sort map_;
	std::vector<bin_data_t_sort*> list_;
	std::string file_;
	std::string dir_;
};

class MCSF_LOGVIEWER_INTERFACE log_feild_maps_sort
{
public:
	static void init(const std::string& log_dir)
	{
		levels_sort_.init(log_dir + "/levels");
		sources_sort_.init(log_dir + "/sources");
		files_sort_.init(log_dir + "/files");
		funcs_sort_.init(log_dir + "/funcs");
		mcnm_sort_.init(log_dir + "/mcnm");
	}

	static log_str_to_int_map_sort levels_sort_;
	static log_str_to_int_map_sort sources_sort_;
	static log_str_to_int_map_sort files_sort_;
	static log_str_to_int_map_sort funcs_sort_;
	static log_str_to_int_map_sort mcnm_sort_;
	static std::string log_dir_base_sort_;
};
typedef log_feild_maps_sort feilds_sort;

class MCSF_LOGVIEWER_INTERFACE log_feild_maps
{
public:
	static void init(const std::string& log_dir)
	{
		levels_.init(log_dir + "/levels");
		sources_.init(log_dir + "/sources");
		files_.init(log_dir + "/files");
		funcs_.init(log_dir + "/funcs");
		mcnm_.init(log_dir + "/mcnm");
	}

	static log_str_to_int_map levels_;
	static log_str_to_int_map sources_;
	static log_str_to_int_map files_;
	static log_str_to_int_map funcs_;
	static log_str_to_int_map mcnm_;
	static std::string log_dir_base_;
};
typedef log_feild_maps feilds;
class MCSF_LOGVIEWER_INTERFACE log_viewer_interface
{
public:
	static void init();
	static void init_map_sort();
};

class MCSF_LOGVIEWER_INTERFACE log_parser
{
public :
	log_parser(char_array_t blob, std::size_t blobsize)
		: blob_(blob)
		, size_(blobsize)
		, pos_(0)
	{
        try
        {
            caIndexLogSet.reset(new char[EXPECTED_LOG_NUM_PER_WRITE * INDEXED_LOG_SIZE]);
        }
        catch(std::exception& ex)
        {
            std::cout << "exception " << ex.what() << "in func" << __FUNCTION__ << std::endl;
        }
        catch(...)
        {
            std::cout << "exception " << "in func" << __FUNCTION__ << std::endl;
        }
	}

	void to_indexed_logs(std::size_t file_tobe_write_pos
		,/* const std::string& index_to_file,*/ std::fstream& fsIndex);

	/*bool read_until(char* src, char* ret)
	{
		return false;
	}
	bool read_level(char* src, char* next_feild, char* level)
	{
		return false;
	}

	bool read_log()
	{
		return false;
	}*/

    inline void set_value(char_array_t blob, std::size_t blobsize)
    {
        blob_ = blob;
        size_ = blobsize;
        pos_ = 0;
    }

private: 
	char_array_t blob_;
	std::size_t size_;
	std::size_t pos_;
    char_array_t caIndexLogSet;
};

class mem_reader
{
public:
	mem_reader(char_array_t blob, std::size_t blobsize)
		: blob_(blob)
		, size_(blobsize)
		, pos_(0)
	{
	}

	bool read(char& ret)
	{
		if(pos_ < size_)
		{
			ret = blob_[pos_++];
			return true;
		}
		return false;
	}

	bool set_rpos(std::size_t pos)
	{
		if(pos < size_)
		{
			pos_ = pos;
			return true;
		}
		return false;
	}

	void reset(char_array_t blob, std::size_t blobsize)
	{
		blob_ = blob;
		size_ = blobsize;
		pos_ = 0;
	}

private:
	char_array_t blob_;
	std::size_t size_;
	std::size_t pos_;
};
}}
#endif // mcsf_logviewer_interface_h__