/*
    http 解析的共有定义
    hql 2020 04 15
*/
#ifndef HNW_HTTP_PARSER_DEFINE_HPP_
#define HNW_HTTP_PARSER_DEFINE_HPP_
#include "../../../define/define.hpp"
namespace hnw
{
	namespace parser
	{
		enum PSTATUS
		{
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
		};

		const unsigned char CR('\r');
		const unsigned char LF('\n');
		const unsigned char SPACE(' ');
		
		//冒号  colon
		const unsigned char COLON(':');

		const std::string CRLF("\r\n");
		const std::string CRLFCRLF("\r\n\r\n");
		const std::string HEAD_SPLIT(": ");

		const std::string JSON_MIME("application/json; charset=UTF-8");
		const std::string XML_MIME("application/rss+xml");
		const std::string HTML_MIME("text/html");
		const std::string HTTP_CT("Content-Type");
		const std::string HTTP_LEN("Content-Length");
		const std::string HTTP_CONN("Connection");
		const std::string HTTP_RANGE("Content-Range");
		const std::string HTTP_TRANSFER_ENCODING("Transfer-Encoding");

		//分块传输
		const std::string HTTP_CHUNKED("chunked");
		
		
	}
}
#endif // !HNW_HTTP_PARSER_DEFINE_HPP_
