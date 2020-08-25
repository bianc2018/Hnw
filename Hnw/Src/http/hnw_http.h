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

//ֻ�Ǵ�
#define HTTP_FILE_FLAG_OPEN 0x00
//�½� �����ڴ���
#define HTTP_FILE_FLAG_CREATE 0x01
//ɾ�� ������ʱ��ɾ��
#define HTTP_FILE_FLAG_DELETE 0x04
//�Ѵ��ڵĴ��� �µ�
#define HTTP_FILE_FLAG_CREATE_NEW 0x08
//д��
#define HTTP_FILE_FLAG_WRITE 0x10
#include <map>

enum HTTP_BODY_TYPE
{
    //ԭʼ����
    BODY_RAW,
    //�ļ�
    BODY_FILE,
    //�Զ���
    BODY_DEFINE,
    //��
    BODY_EMPTY,
};

//��Χ[start,end] -1��ʶ�յ���߿�ʼ
struct HttpRange
{
    //�Ƿ�ʹ�ñ߾�
    bool use_start_endpoint = true;
    bool use_end_endpoint = true;

    std::uint64_t start=0;
    std::uint64_t end=0;

    //�ܳ�
    std::uint64_t total = 0;

public:
    //�����С
    std::uint64_t section_size()const
    {
        if (total == 0 && use_end_endpoint)
            return 0;
        return (use_end_endpoint? total-1:end) - (use_start_endpoint ? 0 : start)+1;
    }
    std::uint64_t section_start()const
    {
        if (use_start_endpoint)
            return 0;
        return start;
    }
    std::uint64_t section_end()const
    {
        if (use_end_endpoint)
            return total-1;
        return end;
    }
    bool operator==(HttpRange& r)const
    {
        return (use_start_endpoint == r.use_start_endpoint) && \
            (use_end_endpoint == r.use_end_endpoint) && \
            (start == r.start) && \
            (end == r.end) && \
            (total == r.total);
    }
};

//����start_line
class HnwHttpRequestLine
{
public:
    //start line
    virtual std::string method() = 0;
    virtual void method(const std::string& m) = 0;
    virtual std::string url() = 0;
    virtual void url(const std::string& m) = 0;
    virtual std::string version() = 0;
    virtual void version(const std::string& m) = 0;

    //ת��Ϊ�ַ���
    virtual std::string string() = 0;
    virtual bool string(const std::string&raw) = 0;

    virtual bool path(const std::string& p) = 0;
    virtual std::string path() = 0;
    virtual std::vector<std::string> path_ver() = 0;
    virtual std::string  get_query(const std::string &key, const std::string& not="") = 0;
    virtual bool  add_query(const std::string& key, const std::string & value) = 0;
private:

};
typedef std::shared_ptr<HnwHttpRequestLine> SPHnwHttpRequestLine;

//Cookie
struct HnwCookie
{
    std::string key;
    std::string value;
};
//Set-Cookie: value[; expires=date][; domain=domain][; path=path][; secure]
struct HnwSetCookie
{
    HnwCookie value;
    int max_age = -1;
    //Wdy, DD-Mon-YYYY HH:MM:SS GMT
    std::string expires="";
    std::string domain = "";
    std::string path = "";
    bool is_secure=false;
};
//��֤�ֶ�
//��֤�㷨
enum HnwHttpAuthMethod
{
    //����֤��Ϣ
    NoneAuth,
    //����
    BasicAuth,
    //����ժҪ
    DigestAuth,
};
struct HnwWWWAuthenticate
{
    HnwHttpAuthMethod method= HnwHttpAuthMethod::NoneAuth;
    std::string realm;

    std::string    qop="auth";
    std::string    nonce;
    std::string    opaque;
};
struct HnwAuthorization
{
    HnwHttpAuthMethod method= HnwHttpAuthMethod::NoneAuth;
    std::string username;
    std::string password;//DigestAuth ���������ֵ���ͻ�����Ϊ�������� 
    //DigestAuth ��Ч
    std::string realm;
    std::string    nonce;
    std::string    uri ;
    std::string   qop;
    std::string    nc;
    std::string    cnonce; 
    std::string    response;
    std::string    opaque;

