/*
    基于HnwBase 封装的http库 外放接口
    hql 2020/01/04
*/
#ifndef HNW_HTTP_H_
#define HNW_HTTP_H_

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdint.h>

//定义导出符号
#ifdef _WIN32
#define HNW_BASE_EXPORT_SYMBOLS extern "C" __declspec(dllexport)
#else
#define HNW_BASE_EXPORT_SYMBOLS 
#endif

#include "hnw_define.h"
#include <map>

//http 请求
struct HnwHttpRequest 
{
public:
    std::string method;
    std::string url;
    std::string version;
public:
    //报文头
    std::unordered_multimap<std::string, std::string > head;
public:
    //报文体
    std::string body;

public:
    HnwHttpRequest(const std::string&m="GET",
        const std::string& u = "/",
        const std::string& v = "HTTP/1.1")
        :method(m), url(u), version(v)
    {

    }
};

//http 回复
struct HnwHttpResponse
{
public:
    //HTTP/1.1 200 OK
    std::string version;
    std::string status_code;
    std::string reason;
    
public:
    //报文头
    std::unordered_multimap<std::string, std::string > head;
public:
    //报文体
    std::string body;

public:
    HnwHttpResponse(const std::string& v = "HTTP/1.1",
        const std::string& s = "200",
        const std::string& r = "OK"
        )
        : version(v), status_code(s), reason(r)
    {

    }
};


//启动服务器
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartServer(
    unsigned int local_port,//本地端口
    HNW_EVENT_CB cb,
    HNW_HANDLE& handle);

//启动会话
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSession(
    const std::string& host,//服务主机
    HNW_EVENT_CB cb,
    HNW_HANDLE& handle);

//发送请求
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Request(
    HNW_HANDLE handle,std::shared_ptr<HnwHttpRequest> req);


//回复
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Response(
    HNW_HANDLE handle, std::shared_ptr<HnwHttpResponse> req);

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Close(HNW_HANDLE handle);


//简单的接口

//简单的http 服务器
typedef std::function<void(const std::shared_ptr<HnwHttpRequest>& req, const std::shared_ptr<HnwHttpResponse>& res)> HTTP_SERVER_CB;
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSimpleServer(unsigned int local_port,//本地端口
    HTTP_SERVER_CB cb);

//简单的客户端请求
typedef std::function<void(const std::shared_ptr<HnwHttpResponse>& req, const std::shared_ptr<HnwHttpRequest>& res)> HTTP_CLIENT_CB;
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSimpleSession(const std::string host,//本地端口
    HTTP_CLIENT_CB cb);

//get
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Get(const std::string &url,std::string&body);

//post
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Post(const std::string& url,const std::string& msg, std::string& response);


//功能函数

struct UrlParam
{
    std::string protocol;
    std::string host;
    int port = 80;
    std::string path;
    std::map<std::string,std::string> params;
};

#endif // !BNS_H_


