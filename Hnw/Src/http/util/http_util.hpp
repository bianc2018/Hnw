/*
    http 共有定义
    hql 2020 04 15
*/
#ifndef HNW_HTTP_UTIL_HPP_
#define HNW_HTTP_UTIL_HPP_
#include "define/parser.hpp"
#include "../hnw_http.h"

#ifdef _WIN32
#include "windows.h"
#elif __linux__

#endif
#include <iostream>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#define GET_VALUE_FOR_MAP(map,key,value,default_value)\
do\
{\
	auto d = map.find(key);\
	if (d == map.end())\
	{\
		value = default_value;\
	}\
	else\
	{\
		value = d->second;\
	}\
} while (false)
#define GET_VALUE_FOR_MAP_STR(map,key,value) GET_VALUE_FOR_MAP(map,key,value,"")
namespace hnw
{
	namespace util
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

		//方法
		const std::string HTTP_METHOD_GET("GET");
		const std::string HTTP_METHOD_POST("POST");
		const std::string HTTP_METHOD_HEAD("HEAD");
		const std::string HTTP_METHOD_PUT("PUT");
		const std::string HTTP_METHOD_DELETE("DELETE");
		//STATUS Partial
		const std::string HTTP_STATUS_OK("200");
		const std::string HTTP_STATUS_PARTIAL("206");
		const std::string HTTP_STATUS_FORBIDDEN("403");
		const std::string HTTP_STATUS_NOT_FOUND("404");
		const std::string HTTP_STATUS_MOVE_P("301");
		//临时变更
		const std::string HTTP_STATUS_MOVE_T("302");

		//MIME
		const std::string JSON_MIME("application/json; charset=UTF-8");
		const std::string XML_MIME("application/rss+xml");
		const std::string HTML_MIME("text/html");
		const std::string OCT_MIME("application/octet-stream");
		const std::string ACCEPT_ALL("*/*");
		const std::string HNW_HTTP_UA("HnwHttpClient/1.0");

		//HEAD location User-Agent
		const std::string HTTP_HOST("Host");
		const std::string HTTP_UA("User-Agent");
		const std::string HTTP_LOC("Location");
		const std::string HTTP_ACCEPT("Accept");
		const std::string HTTP_CT("Content-Type");
		const std::string HTTP_LEN("Content-Length");
		const std::string HTTP_CONN("Connection");
		const std::string HTTP_CONN_CLOSE("Close");
		const std::string HTTP_CONN_KEEPALIVE("keep-alive");
		const std::string HTTP_RANGE("Range");
		const std::string HTTP_CONTENT_RANGE("Content-Range");
		const std::string HTTP_TRANSFER_ENCODING("Transfer-Encoding");
		const std::string HTTP_SET_COOKIE("Set-Cookie");
		const std::string HTTP_COOKIE("Cookie");
		const std::string HTTP_WWW_AUTH("WWW-Authenticate");
		const std::string HTTP_AUTH("Authorization");
		const std::string HTTP_RANGE_BYTE("bytes");
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
				pos = src.find(splitor, beg);

