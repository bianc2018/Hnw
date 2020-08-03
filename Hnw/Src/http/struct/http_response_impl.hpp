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
				body = RawBodyImpl::generate();
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
					return false;
				}
				return true;
			}
		};
	}
}
#endif