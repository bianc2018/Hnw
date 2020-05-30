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

//��־�ص�
typedef std::function<void(BLOG_LEVEL lv, const std::string& log_message)> HNW_HTTP_LOG_CB;
//������־�ص�
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_SetLogCB(HNW_HTTP_LOG_CB cb);

//�˵�����
enum PeerType
{
    Server,
    Client,
    None
};

//��������
struct HttpParam
{
    //��������
    std::string host;

    //����
    PeerType peer_type= PeerType::None;

    //ssl
    //֤���ַ
    std::string cert_file;
    //˽Կ�ļ���ַ
    std::string pri_key_file;
    //�����ļ���ַ
    std::string temp_dh_file;

    //�����߳�
    size_t accept_num = 0;
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

//������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Config(HNW_HANDLE handle, int config_type, void* data, size_t data_len);

//�����յ����� ,ʧ�ܷ��� NULL

#endif // !BNS_H_


