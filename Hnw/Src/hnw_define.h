/*
    ����hnw�������ݶ���
    hql 2020/01/04
*/
#ifndef HNW_DEFINE_H_
#define HNW_DEFINE_H_
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <stdint.h>
//���嵼������
#ifdef _WIN32
#define HNW_BASE_EXPORT_SYMBOLS extern "C" __declspec(dllexport)
#else
#define HNW_BASE_EXPORT_SYMBOLS 
#endif

#define USE_BOOST_ASIO

//������
enum class HNW_BASE_ERR_CODE
{
    //�޴���
    HNW_BASE_OK,

    //�ڴ�����ʧ��
    HNW_BASE_ALLOC_FAIL = -998,

    //ͨ�����Ͳ���
    HNW_BASE_CHANNEL_TYPE_FAIL,

    //��Ч�ľ��
    HNW_BASE_INVAILD_HANDLE,

    //�ظ��ľ��
    HNW_BASE_HANDLE_IS_USED,

    //�����׽���ʧ��
    HNW_BASE_CREATE_SOCKET_FAIL,

    //����ʧ��
    HNW_BASE_CONNECT_FAIL,

    //��������ʧ��
    HNW_BASE_RECV_DATA_FAIL,

    //��������ʧ��
    HNW_BASE_SEND_DATA_FAIL,

    //�յ��¼��ص�
    HNW_BASE_EMPYTY_EVENT_CB,

    //�յ���־�ص�
    HNW_BASE_EMPYTY_LOG_CB,

    //�յ��ڴ�����ص�
    HNW_BASE_EMPYTY_MAKE_SHARED_CB,

    //�յĽ��պ���
    HNW_BASE_EMPYTY_ACCEPT_CB,

    //�յ�BUFF
    HNW_BASE_EMPYTY_BUFF,

    //��ʧ��
    HNW_BASE_SOCKET_OPEN_FAIL,

    //���õ�ַʧ��
    HNW_BASE_SET_REUSE_ADDRESS_FAIL,

    //�󶨵�ַʧ��
    HNW_BASE_BIND_ADDRESS_FAIL,

    //����ʧ��
    HNW_BASE_LISTEN_FAIL,

    //��������ʧ��
    HNW_BASE_ACCEPT_FAIL,

    //������������Ч��
    HNW_BASE_ACCEPT_CLI_IS_NOT_OPEN,

    //�ظ���ʼ��
    HNW_BASE_REPEATED_INIT,

    //�ظ��ͷ�
    HNW_BASE_REPEATED_RELEASE,

    //����ĵ�ַ�ڵ�
    HNW_BASE_ADDRESS_ENDPOINT_ERROR,

    //�յ��׽���
    HNW_BASE_EMPTY_SOCKET,

    //δ��֧�ֵĲ���
    HNW_BASE_NO_SUPPORT,

    //ͨ��δ����
    HNW_BASE_CHANNEL_NOT_CONN,

    //ͨ������ʧ��
    HNW_BASE_CHANNEL_CREATE_FAIL,

    //ͨ����ʼ��ʧ��
    HNW_BASE_CHANNEL_INIT_FAIL,

    //��Ч�Ĳ�������
    HNW_BASE_PARAMS_IS_INVALID,

    //������ֹͣ
    HNW_BASE_SERVICE_IS_STOPED,

    //http ����ʧ��
    HNW_HTTP_PARSER_FAIL,

    //�յĽ�����
    HNW_HTTP_EMPTY_PARSER,

    //http ��������Ч��
    HNW_HTTP_HOST_IS_IVAILD,

    //http ����ı���
    HNW_HTTP_BAD_MESSAGE,

    //http ��ֵ�ظ�
    HNW_HTTP_PARSER_KEY_IS_NO_EMPTY,

    //http ��ֵ��
    HNW_HTTP_PARSER_KEY_IS_EMPTY,

    //http �����ص�Ϊ��
    HNW_HTTP_PARSER_CB_IS_EMPTY,
    
