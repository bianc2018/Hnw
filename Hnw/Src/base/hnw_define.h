/*
    基于hnw基础数据定义
    hql 2020/01/04
*/
#ifndef HNW_DEFINE_H_
#define HNW_DEFINE_H_
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <stdint.h>
//定义导出符号
#ifdef _WIN32
#define HNW_BASE_EXPORT_SYMBOLS extern "C" __declspec(dllexport)
#else
#define HNW_BASE_EXPORT_SYMBOLS 
#endif

#define USE_BOOST_ASIO

//配置项
//设置缓存大小 输入 size_t  如果不指定通道则默认 之后创建的通道使用这个大小
#define SET_RECV_BUFF_SIZE 0

//设置缓存大小 输入 size_t  如果不指定通道则默认 之后创建的通道使用这个大小
#define SET_SEND_BUFF_SIZE 1

//设置 接受链接的线程数 输入 size_t server类型的通道有效而且不可在accept之后设置，不指定通道则之后创建的通道使用这个大小
#define SET_SERVER_ACCEPT_NUM 3

//设置SSL 服务器证书路径 输入 char[128]
#define  SET_SSL_SERVER_CERT_FILE_PATH 10
//私有密钥 private_key输入 char[128]
#define  SET_SSL_SERVER_PRI_KEY_FILE_PATH 11
//交换密钥
#define SET_SSL_SERVER_TEMP_DH_FILE_PATH 12


//错误码
enum class HNW_BASE_ERR_CODE
{
    //无错误
    HNW_BASE_OK,

    //内存申请失败
    HNW_BASE_ALLOC_FAIL = -998,

    //通道类型不对
    HNW_BASE_CHANNEL_TYPE_FAIL,

    //无效的句柄
    HNW_BASE_INVAILD_HANDLE,

    //重复的句柄
    HNW_BASE_HANDLE_IS_USED,

    //创建套接字失败
    HNW_BASE_CREATE_SOCKET_FAIL,

    //链接失败
    HNW_BASE_CONNECT_FAIL,

    //接收数据失败
    HNW_BASE_RECV_DATA_FAIL,

    //发送数据失败
    HNW_BASE_SEND_DATA_FAIL,

    //空的事件回调
    HNW_BASE_EMPYTY_EVENT_CB,

    //空的日志回调
    HNW_BASE_EMPYTY_LOG_CB,

    //空的内存申请回调
    HNW_BASE_EMPYTY_MAKE_SHARED_CB,

    //空的接收函数
    HNW_BASE_EMPYTY_ACCEPT_CB,

    //空的BUFF
    HNW_BASE_EMPYTY_BUFF,

    //打开失败
    HNW_BASE_SOCKET_OPEN_FAIL,

    //重用地址失败
    HNW_BASE_SET_REUSE_ADDRESS_FAIL,

    //绑定地址失败
    HNW_BASE_BIND_ADDRESS_FAIL,

    //监听失败
    HNW_BASE_LISTEN_FAIL,

    //接收链接失败
    HNW_BASE_ACCEPT_FAIL,

    //接收链接是无效的
    HNW_BASE_ACCEPT_CLI_IS_NOT_OPEN,

    //重复初始化
    HNW_BASE_REPEATED_INIT,

    //重复释放
    HNW_BASE_REPEATED_RELEASE,

    //错误的地址节点
    HNW_BASE_ADDRESS_ENDPOINT_ERROR,

    //空的套接字
    HNW_BASE_EMPTY_SOCKET,

    //未受支持的操作
    HNW_BASE_NO_SUPPORT,

    //通道未链接
    HNW_BASE_CHANNEL_NOT_CONN,

    //通道创建失败
    HNW_BASE_CHANNEL_CREATE_FAIL,

    //通道初始化失败
    HNW_BASE_CHANNEL_INIT_FAIL,

    //无效的参数输入
    HNW_BASE_PARAMS_IS_INVALID,

    //服务已停止
    HNW_BASE_SERVICE_IS_STOPED,

