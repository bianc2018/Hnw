/*
    http 回复解析器
	hql 202 04/14
*/
#ifndef HNW_HTTP_RESPONSE_PARSER_HPP_
#define HNW_HTTP_RESPONSE_PARSER_HPP_
#include "../hnw_http.h"
#include "../struct/http_response_impl.hpp"
namespace hnw
{
	namespace parser
	{
		class HttpResponseParser:public ParserBase
		{
		public:
			HttpResponseParser(HNW_HANDLE handle):ParserBase(handle)
			{
				REG_CFUNC(util::START_LINE, HttpResponseParser::on_start_line);
				REG_CFUNC(util::HEAD, HttpResponseParser::on_head);
				REG_CFUNC(util::BODY, HttpResponseParser::on_body);

				reset();
			}
			virtual ~HttpResponseParser()
			{

			}

		private:
			HNW_BASE_ERR_CODE on_start_line( unsigned char** start,
				 unsigned char* const end)
			{
				auto ch = *(*start);
				(*start)++;

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
						tmp_->line->version(d[0]);
						tmp_->line->status_code(d[1]);
						tmp_->line->reason(d[2]);
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
				if (util::CR == ch)
				{
					return HNW_BASE_ERR_CODE::HNW_BASE_OK;
				}
				else if (util::LF == ch)
				{
					if ("" == parser_cache1_)
					{
						//转到 body

						//转到 body
						EVENT_CB(HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_RESPONSE_HEAD, tmp_);

						if (nullptr == tmp_->body||BODY_EMPTY == tmp_->body->body_type())
						{
							if (!tmp_->auto_create_body())
							{
								PRINTFLOG(BL_ERROR, "cannt create body");
								EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_HTTP_STRUCT_MESSAGE_FAIL, "报文组装失败 body");
								return HNW_BASE_ERR_CODE::HNW_HTTP_STRUCT_MESSAGE_FAIL;
							}
						}
						if (nullptr == tmp_->body)
						{
							PRINTFLOG(BL_ERROR, "body is empty");
							EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_HTTP_STRUCT_MESSAGE_FAIL, "报文组装失败 body");
							return HNW_BASE_ERR_CODE::HNW_HTTP_STRUCT_MESSAGE_FAIL;
						}
						if (tmp_->body->is_complete())
						{
							return push();
						}

						parser_status_ = util::BODY;
						return HNW_BASE_ERR_CODE::HNW_BASE_OK;
						//std::uint64_t p = (std::uint64_t)tmp_->head->get_head_int64(util::HTTP_LEN, 0);
						//if (p == 0)
						//{
						//	//结束
						//	return push();
						//}
						//else
						//{
						//	//拿到大小
						//	body_len_ = p;
						//	if (body_len_ == 0)
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
							PRINTFLOG(BL_ERROR, "bad head key:%s", parser_cache1_.c_str());
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

				//if ((std::uint64_t)data_len >= body_len_)
				//{
				//	//
				//	tmp_->body->write_body((char*)(*start), (size_t)body_len_);
				//	*start = *start + body_len_;
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
				//body_len_ = 0;
				tmp_ = http::ResponseImpl::generate();
				//解析缓存
				parser_cache1_.clear();
				//parser_cache2_.clear();
			}

			HNW_BASE_ERR_CODE push()
			{
				if (tmp_)
				{
					EVENT_CB(HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_RESPONSE,tmp_);
				}
				reset();
				return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			}
		private:
			//解析缓存
			std::string parser_cache1_;
			//std::string parser_cache2_;

			std::uint64_t body_len_;
			SPHnwHttpResponse tmp_;
		};

	}
}
#endif