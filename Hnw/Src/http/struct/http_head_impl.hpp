/*
	http 头部实例
*/
#ifndef HNW_HTTP_HEAD_IMPL_HPP_
#define HNW_HTTP_HEAD_IMPL_HPP_
#include "../hnw_http.h"
#include "../util/http_util.hpp"
namespace hnw
{
	namespace http
	{
		class HeadImpl :public HnwHttpHead
		{
		public:
            //head
            virtual void add_head(const std::string& key, const std::string& value)
            {
                //head_.insert(std::make_pair(util::to_lower(key), util::to_lower(value)));
                head_.insert(std::make_pair(key,value));
            }
            virtual void add_head(const std::string& key, const double& value)
            {
                //head_.insert(std::make_pair(util::to_lower(key), std::to_string(value)));
                add_head(key, std::to_string(value));
            }

            virtual void add_head_double(const std::string& key, const double& value)
            {
                //head_.insert(std::make_pair(util::to_lower(key), std::to_string(value)));
                add_head(key, std::to_string(value));
            }
            virtual void add_head_int64(const std::string& key, const std::int64_t& value)
            {
                //head_.insert(std::make_pair(util::to_lower(key), std::to_string(value)));
                add_head(key, std::to_string(value));
            }
            virtual void add_head_uint64(const std::string& key, const std::uint64_t& value)
            {
                //head_.insert(std::make_pair(util::to_lower(key), std::to_string(value)));
                add_head(key, std::to_string(value));
            }

            //删除对于键值的数据
            virtual void del_head(const std::string& key)
            {
                head_.erase(key);
            }

            virtual std::vector<std::string> get_heads(const std::string& key)
            {
                std::vector<std::string> result;
                auto p = head_.find(key);
                while (head_.end() != p)
                {
                    result.push_back(p->second);
                    ++p;
                }
                return result;
            }
            virtual std::string get_head(const std::string& key, const std::string & not)
            {
                //auto p = head_.find(util::to_lower(key));
                auto p = head_.find(key);
                if (head_.end() == p)
                    return not;
                return p->second;
            }
            virtual double get_head_double(const std::string& key, const double& not)
            {
                auto str = get_head(key, "");
                if (str.empty())
                    return not;
                try
                {
                    return std::stod(str);
                }
                catch (const std::exception&)
                {
                    return not;
                }
            }
            virtual std::int64_t get_head_int64(const std::string& key, const std::int64_t & not)
            {
                auto str = get_head(key, "");
                if (str.empty())
                    return not;
                try
                {
                    return std::stoll(str);
                }
                catch (const std::exception&)
                {
                    return not;
                }
            }
            virtual std::uint64_t get_head_uint64(const std::string& key, const std::uint64_t & not)
            {
                auto str = get_head(key, "");
                if (str.empty())
                    return not;
                try
                {
                    return std::stoull(str);
                }
                catch (const std::exception&)
                {
                    return not;
                }
            }
            virtual std::unordered_multimap<std::string, std::string >& get_all_head()
            {
                return head_;
            }
            virtual size_t get_head_count(const std::string& key)
            {
               //return head_.count(util::to_lower(key));
                return head_.count(key);
            }

            virtual size_t get_head_count()
            {
                return head_.size();
            }

            //
            virtual std::string string()
            {
                std::string str;
                for (auto &h : head_)
                {
                    str += h.first + util::HEAD_SPLIT + h.second + util::CRLF;
                }
                return str;
            }
            //k:v\r\nk:v
            virtual bool string(const std::string& raw)
            {
                return false;
            }
            //k:v
            virtual bool string_kv(const std::string& raw)
            {
                return false;
            }

            virtual bool merge(std::shared_ptr<HnwHttpHead> other, bool bcoverage = true)
            {
                if (nullptr == other)
                    return true;

                auto ps = other->get_all_head();
                for (auto& p : ps)
                {
                    if (0 != get_head_count(p.first))
                    {
                        //覆盖
                        if (bcoverage)
                        {
                            del_head(p.first);
                        }
                        else
                        {
                            //不覆盖
                            continue;
                        }
                    }
                    add_head(p.first, p.second);
                }
                return true;
            }

