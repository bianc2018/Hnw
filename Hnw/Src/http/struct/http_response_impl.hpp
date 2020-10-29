/*
	http 回复实例
*/
#ifndef HNW_HTTP_RESPONSE_IMPL_HPP_
#define HNW_HTTP_RESPONSE_IMPL_HPP_
#include "../hnw_http.h"
#include "http_response_line.hpp"
#include "http_head_impl.hpp"

#include "http_body_raw.hpp"
#include "http_body_file.hpp"
namespace hnw
{
	namespace http
	{
		//请求实例
		class ResponseImpl :public HnwHttpResponse
		{
			//基础数据
		public:
			ResponseImpl()
			{
				line = ResponseLineImpl::generate();
				head = HeadImpl::generate();
			//	body = RawBodyImpl::generate();
				//body = RawBodyImpl::generate();
			}


		public:
			static SPHnwHttpResponse generate()
			{
				return std::make_shared<ResponseImpl>();
			}

		public:
			virtual bool auto_create_body()
			{
				auto size = head->get_head_int64(util::HTTP_LEN, 0);
				if (size <= 1024 * 1024 * 10)
				{
					body = RawBodyImpl::generate(size);
				}
				else
				{
					HttpRange range;
					range.total = size;
					body = FileBodyImpl::generate("./"+ util::uuid()+".HnwHttpTemp", range, HTTP_FILE_FLAG_DELETE|HTTP_FILE_FLAG_CREATE);
				}
				return true;
			}

			virtual bool set_file_body(const std::string& path,\
				const HttpRange &req_range= HttpRange(),
				int flag = HTTP_FILE_FLAG_OPEN)override
			{
				HttpRange range = req_range;
				auto temp_body = http::FileBodyImpl::generate(path, range, flag);
				if (temp_body)
				{
					range.total = PTR_CAST(FileBodyImpl, temp_body)->file_size();
					head->set_content_range(range);
					if (head->content_type().empty())
					{
						head->content_type(util::get_http_mime_by_uri(path));
					}
					body = temp_body;
					return true;
				}
				return false;
			}

			//save body as file
			virtual bool save_body_as_file(const std::string& path,
				int flag = HTTP_FILE_FLAG_OPEN)
			{
				if (!body)
					return false;

				std::fstream file(path, std::ios::out | std::ios::binary);
				if (!file)
				{
					return false;
				}
				const size_t buff_len = 2 * 1024;
				char buff[buff_len] = { 0 };
				while (true)
				{
					auto read_len = body->read_body(buff, buff_len);
					if (read_len == 0)
						break;
					file.write(buff, buff_len);
				}
				file.close();
				return true;
			}
		};
	}
}
#endif