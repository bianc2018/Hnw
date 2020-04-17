/*
    http 回复解析器
	hql 202 04/14
*/
#ifndef HNW_HTTP_REQUEST_PARSER_HPP_
#define HNW_HTTP_REQUEST_PARSER_HPP_
#include "../../../hnw_http.h"
#include "../../../define/parser.hpp"
namespace hnw
{
	namespace parser
	{
		class HttpResponseParser:public ParserBase
		{
		public:
			HttpResponseParser()
			{

			}
			virtual ~HttpResponseParser()
			{

			}

		public:
			//组装原始数据包
			static std::string struct_raw_package(std::shared_ptr<HttpResponseParser> msg,
				HNW_BASE_ERR_CODE& ret)
			{
				return "";
			}
		};

	}
}
#endif