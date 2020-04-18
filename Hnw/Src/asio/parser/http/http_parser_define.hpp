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
			START_LINE,
			START_LINE1,
			START_LINE2,
			START_LINE3,
			HEAD,
			HEAD_KEY,
			HEAD_VALUE,
			BODY,
			CHUNKED_LEN,
			CHUNKED_DATA,
			OVER,
			PERROR = -1,
		};

		const unsigned char CR('\r');
		const unsigned char LF('\n');
		const std::string SPACE(" ");
		
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
		
		
		//解析函数
		static std::vector<std::string> split(const std::string& src, const std::string& splitor)
		{
			std::vector<std::string> res;
			auto beg = 0;
			auto pos = std::string::npos;
			do
			{
				pos = src.find(splitor,beg);

				if (std::string::npos == pos)
				{
					res.push_back(src.substr(beg));
					break;
				}
				else
				{
					//auto t = src.substr(beg, pos - beg);
					//截取
					res.push_back(src.substr(beg,pos-beg));

					//步进
					beg = pos + splitor.size();
				}

			} while (true);

			return res;
		}

		//获取对应的MIME
		static std::string http_mime(const std::string& ext, const std::string& defualt_d = HTML_MIME)
		{
			return defualt_d;
		}

		//获取状态说明
		static std::string http_reasion(const std::string& status, const std::string& defualt_r = "NONE")
		{
			return defualt_r;
		}
	}
}
#endif // !HNW_HTTP_PARSER_DEFINE_HPP_