    //http ����ķ��� ��Ҫ�û�����
    //��Ȩ����
    //std::string authorization_data;
};
//
//head �ֶ�
class HnwHttpHead
{
public:
    //head
    virtual void add_head(const std::string& key, const std::string& value) = 0;
    virtual void add_head_double(const std::string& key, const double& value) = 0;
    virtual void add_head_int64(const std::string& key, const std::int64_t& value) = 0;
    virtual void add_head_uint64(const std::string& key, const std::uint64_t& value) = 0;

    virtual void del_head(const std::string& key) = 0;

    virtual std::vector<std::string> get_heads(const std::string& key) = 0;

    virtual std::string get_head(const std::string& key, const std::string & not) = 0;
    virtual double get_head_double(const std::string& key, const double& not) = 0;
    virtual std::int64_t get_head_int64(const std::string& key, const std::int64_t& not) = 0;
    virtual std::uint64_t get_head_uint64(const std::string& key, const std::uint64_t & not) = 0;
    virtual std::unordered_multimap<std::string, std::string >& get_all_head() = 0;
    virtual size_t get_head_count(const std::string& key) = 0;
    virtual size_t get_head_count() = 0;

    //
    virtual std::string string() = 0;
    //k:v\r\nk:v
    virtual bool string(const std::string& raw) = 0;
    //k:v
    virtual bool string_kv(const std::string& raw) = 0;

    //std::shared_ptr<HnwHttpHead>
    //�ϲ� bcoverage ��ͬ���ֶ��Ƿ񸲸�
    virtual bool merge(std::shared_ptr<HnwHttpHead> other,bool bcoverage=true)=0;

    //�߼��ӿ�

    //����/��ȡ ����ķ�Χ
    virtual bool get_range(HttpRange &range)=0;
    virtual bool set_range(const HttpRange& range)=0;

    //����/��ȡ �ظ��ķ�Χ 
    virtual bool get_content_range(HttpRange& range)=0;
    virtual bool set_content_range(const HttpRange& range)=0;

    //����/��ȡ content-type
    virtual bool content_type(const std::string&value) = 0;
    virtual bool set_content_type_by_ext(const std::string& ext) = 0;
    virtual bool set_content_type_by_uri(const std::string& uri) = 0;
    virtual std::string content_type() = 0;
    //����cookie
    //�����
    virtual bool set_setcookie(const HnwSetCookie& cookie)=0;
    virtual std::vector<HnwSetCookie> get_setcookies() = 0;
    virtual bool get_setcookie(const std::string &name, HnwSetCookie&out) = 0;
    virtual bool del_setcookie(const std::string& name) = 0;
    //�ͻ���
    virtual bool set_cookie(const HnwCookie& cookie) = 0;
    virtual std::vector<HnwCookie> get_cookies() = 0;
    virtual bool get_cookie(const std::string& name, HnwCookie& out) = 0;
    virtual bool del_cookie(const std::string& name) = 0;
    
    //true ���ֳ����� false ������
    virtual bool keep_alive() = 0;
    //
    virtual void keep_alive(bool is_alive) = 0;

    //��֤
    virtual HnwWWWAuthenticate get_www_authenticate()=0;
    virtual bool set_www_authenticate(const HnwWWWAuthenticate&auth) = 0;

    virtual HnwAuthorization get_authorization() = 0;
    virtual bool set_authorization(const HnwAuthorization& auth, const std::string& http_method = "GET") = 0;

    //�����֤��Ϣ
    virtual bool check_auth(const std::string & password, const std::string& http_method = "GET") = 0;

    //�����֤��Ϣ
    virtual bool check_auth(const HnwAuthorization& auth, const std::string& password, const std::string& http_method = "GET") = 0;
private:

};
typedef std::shared_ptr<HnwHttpHead> SPHnwHttpHead;

class HnwHttpBody
{
public:
    //body
     //������ݵ���ʽ
    //virtual void body_type(HTTP_BODY_TYPE type) = 0;
    virtual HTTP_BODY_TYPE body_type() = 0;

