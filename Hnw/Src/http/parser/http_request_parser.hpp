/*
    http 请求解析器
    hql 202 04/14
*/
#ifndef HNW_HTTP_REQUEST_PARSER_HPP_
#define HNW_HTTP_REQUEST_PARSER_HPP_
#include "../hnw_http.h"
#include "../struct/http_request_impl.hpp"
namespace hnw
{
    namespace parser
    {
        class HttpRequestParser :public ParserBase
        {
            
        public:
            HttpRequestParser(HNW_HANDLE handle):ParserBase(handle)
            {
                REG_CFUNC(util::START_LINE, HttpRequestParser::on_start_line);
                REG_CFUNC(util::HEAD, HttpRequestParser::on_head);
                REG_CFUNC(util::BODY, HttpRequestParser::on_body);

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
				if (util::CR == ch)
				{
					return HNW_BASE_ERR_CODE::HNW_BASE_OK;
				}
				else if (util::LF == ch)
				{
					//空格分割
					auto d = util::split(parser_cache1_, util::SPACE);

					if (d.size() < 3)
					{
						PRINTFLOG(BL_ERROR, "bad start line:%s", parser_cache1_.c_str());
						return HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_BAD_START_LINE;
					}
					if (tmp_)
					{
						tmp_->line->method(d[0]);
						tmp_->line->url(d[1]);
						tmp_->line->version(d[2]);
					}
					parser_cache1_ = "";
					parser_status_ = util::HEAD;
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

				//PRINTFLOG(BL_DEBUG, "%c", ch);
				if (util::CR == ch)
				{
					return HNW_BASE_ERR_CODE::HNW_BASE_OK;
				}
				else if (util::LF == ch)
				{

					if ("" == parser_cache1_)
					{
						//转到 body
						EVENT_CB(HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_REQUEST_HEAD, tmp_);

						if (nullptr == tmp_->body
							|| HTTP_BODY_TYPE::BODY_EMPTY ==tmp_->body->body_type())
						{
							if (!tmp_->auto_create_body())
							{
								PRINTFLOG(BL_ERROR, "cannt create body");
								EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_HTTP_STRUCT_MESSAGE_FAIL, "报文组装失败 body");
								return HNW_BASE_ERR_CODE::HNW_HTTP_STRUCT_MESSAGE_FAIL;
							}
						}

						if (tmp_->body->is_complete())
						{
							return push();
						}

						parser_status_ = util::BODY;
						return HNW_BASE_ERR_CODE::HNW_BASE_OK;

						//size_t p = (size_t)tmp_->head->get_head_int64(util::HTTP_LEN,0);
						//if (p<=0)
						//{
						//	//结束
						//	return push();
						//}
						//else
						//{
						//	//拿到大小
						//	body_len_ = p;
						//	if(body_len_ == 0)
						//		return push();
						//	parser_status_ = util::BODY;
						//}
						//return HNW_BASE_ERR_CODE::HNW_BASE_OK;
					}
					else
					{
						//空格分割
						auto d = util::split(parser_cache1_, util::HEAD_SPLIT);

						if (d.size() < 2)
						{
							PRINTFLOG(BL_ERROR, "bad start line:%s", parser_cache1_.c_str());
							return HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_BAD_HEAD_KEY;
						}
						if (tmp_)
						{
							//tmp_->head[d[0]] = d[1];
							tmp_->head->add_head(d[0], d[1]);
						}
						parser_cache1_ = "";
						parser_status_ = util::HEAD;
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

				auto write_size = tmp_->body->write_body((char*)(*start), data_len);
				*start = *start + write_size;
				//没有写完
				if (tmp_->body->is_complete())
				{
					return push();
				}
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;

				//if ((size_t)data_len >= body_len_)
				//{
				//	//
				//	tmp_->body->write_body((char*)(*start), (size_t)body_len_);
				//	*start = *start+ body_len_;
				//	body_len_ = 0;
				//	return push();
				//}
				//else
				//{
				//	tmp_->body->write_body((char*)(*start), data_len);
				//	*start = end;
				//	body_len_ -= data_len;
				//	return HNW_BASE_ERR_CODE::HNW_BASE_OK;
				//}
				
			}
         
	   public:

		   void reset()
		   {
			   //解析状态
			   parser_status_ = util::PSTATUS::START_LINE;
			   tmp_ = std::make_shared<http::RequestImpl>();
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
            std::shared_ptr<http::RequestImpl> tmp_;
        };

    }
}
#endif
