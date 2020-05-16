/*
    通道基类
    一个实例代表一个数据通道
    仅仅使用通信功能,不提供检验等功能
    hql 2020 01 15
*/
#ifndef HNW_CHANNEL_H_
#define HNW_CHANNEL_H_
#include "define.hpp"

namespace hnw
{
    class Channel :public std::enable_shared_from_this<Channel>
    {
    public:
        Channel()
            :handle_(generate_handle()), recv_buff_size_(hnw::default_recv_buff_size)
        {}

        virtual ~Channel()
        {}

        //初始化
        virtual HNW_BASE_ERR_CODE init(const NetPoint& local)
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :init");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        //设置事件回调
        virtual HNW_BASE_ERR_CODE set_event_cb(HNW_EVENT_CB cb)
        {
            event_cb_ = cb;
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        }

        //日志回调
        virtual HNW_BASE_ERR_CODE set_log_cb(HNW_LOG_CB cb)
        {
            log_cb_ = cb;
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        }

        //buff申请回调
        virtual HNW_BASE_ERR_CODE set_shared_cb(HNW_MAKE_SHARED_CB cb)
        {
            make_shared_ = cb;
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        }

        //buff申请回调
        virtual HNW_BASE_ERR_CODE set_recv_buff_size(size_t recv_buff_size)
        {
            recv_buff_size_ = recv_buff_size;
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        }

        //连接
        virtual HNW_BASE_ERR_CODE connect(const NetPoint& remote)
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :connect");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        //连接
        virtual HNW_BASE_ERR_CODE accept()
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :accept");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        //发送数据
        virtual HNW_BASE_ERR_CODE send(std::shared_ptr<void> message, size_t message_size)
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :send");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        //关闭一个通道
        virtual HNW_BASE_ERR_CODE close()
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :close");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }


        //配置
        virtual HNW_BASE_ERR_CODE config(int config_type,void *data,size_t data_len)
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :config");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }



        //获取句柄
        virtual HNW_HANDLE get_handle()
        {
            return handle_;
        }


    protected:
        //测试节点
        static bool check_endpoint(const NetPoint& point)
        {
            return "" != point.ip && point.port >= 0;
        }
    protected:
        //事件回调
        HNW_EVENT_CB event_cb_;

        ////回调函数
        HNW_MAKE_SHARED_CB make_shared_;
        HNW_LOG_CB log_cb_;

        //接收数据缓冲区大小
        size_t recv_buff_size_;
    protected:
        HNW_HANDLE handle_;

    };
}
#endif