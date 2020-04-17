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

    //http 客户端
    HTTP_CLIENT,

    //http 服务端
    HTTP_SERVER
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

#define PTR_CAST(x,p) std::reinterpret_pointer_cast<x>(p)
//转换为 char
#define EVENT_RECV(buff) std::reinterpret_pointer_cast<char>(buff)

//accept 事件数据
#define EVENT_ACCEPT(buff) std::reinterpret_pointer_cast<HNW_HANDLE>(buff)


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

/*
    基础事件类
*/
struct HnwBaseEvent
{
    HNW_BASE_EVENT_TYPE type;
    HnwBaseEvent(HNW_BASE_EVENT_TYPE t) :type(t)
    {

    }
};

//HNW_BASE_ERROR 错误数据
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

/*日志级别*/
enum BLOG_LEVEL
{
    BL_DEBUG,
    BL_INFO,
    BL_WRAN,
    BL_ERROR,
};
#endif