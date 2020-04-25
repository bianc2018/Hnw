/*
    ����HnwBase ��װ��http�� ��Žӿ�
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

//���嵼������
#ifdef _WIN32
#define HNW_BASE_EXPORT_SYMBOLS extern "C" __declspec(dllexport)
#else
#define HNW_BASE_EXPORT_SYMBOLS 
#endif

#include "hnw_define.h"
#include <map>

//http ����
struct HnwHttpRequest 
{
public:
    std::string method;
    std::string url;
    std::string version;
public:
    //����ͷ
    std::unordered_multimap<std::string, std::string > head;
public:
    //������
    std::string body;

public:
    HnwHttpRequest(const std::string&m="GET",
        const std::string& u = "/",
        const std::string& v = "HTTP/1.1")
        :method(m), url(u), version(v)
    {

    }
};

//http �ظ�
struct HnwHttpResponse
{
public:
    //HTTP/1.1 200 OK
    std::string version;
    std::string status_code;
    std::string reason;
    
public:
    //����ͷ
    std::unordered_multimap<std::string, std::string > head;
public:
    //������
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


//����������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartServer(
    unsigned int local_port,//���ض˿�
    HNW_EVENT_CB cb,
    HNW_HANDLE& handle);

//�����Ự
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSession(
    const std::string& host,//��������
    HNW_EVENT_CB cb,
    HNW_HANDLE& handle);

//��������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Request(
    HNW_HANDLE handle,std::shared_ptr<HnwHttpRequest> req);


//�ظ�
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Response(
    HNW_HANDLE handle, std::shared_ptr<HnwHttpResponse> req);

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Close(HNW_HANDLE handle);


//�򵥵Ľӿ�

//�򵥵�http ������
typedef std::function<void(const std::shared_ptr<HnwHttpRequest>& req, const std::shared_ptr<HnwHttpResponse>& res)> HTTP_SERVER_CB;
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSimpleServer(unsigned int local_port,//���ض˿�
    HTTP_SERVER_CB cb);

//�򵥵Ŀͻ�������
typedef std::function<void(const std::shared_ptr<HnwHttpResponse>& req, const std::shared_ptr<HnwHttpRequest>& res)> HTTP_CLIENT_CB;
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSimpleSession(const std::string host,//���ض˿�
    HTTP_CLIENT_CB cb);

//get
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Get(const std::string &url,std::string&body);

//post
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Post(const std::string& url,const std::string& msg, std::string& response);


//���ܺ���

struct UrlParam
{
    std::string protocol;
    std::string host;
    int port = 80;
    std::string path;
    std::map<std::string,std::string> params;
};

#endif // !BNS_H_


