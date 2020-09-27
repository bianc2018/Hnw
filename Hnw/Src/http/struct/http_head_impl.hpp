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
            virtual std::string get_head(const std::string& key, const std::string & notfind)
            {
                //auto p = head_.find(util::to_lower(key));
                auto p = head_.find(key);
                if (head_.end() == p)
                    return notfind;
                return p->second;
            }
            virtual double get_head_double(const std::string& key, const double& notfind)
            {
                auto str = get_head(key, "");
                if (str.empty())
                    return notfind;
                try
                {
                    return std::stod(str);
                }
                catch (const std::exception&)
                {
                    return notfind;
                }
            }
            virtual std::int64_t get_head_int64(const std::string& key, const std::int64_t & notfind)
            {
                auto str = get_head(key, "");
                if (str.empty())
                    return notfind;
                try
                {
                    return std::stoll(str);
                }
                catch (const std::exception&)
                {
                    return notfind;
                }
            }
            virtual std::uint64_t get_head_uint64(const std::string& key, const std::uint64_t & notfind)
            {
                auto str = get_head(key, "");
                if (str.empty())
                    return notfind;
                try
                {
                    return std::stoull(str);
                }
                catch (const std::exception&)
                {
                    return notfind;
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
                    PRINTFLOG(BL_ERROR, "http head %s notfind found ", util::HTTP_RANGE.c_str());
                    return false;
                }
                //Range: bytes=0-1551;1-3
                //分割=
                auto bv = util::split(src, "=");
                if (bv.size() != 2||(bv[0].find(util::HTTP_RANGE_BYTE)== std::string::npos))
                {
                    PRINTFLOG(BL_ERROR, "http head %s notfind support like %s ",
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
                        PRINTFLOG(BL_ERROR, "http head %s notfind support like %s ",
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
                    PRINTFLOG(BL_ERROR, "http head %s notfind found ",
                        util::HTTP_CONTENT_RANGE.c_str());
                    return false;
                }

                //Content-Range: bytes 0-10/3103
                //分割=
                auto bv = util::split(src, " ");
                if (bv.size() != 2 || (bv[0].find(util::HTTP_RANGE_BYTE) == std::string::npos))
                {
                    PRINTFLOG(BL_ERROR, "http head %s notfind support like %s ",
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
                        PRINTFLOG(BL_ERROR, "http head %s notfind support like %s ",
                            util::HTTP_CONTENT_RANGE.c_str(), src.c_str());
                        return false;
                    }
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "http head %s notfind support like %s ",
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

            //验证
            virtual HnwWWWAuthenticate get_www_authenticate() override
            {
                HnwWWWAuthenticate auth;
                auto str = get_head(util::HTTP_WWW_AUTH, "");
                if (str.empty())
                {
                    PRINTFLOG(BL_ERROR, "notfind find key=%s", util::HTTP_WWW_AUTH.c_str());
                    return HnwWWWAuthenticate();
                }

                //空格分割
                auto p = str.find_first_of(" ");
                if (std::string::npos == p||p+1 >=str.size())
                {
                    PRINTFLOG(BL_ERROR, "get_www_authenticate error %s", str.c_str());
                    return HnwWWWAuthenticate();
                }
                auto auth_method = str.substr(0,p);
                auto data = str.substr(p + 1);
                auto map = split_ky(data);
                //数字验证
                if ("Digest" == auth_method)
                {
                    auth.method = DigestAuth;
                    GET_VALUE_FOR_MAP(map, "realm", auth.realm, "");
                    GET_VALUE_FOR_MAP(map, "nonce", auth.nonce, "");
                    GET_VALUE_FOR_MAP(map, "opaque", auth.opaque, "");
                    GET_VALUE_FOR_MAP(map, "qop", auth.qop, "");
                    return auth;
                }
                else if("Basic" == auth_method)
                {
                    auth.method = BasicAuth;
                    GET_VALUE_FOR_MAP(map, "realm", auth.realm, "");
                    return auth;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "%s error,unsupport auth type %s", str.c_str(), auth_method);
                    return HnwWWWAuthenticate();
                }

            }
            virtual bool set_www_authenticate(const HnwWWWAuthenticate& auth)override
            {
                if (NoneAuth == auth.method)
                {
                    //
                    PRINTFLOG(BL_ERROR, "notfind support auth method NoneAuth");
                    return false;
                }
                std::string data;
                if (BasicAuth == auth.method)
                {
                    data = "Basic realm=\"" + auth.realm+ "\"";
                }
                else if (DigestAuth == auth.method)
                {
                    /*
                    * Digest realm="testrealm@host.com",
                        qop="auth,auth-int",
                        nonce="dcd98b7102dd2f0e8b11d0f600bfb0c093",
                        opaque="5ccc069c403ebaf9f0171e9517f40e41"
                    */
                    data = "Digest realm=\"" + auth.realm + "\","\
                        "qop=\"" + auth.qop + "\","
                        "nonce=\"" + auth.nonce + "\","
                        "opaque=\"" + auth.opaque + "\"";
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "notfind support auth method %d",auth.method);
                    return false;
                }
                del_head(util::HTTP_WWW_AUTH);
                add_head(util::HTTP_WWW_AUTH, data);
                return true;
            }

            virtual HnwAuthorization get_authorization()
            {
                HnwAuthorization auth;
                auto str = get_head(util::HTTP_AUTH, "");
                if (str.empty())
                {
                    PRINTFLOG(BL_ERROR, "notfind find key=%s", util::HTTP_AUTH.c_str());
                    return HnwAuthorization();
                }

                //空格分割
                auto p = str.find_first_of(" ");
                if (std::string::npos == p || p + 1 >= str.size())
                {
                    PRINTFLOG(BL_ERROR, "get_authorization error %s", str.c_str());
                    return HnwAuthorization();
                }
                auto auth_method = str.substr(0, p);
                auto data = str.substr(p + 1);
                
                //数字验证
                if ("Digest" == auth_method)
                {
                    auto map = split_ky(data);
                    auth.method = DigestAuth;
                    GET_VALUE_FOR_MAP(map, "realm", auth.realm, "");
                    GET_VALUE_FOR_MAP(map, "nonce", auth.nonce, "");
                    GET_VALUE_FOR_MAP(map, "opaque", auth.opaque, "");
                    GET_VALUE_FOR_MAP(map, "qop", auth.qop, "");
                    GET_VALUE_FOR_MAP(map, "cnonce", auth.cnonce, "");
                    GET_VALUE_FOR_MAP(map, "nc", auth.nc, "");
                    GET_VALUE_FOR_MAP(map, "response", auth.response, "");
                    GET_VALUE_FOR_MAP(map, "uri", auth.uri, "");
                    GET_VALUE_FOR_MAP(map, "username", auth.username, "");
                    return auth;
                }
                else if ("Basic" == auth_method)
                {
                    auth.method = BasicAuth;
                    auto decode = util::decode_base64(data);
                    auto u_p = util::split(decode, ":");
                    if (u_p.size() < 2)
                    {
                        PRINTFLOG(BL_ERROR, "%s de_base64-> %s,notfind found username and password"\
                            , data.c_str(), decode.c_str());
                        return HnwAuthorization();
                    }
                    auth.username = u_p[0];
                    auth.password = u_p[1];
                    return auth;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "%s error,unsupport auth type %s", str.c_str(), auth_method);
                    return HnwAuthorization();
                }
            }
            virtual bool set_authorization(const HnwAuthorization& auth, \
                const std::string& http_method = "GET")override
            {
                if (NoneAuth == auth.method)
                {
                    //
                    PRINTFLOG(BL_ERROR, "notfind support auth method NoneAuth");
                    return false;
                }
                std::string data;
                if (BasicAuth == auth.method)
                {
                    data = "Basic "+get_auth_info(auth, http_method);
                }
                else if (DigestAuth == auth.method)
                {
                    /*
                    * Digest username="Mufasa",
                     realm="testrealm@host.com",
                     nonce="dcd98b7102dd2f0e8b11d0f600bfb0c093",
                     uri="/dir/index.html",
                     qop=auth,
                     nc=00000001,
                     cnonce="0a4f113b",
                     response="6629fae49393a05397450978507c4ef1",
                     opaque="5ccc069c403ebaf9f0171e9517f40e41"
                    */
                    data = "Digest username=\"" + auth.username + "\","\
                        "realm=\"" + auth.realm + "\","
                        "nonce=\"" + auth.nonce + "\","
                        "uri=\"" + auth.uri + "\","
                        "qop=" + auth.qop + ","
                        "nc=" + auth.nc + ","
                        "cnonce=\"" + auth.cnonce + "\","
                        "response=\"" + get_auth_info(auth, http_method) + "\","
                        "opaque=\"" + auth.opaque + "\",";
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "notfind support auth method %d", auth.method);
                    return false;
                }
                del_head(util::HTTP_AUTH);
                add_head(util::HTTP_AUTH, data);
                return true;
            }

            //检查验证信息
            virtual bool check_auth(const std::string& password,
                const std::string& http_method = "GET") override
            {
                return check_auth(get_authorization(), password, http_method);
            }

            //检查验证信息
            virtual bool check_auth(const HnwAuthorization& auth,
                const std::string& password, const std::string& http_method = "GET")override
            {
                if (BasicAuth == auth.method)
                {
                    // \0对比会出错
                    //return (auth.password == password);
                    return (0 == strcmp(auth.password.c_str(),password.c_str()));
                }
                else if (DigestAuth == auth.method)
                {
                    HnwAuthorization a = auth;
                    a.password = password;
                    return (get_auth_info(a, http_method) == a.response);
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "notfind support auth method %d", auth.method);
                    return false;
                }
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

            //split k&v 分割 a=1,b=2的内容，忽略 “” ''
            virtual std::map<std::string, std::string> split_ky(const std::string& src,
                const char& link = ',')
            {
                std::map<std::string, std::string> k_vs;
                std::string key, value;
                enum SplitKeyValueStatus
                {
                    Status_KEY,
                    Status_VALUE,
                    Status_STR,
                };
                SplitKeyValueStatus s = Status_KEY;
                for (auto c : src)
                {
                    if (Status_KEY == s)
                    {
                        if (c == '=')
                        {
                            s = Status_VALUE;
                            continue;
                        }
                        else
                        {
                            key += c;
                            continue;
                        }

                    }
                    else if (Status_VALUE == s)
                    {
                        if (c == '"' || c == '\'')
                        {
                            s = Status_STR;
                            continue;
                        }
                        else if (c == link)
                        {
                            
                            //找到
                            k_vs[util::trim(key)] = util::trim(value);
                            key = value = "";
                            s = Status_KEY;
                            continue;
                        }
                        else
                        {
                            value += c;
                            continue;
                        }
                    }
                    else if (Status_STR == s)
                    {
                        if (c == '"' || c == '\'')
                        {
                            s = Status_VALUE;
                            continue;
                        }
                        else
                        {
                            value += c;
                            continue;
                        }
                    }
                }
                if (!key.empty())
                {
                    //找到
                    k_vs[util::trim(key)] = util::trim(value);
                }
                return k_vs;
            }

            //获取验证信息
            virtual std::string get_auth_info(const HnwAuthorization& auth,\
                const std::string& http_method="GET")
            {
                if (auth.method == BasicAuth)
                {
                    return util::encode_base64(auth.username + ":" + auth.password);
                }
                if (auth.method == DigestAuth)
                {
                    /*
                    * 如下所述，response 值由三步计算而成。当多个数值合并的时候，使用冒号作为分割符。
                        1.对用户名、认证域(realm)以及密码的合并值计算 MD5 哈希值，结果称为 HA1。
                        2.对HTTP方法以及URI的摘要的合并值计算 MD5 哈希值，例如，GET 和 /dir/index.html，结果称为 HA2。
                        3.对 HA1、服务器密码随机数(nonce)、请求计数(nc)、客户端密码随机数(cnonce)、
                        \保护质量(qop)以及 HA2 的合并值计算 MD5 哈希值。结果即为客户端提供的 response 值。
                        因为服务器拥有与客户端同样的信息，因此服务器可以进行同样的计算，以验证客户端提交的 response 值的正确性。
                        \在上面给出的例子中，结果是如下计算的。 \
                        （MD5()表示用于计算 MD5 哈希值的函数；“\”表示接下一行；引号并不参与计算）
                        完成 RFC 2617 中所给出的示例，将在每步得出如下结果。

                    */
                    auto ha1 = util::md5(auth.username + ":" + auth.realm + ":" + auth.password);
                    auto ha2 = util::md5(http_method + ":" + auth.uri);

                    //
                    return util::md5(ha1 + ":" \
                        + auth.nonce + ":" + auth.nc + ":" \
                        + auth.cnonce + ":" + auth.qop + ":" + ha2);
                }
                else
                {
                    return "";
                }
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