            //设置/获取 请求的范围
            virtual bool get_range(HttpRange& range)
            {
                //HTTP_RANGE
                auto src = get_head(util::HTTP_RANGE, "");
                if (src.empty())
                {
                    PRINTFLOG(BL_ERROR, "http head %s not found ", util::HTTP_RANGE.c_str());
                    return false;
                }
                //Range: bytes=0-1551;1-3
                //分割=
                auto bv = util::split(src, "=");
                if (bv.size() != 2||(bv[0].find(util::HTTP_RANGE_BYTE)== std::string::npos))
                {
                    PRINTFLOG(BL_ERROR, "http head %s not support like %s ",
                        util::HTTP_RANGE.c_str(), src.c_str());
                    return false;
                }
                //区间
                auto vec = util::split(bv[1], ";");
                
                for (auto i = 0; i < vec.size(); ++i)
                {
                    //只取第一个不支持多个的情况
                    auto r = util::split(vec[i], "-");
                    try
                    {
                        if (r[0].size() == 0)
                        {
                            range.use_start_endpoint = true;
                        }
                        else
                        {
                            range.use_start_endpoint = false;
                            range.start = std::stoull(r[0]);
                        }
                        if (r.size() >= 2)
                        {
                            if (r[1].size() == 0)
                            {
                                range.use_end_endpoint = true;
                            }
                            else
                            {
                                range.use_end_endpoint = false;
                                range.end = std::stoull(r[1]);
                            }
                        }
                    }
                    catch (const std::exception& e)
                    {
                        PRINTFLOG(BL_ERROR, "http head %s not support like %s ",
                            util::HTTP_RANGE.c_str(), src.c_str());
                        return false;
                    }
                    break;
                }
                return true;
            }
            virtual bool set_range(const HttpRange& range)
            {
                //bytes=start-end
                const int tmp_size = 32;
                char tmp[tmp_size] = { 0 };
                if (range.use_end_endpoint)
                {
                    if (snprintf(tmp, tmp_size, "%s=%llu-",
                        util::HTTP_RANGE_BYTE.c_str(),
                        range.use_start_endpoint ? 0 : range.start) >= 32)
                    {
                        PRINTFLOG(BL_ERROR, "BUFF ERROR");
                        return false;
                    }
                }
                else
                {
                    if (snprintf(tmp, tmp_size, "%s=%llu-%llu",
                        util::HTTP_RANGE_BYTE.c_str(),
                        range.use_start_endpoint ? 0 : range.start,range.end) >= 32)
                    {
                        PRINTFLOG(BL_ERROR, "BUFF ERROR");
                        return false;
                    }
                }
                del_head(util::HTTP_RANGE);
                add_head(util::HTTP_RANGE, tmp);
                return true;
            }
            //设置/获取 回复的范围 
            virtual bool get_content_range(HttpRange& range)
            {
                auto src = get_head(util::HTTP_CONTENT_RANGE, "");
                if (src.empty())
                {
                    PRINTFLOG(BL_ERROR, "http head %s not found ",
                        util::HTTP_CONTENT_RANGE.c_str());
                    return false;
                }

                //Content-Range: bytes 0-10/3103
                //分割=
                auto bv = util::split(src, " ");
                if (bv.size() != 2 || (bv[0].find(util::HTTP_RANGE_BYTE) == std::string::npos))
                {
                    PRINTFLOG(BL_ERROR, "http head %s not support like %s ",
                        util::HTTP_CONTENT_RANGE.c_str(), src.c_str());
                    return false;
                }
                //区间0-10/3103
                auto vec = util::split(bv[1], "/");

                if (vec.size() == 2)
                {
                    try
                    {
                        //区间
                        auto r = util::split(vec[0], "-");
                        if (r[0].size() == 0)
                        {
                            range.use_start_endpoint = true;
                        }
                        else
                        {
                            range.use_start_endpoint = false;
                            range.start = std::stoull(r[0]);
                        }
                        if (r.size() >= 2)
                        {
                            if (r[1].size() == 0)
                            {
                                range.use_end_endpoint = true;
                            }
                            else
                            {
                                range.use_end_endpoint = false;
                                range.end = std::stoull(r[1]);
                            }
                        }

                        //总数
                        range.total = std::stoull(vec[1]);
                    }
                    catch (const std::exception& e)
                    {
                        PRINTFLOG(BL_ERROR, "http head %s not support like %s ",
                            util::HTTP_CONTENT_RANGE.c_str(), src.c_str());
                        return false;
                    }
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "http head %s not support like %s ",
                        util::HTTP_RANGE.c_str(), src.c_str());
                    return false;
                }
                return true;
            }
            virtual bool set_content_range(const HttpRange& range)
            {
                //Content-Range: bytes 0-10/3103
                const int tmp_size = 256;
                char tmp[tmp_size] = { 0 };
                if (snprintf(tmp, tmp_size, "%s %llu-%llu/%llu",
                    util::HTTP_RANGE_BYTE.c_str(),
                    range.section_start(), range.section_end(),range.total) >= tmp_size)
                {
                    PRINTFLOG(BL_ERROR, "BUFF ERROR");
                    return false;
                }
                del_head(util::HTTP_CONTENT_RANGE);
                add_head(util::HTTP_CONTENT_RANGE, tmp);
                return true;
            }

