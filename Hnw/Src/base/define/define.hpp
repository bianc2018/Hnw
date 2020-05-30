/*
    bns 内部定义
*/
#ifndef HWN_DEFINE_H_
#define HWN_DEFINE_H_

#include "../hnw_base.h"
#include <mutex>
#include <thread>
#include <string.h>
#include <cstddef>

#define PRINTF_BUFF_SIZE 2048
static std::mutex PRINTFLOG_LOCK;
static char PRINTFLOG_MESSAGE[PRINTF_BUFF_SIZE] = { 0 };
static char PRINTFLOG_DST[PRINTF_BUFF_SIZE] = { 0 };
//日志打印
#define PRINTFLOG(l,fmt,...) do\
{\
    std::lock_guard<std::mutex> lk(PRINTFLOG_LOCK);\
    snprintf(PRINTFLOG_MESSAGE,PRINTF_BUFF_SIZE,fmt,##__VA_ARGS__);\
\
    auto  now_time = time(nullptr);\
    auto file = std::string(__FILE__);\
    auto p = file.find_last_of("/\\");\
    auto file_name = file.substr(p + 1);\
    auto time_str = ctime(&now_time);\
\
    time_str[strlen(time_str)-1]='\0';\
\
    snprintf(PRINTFLOG_DST, PRINTF_BUFF_SIZE, \
    "%s [%s][%d]:%s\n", \
    time_str, \
    file_name.c_str(), \
    __LINE__, PRINTFLOG_MESSAGE);\
\
    if (log_cb_)\
        log_cb_(l,PRINTFLOG_DST);\
}while(false)\

#define MAKE_SHARED(x) make_shared_?make_shared_(x):nullptr

//回调
#define EVENT_CB(opt,data) \
if (event_cb_)\
    event_cb_((std::int64_t)get_handle(),(int)opt,data)

#define EVENT_RECV_CB(buff,len) \
EVENT_CB(HNW_BASE_EVENT_TYPE::HNW_BASE_RECV_DATA,std::make_shared<HnwBaseRecvDataEvent>((char*)(buff.get()), len))

#define EVENT_SEND_CB(buff,len) \
EVENT_CB(HNW_BASE_EVENT_TYPE::HNW_BASE_SEND_COMPLETE,std::make_shared<HnwBaseSendDataEvent>((char*)(buff.get()), len))

#define EVENT_ERR_CB(err,msg) \
EVENT_CB(HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR, std::make_shared<HnwBaseErrorEvent>((int)err,msg))

#define EVENT_ACCEPT_CB(cli) \
EVENT_CB(HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT,std::make_shared<HnwBaseAcceptEvent>(cli))

#define EVENT_OK(opt) \
EVENT_CB(opt,nullptr)

#ifndef INVAILD_HANDLE
#define INVAILD_HANDLE HNW_HANDLE(-1)
#endif

namespace hnw
{
    //接收缓存 1M
    const size_t default_recv_buff_size = 1024 * 1024;

    //接收缓存 1M
    const size_t default_accept_num = 5;

    //默认的函数
    static std::shared_ptr<char> default_make_shared(size_t memory_size)
    {
        try
        {
            return std::shared_ptr<char>(new char[memory_size], std::default_delete<char[]>());
        }
        catch (const std::exception&e)
        {
            printf("default_make_shared exception what=%s\n", e.what());
            return nullptr;
        }
        
    }
    

    //默认日志打印
    static std::string g_log_tag[4] = { "DEBUG","INFO","WRAN","ERROR" };
    static void default_log_print(BLOG_LEVEL lv, const std::string& log_message)
    {
        auto str = g_log_tag[lv] + ":" + log_message;
        printf(str.c_str());
    }

    //句柄生成
    static HNW_HANDLE  g_handle = INVAILD_HANDLE;
    static std::mutex  g_handle_lock;
    static HNW_HANDLE generate_handle()
    {
        std::lock_guard<std::mutex> lk(g_handle_lock);
        ++g_handle;
        if (g_handle < 0)
            g_handle = 0;
        return g_handle;
    }

    static size_t get_default_thread_num()
    {
        return std::thread::hardware_concurrency() * 2 + 2;
    }

    //基础类
    //class HnwBaseObject
    //{
    //public:
    //    //设置日志回调
    //    virtual HNW_BASE_ERR_CODE set_log_cb(HNW_LOG_CB cb)
    //    {
    //        log_cb_ = cb;
    //        return HNW_BASE_ERR_CODE::HNW_BASE_OK;
    //    }

    //    //设置缓存回调
    //    virtual HNW_BASE_ERR_CODE set_make_shared_cb(HNW_MAKE_SHARED_CB cb)
    //    {
    //        if (cb)
    //        {
    //            make_shared_ = cb;
    //            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
    //        }
    //        PRINTFLOG(BL_ERROR, "make_shared_ dont set,make_shared_ is empty!");
    //        return HNW_BASE_ERR_CODE::HNW_BASE_EMPYTY_MAKE_SHARED_CB;
    //    }
    //protected:
    //    //回调函数
    //    HNW_MAKE_SHARED_CB make_shared_;
    //    HNW_LOG_CB log_cb_;
    //};
}
#endif