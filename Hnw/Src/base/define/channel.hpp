/*
    ͨ������
    һ��ʵ������һ������ͨ��
    ����ʹ��ͨ�Ź���,���ṩ����ȹ���
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

        //��ʼ��
        virtual HNW_BASE_ERR_CODE init(const NetPoint& local)
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :init");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        //�����¼��ص�
        virtual HNW_BASE_ERR_CODE set_event_cb(HNW_EVENT_CB cb)
        {
            event_cb_ = cb;
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        }

        //��־�ص�
        virtual HNW_BASE_ERR_CODE set_log_cb(HNW_LOG_CB cb)
        {
            log_cb_ = cb;
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        }

        //buff����ص�
        virtual HNW_BASE_ERR_CODE set_shared_cb(HNW_MAKE_SHARED_CB cb)
        {
            make_shared_ = cb;
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        }

        //buff����ص�
        virtual HNW_BASE_ERR_CODE set_recv_buff_size(size_t recv_buff_size)
        {
            recv_buff_size_ = recv_buff_size;
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        }

        //����
        virtual HNW_BASE_ERR_CODE connect(const NetPoint& remote)
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :connect");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        //����
        virtual HNW_BASE_ERR_CODE accept()
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :accept");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        //��������
        virtual HNW_BASE_ERR_CODE send(std::shared_ptr<void> message, size_t message_size)
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :send");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        //�ر�һ��ͨ��
        virtual HNW_BASE_ERR_CODE close()
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :close");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }


        //����
        virtual HNW_BASE_ERR_CODE config(int config_type,void *data,size_t data_len)
        {
            PRINTFLOG(BL_DEBUG, "this channel no support :config");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }



        //��ȡ���
        virtual HNW_HANDLE get_handle()
        {
            return handle_;
        }


    protected:
        //���Խڵ�
        static bool check_endpoint(const NetPoint& point)
        {
            return "" != point.ip && point.port >= 0;
        }
    protected:
        //�¼��ص�
        HNW_EVENT_CB event_cb_;

        ////�ص�����
        HNW_MAKE_SHARED_CB make_shared_;
        HNW_LOG_CB log_cb_;

        //�������ݻ�������С
        size_t recv_buff_size_;
    protected:
        HNW_HANDLE handle_;

    };
}
#endif