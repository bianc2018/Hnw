/*
    http回复行实例
*/
#ifndef HNW_HTTP_RESPONSE_LINE_HPP_
#define HNW_HTTP_RESPONSE_LINE_HPP_
#include "../hnw_http.h"
#include "../util/http_util.hpp"
namespace hnw
{
    namespace http
    {
        class ResponseLineImpl :public HnwHttpResponseLine
        {
        public:
            ResponseLineImpl():
                version_("HTTP/1.1"),
                status_code_("200"),
                reason_("OK")
            {

            }

            //start line
            virtual std::string version()
            {
                return version_;
            }
            virtual void version(const std::string& m)
            {
                version_ = m;
            }
            
            virtual std::string status_code()
            {
                return status_code_;
            }
            virtual void status_code(const std::string& m)
            {
                reason_=util::http_reasion(m);
                status_code_ =m;
            }

            virtual std::string reason()
            {
                return reason_;
            }
            virtual void reason(const std::string& m)
            {
                reason_ = m;
            }

            //转换为字符串
            virtual std::string string()
            {
               return version_ + util::SPACE + status_code_ + util::SPACE + reason_;
            }

            virtual bool string(const std::string& raw)
            {
                return false;
            }
        public:
            static SPHnwHttpResponseLine generate()
            {
                return std::make_shared<ResponseLineImpl>();
            }
        private:
            std::string version_, status_code_, reason_;
        };
    
    }
}
#endif