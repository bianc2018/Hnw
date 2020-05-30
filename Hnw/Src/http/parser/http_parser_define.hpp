/*
    http 解析的共有定义
    hql 2020 04 15
*/
#ifndef HNW_HTTP_PARSER_DEFINE_HPP_
#define HNW_HTTP_PARSER_DEFINE_HPP_
#include "define/parser.hpp"
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

		//获取对应的MIME .mp4
		static std::string http_mime(const std::string& ext, const std::string& defualt_d = HTML_MIME)
		{
			static std::map<std::string, std::string> m = \
			{
				{ ".aac", "audio/aac" },
				{ ".abw","application/x-abiword" },
				{ ".avi","video/x-msvideo" },
				{ ".bmp","image/bmp" },
				{ ".bz","application/x-bzip" },
				{ ".bz2","application/x-bzip2" },
				{ ".css","text/css" },
				{ ".csv","text/csv" },
				{ ".doc","application/msword" },
				{ ".docx","application/vnd.openxmlformats-officedocument.wordprocessingml.document" },
				{ ".epub","application/epub+zip" },
				{ ".gif","image/gif" },
				{ ".htm","text/html" },
				{ ".html","text/html" },
				{ ".ico","image/vnd.microsoft.icon" },
				{ ".jar","application/java-archive" },
				{ ".jpeg","image/jpeg" },
				{ ".jpg","image/jpeg" },
				{ ".js","text/javascript" },
				{ ".json","application/json" },
				{ ".mp3","audio/mpeg" },
				{ ".mpeg","video/mpeg" },
				{ ".png","image/png" },
				{ ".rar","application/x-rar-compressed" },
				{ ".txt","text/plain" },
				{ ".ttf","font/ttf" },
				{ ".wav","audio/wav" },
				{ ".xls","application/vnd.ms-excel" },
				{ ".xlsx","application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" },
				{ ".xml","application/xml" },
				{ ".zip","application/zip" },
				{ ".mp4","video/mpeg4" },
				{ ".rmvb","application/vnd.rn-realmedia-vbr" },
				{ ".zip","application/zip" },
				{ ".zip","application/zip" },
			};
			auto p = m.find(ext);
			if (p == m.end())
				return defualt_d;
			return p->second;
		}

		//获取状态说明
		static std::string http_reasion(const std::string& status, const std::string& defualt_r = "NONE")
		{
			static std::map<std::string, std::string> m =\
			{ 
				{ "200","OK" },
				{ "400","Bad Request" },
				{ "401","Unauthorized" },
				{ "403","Forbidden" },
				{ "404","Not Found" },
				{ "405","Internal Server Error" },
				{ "503","Server Unavailable" },
			};
			auto p = m.find(status);
			if (p == m.end())
				return defualt_r;
			return p->second;
		}

		//解析url
		struct UrlParam
		{
			std::string protocol;
			std::string host;
			std::string port;
			std::string path;
			std::string query_string;

		};
		static bool parser_url(const std::string& url, UrlParam& out)
		{
			// http://www.baidu.com:8080/in?q=q&
			size_t pos = 0;
			auto p = url.find("://", pos);
			if (p != std::string::npos)
			{
				out.protocol = url.substr(0, p);
				pos = p+3;
			}

			p = url.find("/", pos);
			if (p != std::string::npos)
			{
				auto str = url.substr(pos, p-pos);
				auto v = split(str, ":");
				out.host = v[0];
				if (v.size() >= 2)
				{
					out.port = v[1];
				}
				pos = p+1;
			}
			else
			{
				auto str = url.substr(pos);
				auto v = split(str, ":");
				out.host = v[0];
				if (v.size() >= 2)
				{
					out.port = v[1];
				}
				return true;
			}
			p = url.find("?", pos);
			if (p != std::string::npos)
			{
				out.path = url.substr(pos, p - pos);
				pos = p + 1;
				out.query_string = url.substr(pos);
			}
			else
			{
				out.path = url.substr(pos);
			}
			return true;
		}
	}
}
#endif // !HNW_HTTP_PARSER_DEFINE_HPP_
