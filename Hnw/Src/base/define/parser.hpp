/*
    解析器基础类
    负责解析报文
*/
#ifndef HNW_PARSER_HPP_
#define HNW_PARSER_HPP_

#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include <map>

#include "define.hpp"

#include "../hnw_define.h"

//注册类成员函数
#define REG_CFUNC(s,cb) reg_parser_cb(s,std::bind(&cb, this,\
std::placeholders::_1, std::placeholders::_2))

//注册普通函数
#define REG_FUNC(s,cb) reg_parser_cb(s,std::bind(cb,\
 std::placeholders::_1, std::placeholders::_2))

namespace hnw
{
	namespace parser
	{
		//解析回调
		typedef std::function<HNW_BASE_ERR_CODE(unsigned char **start,
			 unsigned char* const end)> PARSER_CB;

		//解析器基类
		class ParserBase 
		{
		public:
			ParserBase(HNW_HANDLE handle):handle_(handle)
			{
				reg_parser_default_cb([this](unsigned char** start,
					 unsigned char* const end) mutable{
					return default_parser(start,end);
				});
			}
			virtual ~ParserBase()
			{

			}
		public:
			/*
				输入的数据
				返回，0无错误 其他自定义
			*/
			virtual HNW_BASE_ERR_CODE input_data(unsigned char* data,
				size_t data_len)
			{
				unsigned char* start = data;
				auto end = data + data_len;
				while(start!=end)
				{
					auto ret = call_parser_cb(&start,end);
					if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
					{
						PRINTFLOG(BL_ERROR, "call_parser_cb fail ret=%d,status=%d",
							(int)ret,parser_status_);
						return ret;
					}
					PRINTFLOG(BL_DEBUG, "%p-%p status %d", start, end, parser_status_);
				}
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}

		public:
			virtual bool reg_parser_cb(int status, PARSER_CB cb)
			{
				parser_cb_map_[status] = cb;
				return true;
			}
			virtual bool reg_parser_default_cb(PARSER_CB cb)
			{
				default_parser_cb_ = cb;
				return true;
			}

			virtual HNW_BASE_ERR_CODE call_parser_cb(unsigned char** start,
				 unsigned char* const  end)
			{
				PARSER_CB cb= default_parser_cb_;
				auto f = parser_cb_map_.find(parser_status_);
				if (parser_cb_map_.end() != f)
					cb = f->second;
				if (cb)
					return cb(start,end);
				return HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_CB_IS_EMPTY;
			}

			virtual HNW_BASE_ERR_CODE default_parser(unsigned char** start,
				 unsigned char* const end)
			{
				PRINTFLOG(BL_ERROR, "default_parser fail status=%d",
					 parser_status_);
				*start = end;
				return HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_CB_IS_DEFAULT;
			}
		public:
			//设置日志回调
			virtual HNW_BASE_ERR_CODE set_log_cb(HNW_LOG_CB cb)
			{
				log_cb_ = cb;
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}

			//设置缓存回调
			virtual HNW_BASE_ERR_CODE set_make_shared_cb(HNW_MAKE_SHARED_CB cb)
			{
				if (cb)
				{
					make_shared_ = cb;
					return HNW_BASE_ERR_CODE::HNW_BASE_OK;
				}
				PRINTFLOG(BL_ERROR, "make_shared_ dont set,make_shared_ is empty!");
				return HNW_BASE_ERR_CODE::HNW_BASE_EMPYTY_MAKE_SHARED_CB;
			}

			//设置事件回调
			virtual HNW_BASE_ERR_CODE set_event_cb(HNW_EVENT_CB cb)
			{
				event_cb_ = cb;
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}

			//获取句柄
			virtual HNW_HANDLE get_handle()
			{
				return handle_;
			}
		protected:
			//回调函数
			HNW_MAKE_SHARED_CB make_shared_;

			HNW_LOG_CB log_cb_;

			HNW_EVENT_CB event_cb_;

			HNW_HANDLE handle_;
		protected:
			int parser_status_;

			PARSER_CB default_parser_cb_;

			std::map<int, PARSER_CB> parser_cb_map_;
		};

		
	}
}
#endif // !HNW_PARSER_HPP_
