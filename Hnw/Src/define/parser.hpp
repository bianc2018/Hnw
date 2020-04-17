/*
    ������������
    �����������
*/
#ifndef HNW_PARSER_HPP_
#define HNW_PARSER_HPP_

#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include "../hnw_define.h"

//ע�����Ա����
#define REG_CFUNC(s,cb) reg_parser_cb(s,std::bind(&cb, this, std::placeholders::_1))

//ע����ͨ����
#define REG_FUNC(s,cb) reg_parser_cb(s,std::bind(cb, std::placeholders::_1))

namespace hnw
{
	namespace parser
	{
		//�����ص�
		typedef std::function<HNW_BASE_ERR_CODE(const  unsigned char ch)> PARSER_CB;

		//����������
		class ParserBase 
		{
		public:
			ParserBase()
			{
				reg_parser_default_cb([this](const unsigned char ch) {
					return default_parser(ch);
				});
			}
			virtual ~ParserBase()
			{

			}
		public:
			/*
				���������
				���أ�0�޴��� �����Զ���
			*/
			virtual HNW_BASE_ERR_CODE input_data(const unsigned char* data,
				size_t data_len)
			{
				for (size_t i = 0; i < data_len; ++i)
				{
					auto ch_p = data[i];
					auto ret = call_parser_cb(ch_p);
					if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
					{
						PRINTFLOG(BL_ERROR, "call_parser_cb fail ret=%d,status=%d",
							(int)ret,parser_status_);
						return ret;
					}
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

			virtual HNW_BASE_ERR_CODE call_parser_cb(const unsigned char ch)
			{
				PARSER_CB cb= default_parser_cb_;
				auto f = parser_cb_map_.find(parser_status_);
				if (parser_cb_map_.end() != f)
					cb = f->second;
				if (cb)
					return cb(ch);
				return HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_CB_IS_EMPTY;
			}

			virtual HNW_BASE_ERR_CODE default_parser(const const unsigned char ch)
			{
				PRINTFLOG(BL_ERROR, "default_parser fail status=%d",
					 parser_status_);
				return HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_CB_IS_DEFAULT;
			}
		public:
			//������־�ص�
			virtual HNW_BASE_ERR_CODE set_log_cb(HNW_LOG_CB cb)
			{
				log_cb_ = cb;
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}

			//���û���ص�
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

			//�����¼��ص�
			virtual HNW_BASE_ERR_CODE set_event_cb(HNW_EVENT_CB cb)
			{
				event_cb_ = cb;
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}
		protected:
			//�ص�����
			HNW_MAKE_SHARED_CB make_shared_;

			HNW_LOG_CB log_cb_;

			HNW_EVENT_CB event_cb_;
		protected:
			int parser_status_;

			PARSER_CB default_parser_cb_;

			std::map<int, PARSER_CB> parser_cb_map_;
		};

		
	}
}
#endif // !HNW_PARSER_HPP_
