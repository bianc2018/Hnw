/*
    http �����Ĺ��ж���
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
		
		//ð��  colon
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

		//�ֿ鴫��
		const std::string HTTP_CHUNKED("chunked");
		
		
		//��������
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
					//��ȡ
					res.push_back(src.substr(beg,pos-beg));

					//����
					beg = pos + splitor.size();
				}

			} while (true);

			return res;
		}

		//��ȡ��Ӧ��MIME
		static std::string http_mime(const std::string& ext, const std::string& defualt_d = HTML_MIME)
		{
			return defualt_d;
		}

		//��ȡ״̬˵��
		static std::string http_reasion(const std::string& status, const std::string& defualt_r = "NONE")
		{
			return defualt_r;
		}

		//����url
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
				out.host = url.substr(pos);
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