            //设置/获取 content-type
            virtual bool content_type(const std::string& value)
            {
                add_head(util::HTTP_CT, value);
                return true;
            }
            virtual bool set_content_type_by_ext(const std::string& ext)
            {
                auto ct = util::http_mime(ext,"");
                if (ct.empty())
                    return  false;
                return content_type(ct);
            }
            virtual bool set_content_type_by_uri(const std::string& uri)
            {
                return set_content_type_by_ext(util::get_ext(uri));
            }
            virtual std::string content_type()
            {
                return get_head(util::HTTP_CT, "");
            }

            //设置cookie
            //服务端
            virtual bool set_setcookie(const HnwSetCookie& cookie)override
            {
                add_head(util::HTTP_SET_COOKIE, set_cookie_to_string(cookie));
                return true;
            }
            virtual std::vector<HnwSetCookie> get_setcookies()
            {
                auto strs = get_heads(util::HTTP_SET_COOKIE);
                std::vector<HnwSetCookie> res;
                for (auto str : strs)
                {
                    HnwSetCookie s;
                    if (set_cookie_from_string(str, s))
                    {
                        res.push_back(s);
                    }
                }
                return res;
            }

            virtual bool get_setcookie(const std::string& name, HnwSetCookie& out)
            {
                auto strs = get_heads(util::HTTP_SET_COOKIE);
                
                for (auto str : strs)
                {
                    HnwSetCookie s;
                    if (set_cookie_from_string(str, s))
                    {
                        if (s.value.key == name)
                        {
                            out = s;
                            return true;
                        }
                    }
                }
                return false;
            }
            
