/*
    http 请求解析器
	hql 202 04/14
*/
#ifndef HNW_HTTP_RESPONSE_PARSER_HPP_
#define HNW_HTTP_RESPONSE_PARSER_HPP_
#include "../../hnw_http.h"
#include "../../define/parser.hpp"
#include "http_parser_define.hpp"
namespace hnw
{
	namespace parser
	{
		class HttpRequestParser :public ParserBase
		{
			
		public:
			HttpRequestParser()
			{
				clear();

				REG_CFUNC(START_LINE1, HttpRequestParser::on_start_line1);
				REG_CFUNC(START_LINE2, HttpRequestParser::on_start_line1);
				REG_CFUNC(START_LINE3, HttpRequestParser::on_start_line1);
				REG_CFUNC(HEAD_KEY, HttpRequestParser::on_head_value);
				REG_CFUNC(HEAD_VALUE, HttpRequestParser::on_head_key);
				REG_CFUNC(BODY, HttpRequestParser::on_body);
				REG_CFUNC(CHUNKED_LEN, HttpRequestParser::on_chunked_len);
				REG_CFUNC(CHUNKED_DATA, HttpRequestParser::on_chunked_data);
				REG_CFUNC(OVER, HttpRequestParser::on_over);
				REG_CFUNC(PERROR, HttpRequestParser::on_error);

			}
			virtual ~HttpRequestParser()
			{

			}

		private:
			/*
			START_LINE1 = 1,
			START_LINE2 = 2,
			START_LINE3 = 3,
			HEAD_KEY = 4,
			HEAD_VALUE = 5,
			BODY = 6,
			CHUNKED_LEN = 7,
			CHUNKED_DATA = 8,
			OVER = 9,
			PERROR = -1,
			*/

			//START_LINE1
			HNW_BASE_ERR_CODE on_start_line1(const unsigned char ch)
			{
				if (SPACE == ch)
				{
					set_start_line2(parser_cache1_);
					parser_cache1_ = "";
					parser_status_ = START_LINE2;
				}
				else
				{
					parser_cache1_ += ch;
				}
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}

			//START_LINE2
			HNW_BASE_ERR_CODE on_start_line2(const unsigned char ch)
			{
				if (SPACE == ch)
				{
					set_start_line2(parser_cache1_);
					parser_cache1_ = "";
					parser_status_ = START_LINE3;
				}
				else
				{
					parser_cache1_ += ch;
				}
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}

			//START_LINE3
			HNW_BASE_ERR_CODE on_start_line3(const unsigned char ch)
			{
				if (CR == ch)
				{
					return;
				}
				else if (LF == ch)
				{
					set_start_line3(parser_cache1_);
					parser_cache1_ = "";
					parser_status_ = HEAD_KEY;
				}
				else
				{
					parser_cache1_ += ch;
				}
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}

			//HEAD_KEY
			HNW_BASE_ERR_CODE on_head_key(const unsigned char ch)
			{
				if (CR == ch)
				{
					continue;
				}
				else if (LF == ch)
				{
					if (parser_cache1_ != "")
					{
						parser_status_ = PERROR;
						PRINTFLOG(BL_ERROR, "parser error ch=0x%4X", ch);
						return HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_KEY_IS_NO_EMPTY;
					}

					auto len = get_head_value(HTTP_LEN, "");
					if ("0" == len)//body =0
					{
						parser_status_ = OVER;
					}
					else if ("" == len) //body 没有此字段
					{
						//分块传输
						if (HTTP_CHUNKED == get_head_value(HTTP_TRANSFER_ENCODING, ""))
						{
							parser_status_ = CHUNKED_LEN;
						}
						else
						{
							parser_status_ = OVER;
						}
					}
					else//head 有此字段
					{
						parser_status_ = BODY;
					}

				}
				else if (COLON == ch)
				{

					parser_status_ = HEAD_VALUE;
				}
				else
				{
					parser_cache1_ += ch;
				}
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}

			//HEAD_VALUE
			HNW_BASE_ERR_CODE on_head_value(const unsigned char ch)
			{
				//过滤\r
				if (CR == ch)
				{
					continue;
				}

				if (SPACE == ch)
				{
					//键值为空 冒号之后的空格 忽略
					if ("" == parser_cache2_)
						continue;
					else//否则 作为键值
						parser_cache2_ += ch_p;
				}
				else if (LF == ch)//换行结束
				{
					//键值为空
					if ("" == parser_cache1_)
					{
						parser_status_ = PERROR;
						PRINTFLOG(BL_ERROR, "parser error ch=0x%4X", ch);
						return HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_KEY_IS_EMPTY;
					}

					//缓存长度
					if ((HTTP_LEN == parser_cache1_) && ("" != parser_cache2_))
					{
						try
						{
							content_len_cache_ = std::stoll(parser_cache2_);

						}
						catch (std::exception& e)
						{
							parser_status_ = PARSE_ERROR;
							return HNW_BASE_ERR_CODE::HNW_BASE_OK;
						}
					}

					PRINTFLOG(BL_DEBUG, "parser head_key %s:%s"
						, parser_cache1_.c_str()
						, parser_cache2_.c_str());
					
					add_head_item(parser_cache1_, parser_cache2_);

					parser_cache1_ = "";
					parser_cache2_ = "";
					parser_status_ = HEAD_KEY;
				}
				else
				{
					parser_cache2_ += ch;
				}
			}

			//BODY
			HNW_BASE_ERR_CODE on_body(const unsigned char ch)
			{
			}

			//CHUNKED_LEN
			HNW_BASE_ERR_CODE on_chunked_len(const unsigned char ch);

			//CHUNKED_DATA
			HNW_BASE_ERR_CODE on_chunked_data(const unsigned char ch);

			//OVER
			HNW_BASE_ERR_CODE on_over(const unsigned char ch);

			//error
			HNW_BASE_ERR_CODE on_error(const unsigned char ch);

		private:
			void add_head_item(const std::string& k, const std::string& v);

			std::string get_head_value(const std::string & k, const std::string & n);

			void set_start_line1(const std::string& v);

			void set_start_line2(const std::string& v);

			void set_start_line3(const std::string& v);

			//追加数据
			void apped_body(unsigned char v);

			void clear()
			{
				//解析状态
				parser_status_ = PSTATUS::START_LINE1;

				content_len_cache_ = 0;
				//解析缓存
				parser_cache1_.clear();
				parser_cache2_.clear();
			}
		private:
			std::uint64_t content_len_cache_;
			
			//解析缓存
			std::string parser_cache1_;
			std::string parser_cache2_;

			std::shared_ptr<HnwHttpRequest> tmp_;
		};

	}
}
#endif