    virtual void read_cb(HNW_READ_CB cb) = 0;
    virtual HNW_READ_CB read_cb() = 0;
    virtual void write_cb(HNW_WRITE_CB cb) = 0;
    virtual HNW_WRITE_CB write_cb() = 0;

    virtual void read_pos(std::uint64_t pos) = 0;
    virtual std::uint64_t read_pos() = 0;
    virtual void write_pos(std::uint64_t pos) = 0;
    virtual std::uint64_t write_pos() = 0;

    virtual std::uint64_t body_size() = 0;

    virtual size_t read_body(char* buff, size_t buff_size) = 0;
    virtual size_t write_body(const char* buff, size_t buff_size) = 0;

    //�Ƿ��������ı���
    virtual bool is_complete() = 0;
private:

};
typedef std::shared_ptr<HnwHttpBody> SPHnwHttpBody;

//http ����
class HnwHttpRequest 
{
    //�����ӿ�
public:
    SPHnwHttpRequestLine line;
    SPHnwHttpHead head;
    //���ݿ�
    SPHnwHttpBody body;

public:
    //�Զ�����body
    virtual bool auto_create_body()=0;

};
typedef std::shared_ptr<HnwHttpRequest> SPHnwHttpRequest;

class HnwHttpResponseLine
{
public:
    //start line
    virtual std::string version() = 0;
    virtual void version(const std::string& m) = 0;
    virtual std::string status_code() = 0;
    virtual void status_code(const std::string& m) = 0;
    virtual std::string reason() = 0;
    virtual void reason(const std::string& m) = 0;
   
    //ת��Ϊ�ַ���
    virtual std::string string() = 0;
    virtual bool string(const std::string& raw) = 0;
private:

};
typedef std::shared_ptr<HnwHttpResponseLine> SPHnwHttpResponseLine;

//http �ظ�
struct HnwHttpResponse
{
    //�����ӿ�
public:
    SPHnwHttpResponseLine line;
    SPHnwHttpHead head;
    //���ݿ�
    SPHnwHttpBody body;

public:
    //�Զ�����body
    virtual bool auto_create_body()=0;
    //set response file
    virtual bool set_file_body(const std::string& path, \
        const HttpRange& req_range = HttpRange(),
        int flag = HTTP_FILE_FLAG_OPEN) = 0;
};
typedef std::shared_ptr<HnwHttpResponse> SPHnwHttpResponse;

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

//��־�ص�
typedef std::function<void(BLOG_LEVEL lv, const std::string& log_message)> HNW_HTTP_LOG_CB;
//������־�ص�
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_SetLogCB(HNW_HTTP_LOG_CB cb);

//����������
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Start(
    const HttpParam&param,//���ض˿�
    HNW_EVENT_CB cb,
    HNW_HANDLE& handle);

//��������
HNW_HTTP_EXPORT_SYMBOLS  HNW_BASE_ERR_CODE HnwHttp_GenerateRequest(HNW_HANDLE handle, SPHnwHttpRequest&req);

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE  HnwHttp_GenerateResponse(HNW_HANDLE handle, SPHnwHttpResponse&res);

//��������ı���ͷ
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE  HnwHttp_GenerateHead(HNW_HANDLE handle, SPHnwHttpHead& head);

//��������
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Request(
    HNW_HANDLE handle,SPHnwHttpRequest req);

//����ص�
typedef std::function<void(SPHnwHttpResponse response, HNW_BASE_ERR_CODE err)> \
HNW_ON_RESPONSE;

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_RequestGet(
    const std::string& url, HNW_ON_RESPONSE cb, 
    size_t time_out_ms = 10000, SPHnwHttpHead ext_head = nullptr);

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_RequestPost(
    const std::string& url,const std::string &body, HNW_ON_RESPONSE cb,
    size_t time_out_ms = 10000, SPHnwHttpHead ext_head = nullptr);
//�ظ�
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Response(
    HNW_HANDLE handle, SPHnwHttpResponse req);

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Close(HNW_HANDLE handle);

//������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Config(HNW_HANDLE handle, int config_type, void* data, size_t data_len);


//�ϲ�ӿ�
//��������
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_RawRequest(
    HNW_HANDLE handle, 
    const std::string &method,//����
    const std::string& uri,//·��
    const std::string& data,//����
    SPHnwHttpHead ext_head = nullptr/*��չ�ֶ�*/);

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_RawResponse(
    HNW_HANDLE handle,
    const std::string& code,//״̬��
    const std::string& data,//����
    SPHnwHttpHead ext_head = nullptr);

//���ؿջظ�  ״̬�ظ�
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StatusResponse(
    HNW_HANDLE handle,
    const std::string& code//״̬��
    );
//�����ļ�
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_FileRequest(
    HNW_HANDLE handle,
    const std::string& method,//����
    const std::string& uri,//·��
    const std::string& filepath,//����
    SPHnwHttpHead ext_head = nullptr/*��չ�ֶ�*/);

//���ļ���Ϊ�ظ� �ļ������ڷ��ش���
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_FileResponse(
    HNW_HANDLE handle,
    const std::string& code,//״̬��
    const std::string& filepath,//����
    SPHnwHttpHead ext_head = nullptr);


//��������Ϊ�ļ� ����HNW_HTTP_RECV_REQUEST_HEAD��ʱ��ʹ��D
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_RecvFileRequest(
    SPHnwHttpRequest request,
    const std::string& filepath,
    int flag = HTTP_FILE_FLAG_CREATE);

//��������Ϊ�ļ� ����HNW_HTTP_RECV_RESPONSE_HEAD��ʱ��ʹ��D
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_RecvFileResponse(
    SPHnwHttpResponse response,
    const std::string& filepath,
    int flag = HTTP_FILE_FLAG_CREATE);

//��ȡ��Դ״̬  resource 
//
struct HttpResourceStatus
{
    std::string host = "";

    std::string path="";
    
    //�ض�����Ч
    std::string location="";

    std::uint64_t size=0;

    //�ֶ�����֧��
    bool partial_support=false;

    //
    std::string status = "200";
};

typedef std::function<void(HttpResourceStatus status,
    HNW_BASE_ERR_CODE error)> HTTP_RES_STATUS_CB;

//����ȫ��
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_GetResourceStatus(const std::string& url,
    HTTP_RES_STATUS_CB cb);

//�ļ����ؽӿ� ���� false ��ֹ con_num >=1 ������������ 
typedef std::function<bool(const std::string& url,
    const std::string& output,
    const HttpRange& range,/*�������صķֶ�*/
    std::uint64_t pos,
    HNW_BASE_ERR_CODE status)> HTTP_DOWNLOAD_CB;

//����ȫ��
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_DownLoad(const std::string &url,
    const std::string& output, 
    HTTP_DOWNLOAD_CB cb,
    size_t con_num=1,
    const HttpRange range= HttpRange()
    ,int file_flag= HTTP_FILE_FLAG_CREATE_NEW);



//������
//�ļ��Ƿ���ڣ�
HNW_HTTP_EXPORT_SYMBOLS bool HnwUtil_FileIsExist(const std::string& filepath);
//�ļ�ɾ��
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwUtil_RemoveFile(const std::string& filepath);
//�������ļ�
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwUtil_CreateNullFile(const std::string& filepath,std::uint64_t size);
//����Ŀ¼
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwUtil_CreateDirs(const std::string& dir);

//uri �����
HNW_HTTP_EXPORT_SYMBOLS std::string HnwUtil_UrlEncode(const std::string& src);
HNW_HTTP_EXPORT_SYMBOLS std::string HnwUtil_UrlDecode(const std::string& src);

//utf-8��ANSIת��
HNW_HTTP_EXPORT_SYMBOLS std::string HnwUtil_Utf8ToAnsi(const std::string& src);
HNW_HTTP_EXPORT_SYMBOLS std::string HnwUtil_AnsiToUtf8(const std::string& src);

//utf-8��ANSIת��
HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwUtil_Md5(const std::string& src, std::string& dst);
#endif // !BNS_H_