    //http �����ص���������Ĭ�ϵ�
    HNW_HTTP_PARSER_CB_IS_DEFAULT,

    //http ������װʧ��
    HNW_HTTP_STRUCT_MESSAGE_FAIL,

    //δ֪����
    HNW_BASE_NUKNOW_ERROR = -9999,

};
//ͨ������
enum class HNW_CHANNEL_TYPE
{
    //δ����
    HNW_INVAILD,

    //tcp �����
    TCP_SERVER,

    //tcp �ͻ���
    TCP_CLIENT,

    //udp
    UDP,

    //http �ͻ���
    HTTP_CLIENT,

    //http �����
    HTTP_SERVER
};

//�ڵ�
struct NetPoint
{
    std::string ip = "";
    int port = -1;
};

#ifndef NET_INVALID_POINT 
#define NET_INVALID_POINT  NetPoint()
#endif

//���Ӿ��
typedef std::int64_t HNW_HANDLE;

#define PTR_CAST(x,p) std::reinterpret_pointer_cast<x>(p)
//ת��Ϊ char
#define EVENT_RECV(buff) std::reinterpret_pointer_cast<char>(buff)

//accept �¼�����
#define EVENT_ACCEPT(buff) std::reinterpret_pointer_cast<HNW_HANDLE>(buff)


//�����¼��ص�����
enum class HNW_BASE_EVENT_TYPE
{
    //����
    HNW_BASE_ERROR = -1,

    //����ȷ�� establish 
    HNW_BASE_CONNECT_ESTABLISH = 1,

    //���յ�����
    HNW_BASE_ACCEPT,

    //���յ�����
    HNW_BASE_RECV_DATA,

    //������������ complete
    HNW_BASE_SEND_COMPLETE,

    //���յ� http ����
    HNW_HTTP_RECV_REQUEST,

    //���յ� http �ظ�
    HNW_HTTP_RECV_RESPONSE,

    //�����Ѿ��ر�
    HNW_BASE_CLOSED,
};

/*
    �����¼���
*/
struct HnwBaseEvent
{
    HNW_BASE_EVENT_TYPE type;
    HnwBaseEvent(HNW_BASE_EVENT_TYPE t) :type(t)
    {

    }
};

//HNW_BASE_ERROR ��������
struct HnwBaseErrorEvent:public HnwBaseEvent
{
    int code;
    std::string message;
    HnwBaseErrorEvent() :code((int)HNW_BASE_ERR_CODE::HNW_BASE_OK)
        ,HnwBaseEvent(HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR)
    {}
    HnwBaseErrorEvent(int c, const std::string& msg = "")
        :code(c), message(msg)
        , HnwBaseEvent(HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR)
    {}
};
//HNW_BASE_ACCEPT
struct HnwBaseAcceptEvent :public HnwBaseEvent
{
    HNW_HANDLE client;
    HnwBaseAcceptEvent() :client(-1)
        , HnwBaseEvent(HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT)
    {}
    HnwBaseAcceptEvent(HNW_HANDLE cli) :client(cli)
        , HnwBaseEvent(HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT)
    {}
};

//HNW_BASE_RECV_DATA
struct HnwBaseRecvDataEvent :public HnwBaseEvent
{
    char* buff;
    size_t buff_len;
    HnwBaseRecvDataEvent() :buff(nullptr), buff_len(0)
        , HnwBaseEvent(HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT)
    {}
    HnwBaseRecvDataEvent(char* b, size_t l) :buff(b), buff_len(l)
        , HnwBaseEvent(HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT)
    {}
};

typedef HnwBaseRecvDataEvent HnwBaseSendDataEvent;

typedef std::function<void(std::int64_t handle, \
    int type, std::shared_ptr<void> event_data)> HNW_EVENT_CB;

/*��־����*/
enum BLOG_LEVEL
{
    BL_DEBUG,
    BL_INFO,
    BL_WRAN,
    BL_ERROR,
};
#endif