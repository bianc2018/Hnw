/*
    http 请求解析器
    hql 202 04/14
*/
#ifndef HNW_HTTP_REQUEST_PARSER_HPP_
#define HNW_HTTP_REQUEST_PARSER_HPP_
#include "../hnw_http.h"
#include "http_parser_define.hpp"
namespace hnw
{
    namespace parser
    {
        class HttpRequestParser :public ParserBase
        {
            
        public:
            HttpRequestParser(HNW_HANDLE handle):ParserBase(handle)
            {
                REG_CFUNC(START_LINE, HttpRequestParser::on_start_line);
                REG_CFUNC(HEAD, HttpRequestParser::on_head);
                REG_CFUNC(BODY, HttpRequestParser::on_body);

                reset();

            }
            virtual ~HttpRequestParser()
            {

            }

		private:
			HNW_BASE_ERR_CODE on_start_line( unsigned char** start,
				 unsigned char* const end)
			{
				auto ch = *(*start);
				(*start)++;
				PRINTFLOG(BL_DEBUG, "%c", ch);
				if (CR == ch)
				{
					return HNW_BASE_ERR_CODE::HNW_BASE_OK;
				}
				else if (LF == ch)
				{
					//空格分割
					auto d = split(parser_cache1_, SPACE);

					if (d.size() < 3)
					{
						PRINTFLOG(BL_ERROR, "bad start line:%s", parser_cache1_.c_str());
						return HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_BAD_START_LINE;
					}
					if (tmp_)
					{
						tmp_->method = d[0];
						tmp_->url = d[1];
						tmp_->version = d[2];
					}
					parser_cache1_ = "";
					parser_status_ = HEAD;
				}
				else
				{
					parser_cache1_ += ch;
				}
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}
			HNW_BASE_ERR_CODE on_head( unsigned char** start,
				 unsigned char* const end)
			{
				auto ch = *(*start);
				(*start)++;

				PRINTFLOG(BL_DEBUG, "%c", ch);
				if (CR == ch)
				{
					return HNW_BASE_ERR_CODE::HNW_BASE_OK;
				}
				else if (LF == ch)
				{
					if ("" == parser_cache1_)
					{
						//转到 body
						auto p = tmp_->head.find(HTTP_LEN);
						if (tmp_->head.end() == p)
						{
							//结束
							return push();
						}
						else
						{
							//拿到大小
							body_len_ = std::stoul(p->second);
							if(body_len_ == 0)
								return push();
							parser_status_ = BODY;
						}
						//return HNW_BASE_ERR_CODE::HNW_BASE_OK;
					}
					else
					{
						//空格分割
						auto d = split(parser_cache1_, HEAD_SPLIT);

						if (d.size() < 2)
						{
							PRINTFLOG(BL_ERROR, "bad start line:%s", parser_cache1_.c_str());
							return HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_BAD_HEAD_KEY;
						}
						if (tmp_)
						{
							//tmp_->head[d[0]] = d[1];
							tmp_->head.insert(std::make_pair(d[0], d[1]));
						}
						parser_cache1_ = "";
						parser_status_ = HEAD;
					}
				}
				else
				{
					parser_cache1_ += ch;
				}
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}
			HNW_BASE_ERR_CODE on_body( unsigned char** start,
				 unsigned char* const end)
			{
				auto data_len = end - (*start);
				if (data_len < 0)
				{
					PRINTFLOG(BL_ERROR, "end>start");
					return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
				}
				if (data_len >= body_len_)
				{
					//
					tmp_->body.append((char*)(*start), body_len_);
					*start = *start+ body_len_;
					body_len_ = 0;
					return push();
				}
				else
				{
					tmp_->body.append((char*)(*start), data_len);
					*start = end;
					body_len_ -= data_len;
					return HNW_BASE_ERR_CODE::HNW_BASE_OK;
				}
				
			}
         
	   public:
		   //组装原始数据包
		   static std::string struct_raw_package(std::shared_ptr<HnwHttpRequest> msg,
			   HNW_BASE_ERR_CODE& ret)
		   {
			   ret = HNW_BASE_ERR_CODE::HNW_BASE_OK;

			   if (msg)
			   {
				   //新增body_len
				   auto p = msg->head.find(HTTP_LEN);
				   if (msg->head.end() == p)
				   {
					   msg->head.insert(std::make_pair(HTTP_LEN, std::to_string(msg->body.size())));
				   }

				   //新增body_len
				   p = msg->head.find(HTTP_CONN);
				   if (msg->head.end() == p)
				   {
					   msg->head.insert(std::make_pair(HTTP_CONN, "Close"));
				   }

				   std::string str = msg->method + SPACE + msg->url + SPACE + msg->version + CRLF;

				   for (auto h : msg->head)
				   {
					   str += h.first + HEAD_SPLIT + h.second + CRLF;
				   }

				   str += CRLF;
				   str += msg->body;

				//   printf("http response:\n%s\n", str.c_str());

				   return str;
			   }
			   ret = HNW_BASE_ERR_CODE::HNW_HTTP_BAD_MESSAGE;
			   return "";
		   }

		   void reset()
		   {
			   //解析状态
			   parser_status_ = PSTATUS::START_LINE;
			   body_len_ = 0;
			   tmp_ = std::make_shared<HnwHttpRequest>();
			   //解析缓存
			   parser_cache1_.clear();
			   //parser_cache2_.clear();
		   }

		   HNW_BASE_ERR_CODE push()
		   {
			   if (tmp_)
			   {
				   EVENT_CB(HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_REQUEST, tmp_);
			   }
			   reset();
			   return HNW_BASE_ERR_CODE::HNW_BASE_OK;
		   }
		private:
			//解析缓存
			std::string parser_cache1_;
			size_t body_len_;
            std::shared_ptr<HnwHttpRequest> tmp_;
        };

    }
}
#endif