    //SSL 客户端 握手失败
    HNW_BASE_SSL_CLIENT_HANDSHAKE_FAIL,
    //SSL 服务端端 握手失败
    HNW_BASE_SSL_SERVER_HANDSHAKE_FAIL,

    //http 解析失败
    HNW_HTTP_PARSER_FAIL,

    //空的解析器
    HNW_HTTP_EMPTY_PARSER,

    //http 主机是无效的
    HNW_HTTP_HOST_IS_IVAILD,

    //http 错误的报文
    HNW_HTTP_BAD_MESSAGE,

    //http 键值重复
    HNW_HTTP_PARSER_KEY_IS_NO_EMPTY,

    //http 错误的行头
    HNW_HTTP_PARSER_BAD_START_LINE,

    //http 错误的head
    HNW_HTTP_PARSER_BAD_HEAD_KEY,
    //http 键值空
    HNW_HTTP_PARSER_KEY_IS_EMPTY,

    //http 解析回调为空
    HNW_HTTP_PARSER_CB_IS_EMPTY,
    
    //http 解析回调不可以是默认的
    HNW_HTTP_PARSER_CB_IS_DEFAULT,

    //http 报文组装失败
    HNW_HTTP_STRUCT_MESSAGE_FAIL,

    //未知错误
    HNW_BASE_NUKNOW_ERROR = -9999,

};
//通道类型
enum class HNW_CHANNEL_TYPE
{
    //未定义
    HNW_INVAILD,

    //tcp 服务端
    TCP_SERVER,

    //tcp 客户端
    TCP_CLIENT,

    //udp
    UDP,

    //SSL 客户端
    SSL_CLIENT,
    //SSL 服务端
    SSL_SERVER,
};

//节点
struct NetPoint
{
    std::string ip = "";
    int port = -1;
};

#ifndef NET_INVALID_POINT 
#define NET_INVALID_POINT  NetPoint()
#endif

//链接句柄
typedef std::int64_t HNW_HANDLE;
//无效的
#ifndef HNW_INVALID_HANDLE 
#define HNW_INVALID_HANDLE std::int64_t(-1)
#endif

//#define PTR_CAST(x,p) std::reinterpret_pointer_cast<x>(p)
#define PTR_CAST(x,p) std::static_pointer_cast<x>(p)
//static_pointer_cast

//网络事件回调类型
enum class HNW_BASE_EVENT_TYPE
{
    //错误
    HNW_BASE_ERROR = -1,

    //链接确立 establish 
    HNW_BASE_CONNECT_ESTABLISH = 1,

    //接收到链接
    HNW_BASE_ACCEPT,

    //接收到数据
    HNW_BASE_RECV_DATA,

    //发送数据完整 complete
    HNW_BASE_SEND_COMPLETE,

    //接收到 http 请求
    HNW_HTTP_RECV_REQUEST,

    //接收到 http 回复
    HNW_HTTP_RECV_RESPONSE,

    //链接已经关闭
    HNW_BASE_CLOSED,
};

/*日志级别*/
enum BLOG_LEVEL
{
    BL_DEBUG,
    BL_INFO,
    BL_WRAN,
    BL_ERROR,
};

///*
//    基础事件类
//*/
//struct HnwBaseEvent
//{
//    HNW_BASE_EVENT_TYPE type;
//    HnwBaseEvent(HNW_BASE_EVENT_TYPE t) :type(t)
//    {
//
//    }
//};

//HNW_BASE_ERROR 错误数据
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

//事件回调
typedef std::function<void(std::int64_t handle, \
    int type, std::shared_ptr<void> event_data)> HNW_EVENT_CB;

//日志回调
typedef std::function<void(BLOG_LEVEL lv, const std::string& log_message)> HNW_LOG_CB;

//内存申请回调
typedef std::function<std::shared_ptr<char>(size_t memory_size)> HNW_MAKE_SHARED_CB;

//数据读取回调
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