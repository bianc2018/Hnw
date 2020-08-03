/*
	http请求行实例
*/
#ifndef HNW_HTTP_REQUEST_LINE_HPP_
#define HNW_HTTP_REQUEST_LINE_HPP_
#include "../hnw_http.h"
#include "../util/http_util.hpp"
namespace hnw
{
	namespace http
	{
		class RequestLineImpl :public HnwHttpRequestLine
		{
        public:
            //start line
            RequestLineImpl()
                :method_("GET"),
                path_("/"),
                version_("HTTP/1.1")
            {

            }

            virtual std::string method()
            {
                return method_;
            }
            
            virtual void method(const std::string& m)
            {
                method_ = m;
            }
            
            virtual std::string url()
            {
                std::string url = path_ + "?";
                for (auto m : query_map_)
                {
                    url += m.first + "=" + m.second + "&";
                }
                return url.substr(0, url.size() - 1);
            }

            virtual void url(const std::string& m) 
            {
                //解析
                auto path_q = util::split(m, "?");
                path(path_q[0]);
                if (path_q.size() >= 2)
                {
                    auto q_v = util::split(path_q[1], "&");
                    for (auto q : q_v)
                    {
                        auto k_v = util::split(q, "=");
                        if (k_v.size() >=2)
                        {
                            add_query(k_v[0], k_v[1]);
                        }
                        else
                        {
                            add_query(k_v[0], "");
                        }

                    }
                }
            }

            virtual std::string version()
            {
                return version_;
            }

            virtual void version(const std::string& m)
            {
                version_ = m;
            }

            //转换为字符串
            virtual std::string string()
            {
                return method_ + util::SPACE + url()\
                    + util::SPACE + version_;
            }
            virtual bool string(const std::string& raw)
            {
                return false;
            }

            virtual bool path(const std::string& p)
            {
                path_ = p;
                return true;
            }
            virtual std::string path()
            {
                return path_;
            }
            virtual std::vector<std::string> path_ver()
            {
                return util::split(path_, "/");
            }
            virtual std::string  get_query(const std::string& key, const std::string & not= "")
            {
                auto p = query_map_.find(key);
                if (query_map_.end() == p)
                {
                    return not;
                }
                return p->second;
            }
            virtual bool  add_query(const std::string& key, const std::string& value)
            {
                //直接覆盖
                query_map_[key] = value;
                return true;
            }
        public:
            static SPHnwHttpRequestLine generate()
            {
                return std::make_shared<RequestLineImpl>();
            }

        private:
            
        private:
            std::string method_, version_;
            std::string path_;
            std::map<std::string, std::string> query_map_;
		};
	}
}
#endif