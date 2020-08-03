/*
	http 请求实例
*/
#ifndef HNW_HTTP_REQUEST_IMPL_HPP_
#define HNW_HTTP_REQUEST_IMPL_HPP_
#include "../hnw_http.h"
#include "http_request_line.hpp"
#include "http_head_impl.hpp"

#include "http_body_raw.hpp"
#include "http_body_impl.hpp"
namespace hnw
{
	namespace http
	{
		//请求实例
		class RequestImpl:public HnwHttpRequest
		{
            //基础数据
        public:
			RequestImpl()
			{
				line = RequestLineImpl::generate();
				head = HeadImpl::generate();
				//body = RawBodyImpl::generate();
				body = RawBodyImpl::generate();
			}
		public:
			static SPHnwHttpRequest generate()
			{
				return std::make_shared<RequestImpl>();
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