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
#include "hnw_define.h"
//���嵼������
#ifdef _WIN32
#define HNW_HTTP_EXPORT_SYMBOLS extern "C" __declspec(dllexport)
#else
#define HNW_HTTP_EXPORT_SYMBOLS 
#endif

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

//�˵�����
enum PeerType
{
    Server,
    Client
};
//��������
struct HttpParam
{
    //��������
    std::string host;

    //����
    PeerType peer_type;

    //ssl
    //֤���ַ
    std::string cert_file;
    //˽Կ�ļ���ַ
    std::string pri_key_file;
};

//����������
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Start(
    const HttpParam&param,//���ض˿�
    HNW_EVENT_CB cb,
    HNW_HANDLE& handle);

////����������
//HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartServer(
//    const std::string& host,//���ض˿�
//    HNW_EVENT_CB cb,
//    HNW_HANDLE& handle);
//
////���÷���������
//HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_SetSSLServerParam(HNW_HANDLE handle,
//    const std::string& cert_file, const std::string& pri_key_file);
//
////�����Ự
//HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSession(
//    const std::string& host,//��������
//    HNW_EVENT_CB cb,
//    HNW_HANDLE& handle);

//��������
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Request(
    HNW_HANDLE handle,std::shared_ptr<HnwHttpRequest> req);


//�ظ�
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Response(
    HNW_HANDLE handle, std::shared_ptr<HnwHttpResponse> req);

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Close(HNW_HANDLE handle);


#endif // !BNS_H_


