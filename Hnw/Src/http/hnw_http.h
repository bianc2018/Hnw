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
#include "hnw_define.h"
//定义导出符号
#ifdef _WIN32
#define HNW_HTTP_EXPORT_SYMBOLS extern "C" __declspec(dllexport)
#else
#define HNW_HTTP_EXPORT_SYMBOLS 
#endif

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

//端点类型
enum PeerType
{
    Server,
    Client
};
//启动参数
struct HttpParam
{
    //服务主机
    std::string host;

    //类型
    PeerType peer_type;

    //ssl
    //证书地址
    std::string cert_file;
    //私钥文件地址
    std::string pri_key_file;
};

//启动服务器
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Start(
    const HttpParam&param,//本地端口
    HNW_EVENT_CB cb,
    HNW_HANDLE& handle);

////启动服务器
//HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartServer(
//    const std::string& host,//本地端口
//    HNW_EVENT_CB cb,
//    HNW_HANDLE& handle);
//
////设置服务器参数
//HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_SetSSLServerParam(HNW_HANDLE handle,
//    const std::string& cert_file, const std::string& pri_key_file);
//
////启动会话
//HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSession(
//    const std::string& host,//服务主机
//    HNW_EVENT_CB cb,
//    HNW_HANDLE& handle);

//发送请求
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Request(
    HNW_HANDLE handle,std::shared_ptr<HnwHttpRequest> req);


//回复
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Response(
    HNW_HANDLE handle, std::shared_ptr<HnwHttpResponse> req);

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Close(HNW_HANDLE handle);


#endif // !BNS_H_