            virtual bool del_setcookie(const std::string& name)
            {
                auto p = head_.find(util::HTTP_SET_COOKIE);
                while (head_.end() != p)
                {
                    HnwSetCookie s;
                    if (set_cookie_from_string(p->second, s))
                    {
                        if (s.value.key == name)
                        {
                            head_.erase(p);
                            return true;
                        }
                    }
                }
                return false;
            }
            //客户端
            virtual bool set_cookie(const HnwCookie& cookie)
            {
                auto data = get_cookies();
                data.push_back(cookie);
                del_head(util::HTTP_COOKIE);
                add_head(util::HTTP_COOKIE, cookie_to_string(data));
                return true;
            }
            virtual std::vector<HnwCookie> get_cookies()
            {
                auto strs = get_heads(util::HTTP_COOKIE);
                std::vector<HnwCookie> res;
                for (auto str : strs)
                {
                    std::vector<HnwCookie> s;
                    if (cookie_from_string(str, s))
                    {
                        res.insert(res.end(),s.begin(),s.end());
                    }
                }
                return res;
            }
            virtual bool get_cookie(const std::string& name, HnwCookie& out)
            {
                auto data = get_cookies();
                for (auto& d : data)
                {
                    if (d.key == name)
                    {
                        out = d;
                        return true;
                    }
                }
                return false;
            }
            virtual bool del_cookie(const std::string& name)
            {
                auto data = get_cookies();
                for (auto it = data.begin(); it != data.end();)
                {
                    if (it->key == name)
                        it = data.erase(it);
                    else
                        ++it;
                }
                del_head(util::HTTP_COOKIE);
                add_head(util::HTTP_COOKIE, cookie_to_string(data));
                return true;
            }

            //保活
            virtual bool keep_alive()
            {
                if (get_head(util::HTTP_CONN, util::HTTP_CONN_CLOSE)\
                    == util::HTTP_CONN_CLOSE)
                {
                    return false;
                }
                return true;
            }

            virtual void keep_alive(bool is_alive)
            {
                del_head(util::HTTP_CONN);
                add_head(util::HTTP_CONN, \
                    is_alive ? util::HTTP_CONN_KEEPALIVE : util::HTTP_CONN_CLOSE);
            }
        private:
            virtual bool cookie_from_string(const std::string& str, std::vector<HnwCookie>& cookies)
            {
                auto kvs = util::split(str, ";");
                for (auto kv : kvs)
                {
                    HnwCookie cookie;
                    auto k_v = util::split(kv, "=");
                    cookie.key = k_v[0];
                    if (k_v.size() >= 2)
                    {
                        cookie.value = k_v[1];
                    }
                    cookies.push_back(cookie);
                }
                return true;
            }

            virtual std::string cookie_to_string(const std::vector<HnwCookie>& cookies)
            {
                std::string res;
                for (auto c : cookies)
                {
                    res += c.key + "=" + c.value + ";";
                }
                return res;
            }
            virtual bool set_cookie_from_string(const std::string& str, HnwSetCookie& cookies)
            {
                auto kvs = util::split(str, ";");
                bool first = true;
                for (auto kv : kvs)
                {
                    auto k_v = util::split(kv, "=");
                    if (first)
                    {
                        cookies.value.key = k_v[0];
                        if (k_v.size() >= 2)
                        {
                            cookies.value.value = k_v[1];
                        }
                        first = false;
                        continue;
                    }
                    if (k_v[0] == "Expires")
                    {
                        cookies.expires = k_v[1];
                    }
                    else if (k_v[0] == "Path")
                    {
                        cookies.path = k_v[1];
                    }
                    else if (k_v[0] == "Domain")
                    {
                        cookies.domain = k_v[1];
                    }
                  /*  else if (k_v[0] == "Max-Age")
                    {
                        cookies.max_age = std::stod(k_v[1]);
                    }*/
                    else if (k_v[0] == "HttpOnly")
                    {
                        cookies.is_secure =false;
                    }
                }
                return true;
            }
            virtual std::string set_cookie_to_string(const HnwSetCookie& cookies)
            {
                auto res = cookies.value.key + "=" + cookies.value.value ;
                if (!cookies.domain.empty())
                {
                    res += "; Domain=" + cookies.domain;
                }
                if (!cookies.path.empty())
                {
                    res += "; Path=" + cookies.path;
                }
                if (!cookies.expires.empty())
                {
                    res += "; Expires=" + cookies.expires;
                }
                if (!cookies.is_secure)
                {
                    res += "; HttpOnly";
                }
                return res;
            }
        public:
            static SPHnwHttpHead generate()
            {
                return std::make_shared<HeadImpl>();
            }
		private:
            //报文头
            std::unordered_multimap<std::string, std::string > head_;
           
		};
	}
}
#endif