				if (std::string::npos == pos)
				{
					res.push_back(src.substr(beg));
					break;
				}
				else
				{
					//auto t = src.substr(beg, pos - beg);
					//截取
					res.push_back(src.substr(beg, pos - beg));

					//步进
					beg = pos + splitor.size();
				}

			} while (true);

			return res;
		}

		//获取对应的MIME .mp4
		static std::string http_mime(const std::string& ext, 
			const std::string& default_d = OCT_MIME)
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
				//{ ".txt","text/plain" },
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
				return default_d;
			return p->second;
		}

		//获取状态说明  206 Partial Content302 Move Temporarily
		static std::string http_reasion(const std::string& status, const std::string& defualt_r = "NONE")
		{
			static std::map<std::string, std::string> m = \
			{
				{ "200", "OK" },
				{ "206","Partial Content" },
				{ "301","Moved Permanently" },
				{ "302","Move Temporarily" },
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

		//获取uri对应mime
		static std::string get_http_mime_by_uri(const std::string& uri,
			const std::string& default_d = OCT_MIME)
		{
			//.
			auto p = uri.find_last_of('.');
			if (p == std::string::npos)
				return default_d;
			auto ext = uri.substr(p);
			return http_mime(ext, default_d);
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
				pos = p + 3;
			}

			p = url.find("/", pos);
			if (p != std::string::npos)
			{
				auto str = url.substr(pos, p - pos);
				auto v = split(str, ":");
				out.host = v[0];
				if (v.size() >= 2)
				{
					out.port = v[1];
				}
				//pos = p+1;
				pos = p;
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
		// /之后的东西
		std::string get_path_for_url(const std::string& url)
		{
			UrlParam param;
			parser_url(url, param);
			if (param.query_string.empty())
				return param.path;
			else
				return param.path + "?" + param.query_string;
		}
		//之前
		std::string get_host_for_url(const std::string& url)
		{
			UrlParam param;
			parser_url(url, param);

			std::string result;
			if (param.protocol.empty())
				result += "http://";
			else
				result += param.protocol + "://";

			if (param.host.empty())
				return "";
			result += param.host;

			if (param.port.empty())
				return result;
			else
				return result + ":" + param.port;
		}
		//转换为大写
		static std::string to_upper(const std::string& src)
		{
			std::string dst = src;
			for (auto& c : dst)
			{
				//转换到大写：原值 & 1101 1111

				//转换到小写：原值 | 0010 0000
				if ('a' <= c || c <= 'z')
				{
					c = std::toupper(c);
				}
			}
			return dst;
		}
		//小写 lower 
		static std::string to_lower(const std::string& src)
		{
			std::string dst = src;
			for (auto& c : dst)
			{
				//转换到大写：原值 & 1101 1111

				//转换到小写：原值 | 0010 0000
				if ('A' <= c || c <= 'Z')
				{
					c = std::tolower(c);
				}
			}
			return dst;
		}

#ifdef _WIN32
		static bool create_null_file(const std::string& path, std::uint64_t size)
		{
			DWORD dwResult = 0;
			HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == hFile)
			{
				PRINTFLOG(BL_ERROR, "Windows CreateFile Fail code =%u", GetLastError());
				return false;
			}
			const  std::uint64_t s4g = 1024i64 * 1024i64 * 1024i64 * 4;
			HANDLE hFileMap = CreateFileMapping(hFile, NULL,
				PAGE_READWRITE, size / s4g, size % s4g, NULL);
			if (NULL == hFileMap)
			{
				PRINTFLOG(BL_ERROR, "Windows CreateFileMapping Fail code =%u", GetLastError());
				return false;
			}

			CloseHandle(hFileMap);
			CloseHandle(hFile);

			return true;
		}
#elif __linux__
		static bool create_null_file(const std::string& path, std::uint64_t size)
		{
			return false;
		}
#endif
		static std::uint64_t file_size(const std::string& file_name)
		{
			return boost::filesystem::file_size(file_name);
		}

		static bool remove_file(const std::string& file_name)
		{
			boost::system::error_code ec;
			boost::filesystem::remove_all(file_name, ec);
			if (ec)
			{
				return false;
			}
			return true;
		}

		static bool file_is_exist(const std::string& file_name)
		{
			return boost::filesystem::exists(file_name);
		}

		/*
		//只是打开
		#define HTTP_FILE_FLAG_OPEN 0x00
		//新建 不存在创建
		#define HTTP_FILE_FLAG_CREATE 0x01
		//已存在的创建 新的
		#define HTTP_FILE_FLAG_CREATE_NEW 0x08
		*/
		static bool do_file_by_flag(const std::string& file_name, std::uint64_t size, int flag)
		{
			//是否存在？
			if (!file_is_exist(file_name))
			{
				//不存在，是否新建
				if (flag & HTTP_FILE_FLAG_CREATE || flag & HTTP_FILE_FLAG_CREATE_NEW)
				{
					if (!util::create_null_file(file_name, size))
					{
						PRINTFLOG(BL_ERROR, "create_null_file fail name=%s",
							file_name.c_str());
						return false;
					}
				}
				else
				{
					PRINTFLOG(BL_ERROR, "file %s is not found,set "
						"flag HTTP_FILE_FLAG_CREATE create file",
						file_name.c_str());
					return false;
				}
			}
			else
			{
				//存在，是否新建
				if (flag & HTTP_FILE_FLAG_CREATE_NEW)
				{
					util::remove_file(file_name);

					if (!util::create_null_file(file_name, size))
					{
						PRINTFLOG(BL_ERROR, "create_null_file fail name=%s",
							file_name.c_str());
						return false;
					}
				}

			}

			return true;
		}

		//获取文件后缀
		static std::string get_ext(const std::string& str)
		{
			auto p = str.find_last_of(".");
			if (p == std::string::npos)
			{
				return "";
			}
			else
			{
				return str.substr(p);
			}
		}


		char dec2hexChar(short int n)
		{
			if (0 <= n && n <= 9)
			{
				return char(short('0') + n);
			}
			else if (10 <= n && n <= 15)
			{
				return char(short('A') + n - 10);
			}
			else
			{
				return char(0);
			}
		}

		short int hexChar2dec(char c)
		{
			if ('0' <= c && c <= '9')
			{
				return short(c - '0');
			}
			else if ('a' <= c && c <= 'f')
			{
				return (short(c - 'a') + 10);
			}
			else if ('A' <= c && c <= 'F') {
				return (short(c - 'A') + 10);
			}
			else {
				return -1;
			}
		}

		std::string encode_url(const std::string& URL)
		{
			std::string result = "";
			for (unsigned int i = 0; i < URL.size(); i++)
			{
				char c = URL[i];
				if (
					('0' <= c && c <= '9') ||
					('a' <= c && c <= 'z') ||
					('A' <= c && c <= 'Z') ||
					c == '/' || c == '.'
					)
				{
					result += c;
				}
				else
				{
					int j = (short int)c;
					if (j < 0)
					{
						j += 256;
					}
					int i1, i0;
					i1 = j / 16;
					i0 = j - i1 * 16;
					result += '%';
					result += dec2hexChar(i1);
					result += dec2hexChar(i0);
				}
			}
			return result;
		}

		std::string decode_url(const std::string& URL)
		{
			std::string result = "";
			for (unsigned int i = 0; i < URL.size(); i++)
			{
				char c = URL[i];
				if (c != '%')
				{
					result += c;
				}
				else
				{
					char c1 = URL[++i];
					char c0 = URL[++i];
					int num = 0;
					num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
					result += char(num);
				}
			}
			return result;
		}

		std::string conv(const std::string source,\
			const std::string& from, const std::string& to)
		{
			try
			{
				return boost::locale::conv::between(source, from, to);
			}
			catch (std::exception&)
			{
				return "";
			}
		}

		std::string ansi_to_utf8(const std::string source)
		{
			//return conv(source, "GB2312", "UTF-8");
			return boost::locale::conv::to_utf<char>(source, std::string("gb2312"));
		}
		std::string utf8_to_ansi(const std::string source)
		{
			//return conv(source, "UTF-8", "GB2312");
			return boost::locale::conv::from_utf<char>(source, std::string("gb2312"));
		}

		//base64
		std::string encode_base64(const std::string& src)
		{
			typedef boost::archive::iterators::base64_from_binary<\
				boost::archive::iterators::transform_width<\
				std::string::const_iterator, 6, 8> > Base64EncodeIterator;
			
				std::stringstream result;
			
				std::copy(Base64EncodeIterator(src.begin()),\
					Base64EncodeIterator(src.end()),\
					std::ostream_iterator<char>(result));
			
				size_t equal_count = (3 - src.length() % 3) % 3;
			
				//字节数不足3个的=补全
				for (size_t i = 0; i < equal_count; i++) 
				{
						result.put('=');
				}
			
				return  result.str();
		}
		std::string decode_base64(const std::string& src)
		{
			typedef boost::archive::iterators::transform_width<\
				boost::archive::iterators::binary_from_base64<\
				std::string::const_iterator>,\
				8, 6>  Base64DecodeIterator;
			
			std::stringstream result;
			try
			{
				std::copy(Base64DecodeIterator(src.begin()), \
					Base64DecodeIterator(src.end()), \
					std::ostream_iterator<char>(result));
			}
			catch (const std::exception&e)
			{
				PRINTFLOG(BL_ERROR, "decode base64 %s exception :%s",src.c_str(),e.what());
				return "";
			}
			
			return  result.str().c_str();
		}


		//md5摘要
		std::string md5(const std::string& src)
		{
			std::string dst;
			if (src.empty()) {
				return "";
			}

			boost::uuids::detail::md5 boost_md5;
			boost_md5.process_bytes(src.c_str(), src.size());
			boost::uuids::detail::md5::digest_type digest;
			boost_md5.get_digest(digest);
			/*const auto char_digest = reinterpret_cast<const char*>(&digest);
			boost::algorithm::hex(char_digest, \
				char_digest + sizeof(boost::uuids::detail::md5::digest_type), \
				std::back_inserter(dst));*/

			for (int i = 0; i < 4; ++i) {
				char buff[20] = {};
				snprintf(buff, 20, "%08x", digest[i]);
				dst += buff;
			}
			return dst;
		}

		//sha1
		std::string sha1(const std::string& src)
		{
			std::string dst;
			if (src.empty()) {
				return "";
			}
			char hash[20] = {0};
			boost::uuids::detail::sha1 boost_sha1;
			boost_sha1.process_bytes(src.c_str(), src.size());
			boost::uuids::detail::sha1::digest_type digest;
			boost_sha1.get_digest(digest);
			const char* tmp = reinterpret_cast<char*>(digest);
			for (int i = 0; i < 5; ++i) {
				
				hash[i * 4] = tmp[i * 4 + 3];
				hash[i * 4 + 1] = tmp[i * 4 + 2];
				hash[i * 4 + 2] = tmp[i * 4 + 1];
				hash[i * 4 + 3] = tmp[i * 4];
			}
			for (int i = 0; i < 20; ++i) {
				char buff[20] = {};
				snprintf(buff, 20, "%02X", hash[i]);
				dst += buff;
			}

			return  dst;
			/*std::ostringstream buf;
			for (int i = 0; i < 20; ++i) {
				buf << setiosflags(ios::uppercase) << std::hex << ((hash[i] & 0x0000000F0) >> 4);
				buf << setiosflags(ios::uppercase) << std::hex << (hash[i] & 0x00000000F);
			}

			return  buf.str();*/
		}


		//去除首尾空格
		std::string trim(const std::string& src)
		{
			if (src.empty())
			{
				return "";
			}
			
			int beg = 0, end = src.size()-1;
			for (; beg < end; ++beg)//找开头第一个不是 空格的字符
			{
				if (src[beg] != ' ')
				{
					break;
				}
			}
			if (beg == end&&(' ' == src[beg]))//全是空格
			{
				return "";
			}
			for (; end>=beg; --end)//找结尾第一个不是 空格的字符
			{
				if (src[end] != ' ')
				{
					return src.substr(beg, (end - beg+1));
				}
			}
			return "";//全是空格,应该不会到这里
		}
	}
}
#endif // !HNW_HTTP_PARSER_DEFINE_HPP_
