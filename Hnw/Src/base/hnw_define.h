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
//���û����С ���� size_t  �����ָ��ͨ����Ĭ�� ֮�󴴽���ͨ��ʹ�������С
#define SET_RECV_BUFF_SIZE 0

//���û����С ���� size_t  �����ָ��ͨ����Ĭ�� ֮�󴴽���ͨ��ʹ�������С
#define SET_SEND_BUFF_SIZE 1

//���� �������ӵ��߳��� ���� size_t server���͵�ͨ����Ч���Ҳ�����accept֮�����ã���ָ��ͨ����֮�󴴽���ͨ��ʹ�������С
#define SET_SERVER_ACCEPT_NUM 3

//����SSL ������֤��·�� ���� char[128]
#define  SET_SSL_SERVER_CERT_FILE_PATH 10
//˽����Կ private_key���� char[128]
#define  SET_SSL_SERVER_PRI_KEY_FILE_PATH 11
//������Կ
#define SET_SSL_SERVER_TEMP_DH_FILE_PATH 12


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

    //SSL �ͻ��� ����ʧ��
    HNW_BASE_SSL_CLIENT_HANDSHAKE_FAIL,
    //SSL ����˶� ����ʧ��
    HNW_BASE_SSL_SERVER_HANDSHAKE_FAIL,

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

    //http �������ͷ
    HNW_HTTP_PARSER_BAD_START_LINE,

    //http �����head
    HNW_HTTP_PARSER_BAD_HEAD_KEY,
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

    //SSL �ͻ���
    SSL_CLIENT,
    //SSL �����
    SSL_SERVER,
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
//��Ч��
#ifndef HNW_INVALID_HANDLE 
#define HNW_INVALID_HANDLE std::int64_t(-1)
#endif

//#define PTR_CAST(x,p) std::reinterpret_pointer_cast<x>(p)
#define PTR_CAST(x,p) std::static_pointer_cast<x>(p)
//static_pointer_cast

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

/*��־����*/
enum BLOG_LEVEL
{
    BL_DEBUG,
    BL_INFO,
    BL_WRAN,
    BL_ERROR,
};

///*
//    �����¼���
//*/
//struct HnwBaseEvent
//{
//    HNW_BASE_EVENT_TYPE type;
//    HnwBaseEvent(HNW_BASE_EVENT_TYPE t) :type(t)
//    {
//
//    }
//};

//HNW_BASE_ERROR ��������
struct HnwBaseErrorEvent
{
    int code;
    std::string message;
    HnwBaseErrorEvent() :code((int)HNW_BASE_ERR_CODE::HNW_BASE_OK)
    {}
    HnwBaseErrorEvent(int c, const std::string& msg = "")
        :code(c), message(msg)
    {}
};
//HNW_BASE_ACCEPT
struct HnwBaseAcceptEvent
{
    HNW_HANDLE client;
    HnwBaseAcceptEvent() :client(-1)
    {}
    HnwBaseAcceptEvent(HNW_HANDLE cli) :client(cli)
    {}
};

//HNW_BASE_RECV_DATA
struct HnwBaseRecvDataEvent 
{
    char* buff;
    size_t buff_len;
    HnwBaseRecvDataEvent() :buff(nullptr), buff_len(0)
    {}
    HnwBaseRecvDataEvent(char* b, size_t l) :buff(b), buff_len(l)
    {}
};

typedef HnwBaseRecvDataEvent HnwBaseSendDataEvent;

//�¼��ص�
typedef std::function<void(std::int64_t handle, \
    int type, std::shared_ptr<void> event_data)> HNW_EVENT_CB;

//��־�ص�
typedef std::function<void(BLOG_LEVEL lv, const std::string& log_message)> HNW_LOG_CB;

//�ڴ�����ص�
typedef std::function<std::shared_ptr<char>(size_t memory_size)> HNW_MAKE_SHARED_CB;

//���ݶ�ȡ�ص�
typedef std::function<size_t(std::shared_ptr<void> buff, size_t buff_size)> HNW_SEND_CB;

template<typename Ty, typename ...Args>
static std::shared_ptr<Ty> make_shared_safe(Args&&...args)
{
    try
    {
        return std::make_shared<Ty>(std::forward<Args>(args)...);
    }
    catch (const std::exception& e)
    {
        printf("default_make_shared exception what=%s\n", e.what());
        return nullptr;
    }
}
#endif