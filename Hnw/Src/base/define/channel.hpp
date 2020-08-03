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
            :handle_(generate_handle()), 
            recv_buff_size_(hnw::default_recv_buff_size),
            send_buff_size_(hnw::default_send_buff_size),
            connect_time_out_ms_(hnw::default_connect_time_out_ms),
            recv_time_out_ms_(hnw::default_recv_time_out_ms),
            send_time_out_ms_(hnw::default_send_time_out_ms),
            userdata_(nullptr)
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

        //��ȡ�¼��ص�
        virtual HNW_EVENT_CB get_event_cb()
        {
            return event_cb_ ;
        }

        //��־�ص�
        virtual HNW_LOG_CB get_log_cb()
        {
            return log_cb_;
        }

        //buff����ص�
        virtual HNW_MAKE_SHARED_CB get_shared_cb()
        {
            return make_shared_;
        }

        ////buff����ص�
        //virtual HNW_BASE_ERR_CODE set_recv_buff_size(size_t recv_buff_size)
        //{
        //    recv_buff_size_ = recv_buff_size;
        //    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        //}

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

        //��������
        virtual HNW_BASE_ERR_CODE send(HNW_SEND_CB cb)
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
        virtual HNW_BASE_ERR_CODE config(int config_type, void* data, size_t data_len)
        {
            if (SET_RECV_BUFF_SIZE == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    if (*p == 0)
                    {
                        PRINTFLOG(BL_ERROR, "SET_RECV_BUFF_SIZE recv_buff_size_ must be !=0");
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                    }
                    recv_buff_size_ = *p;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "SET_RECV_BUFF_SIZE config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            else if (GET_RECV_BUFF_SIZE == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    *p = send_buff_size_;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "GET_RECV_BUFF_SIZE config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            else if (SET_SEND_BUFF_SIZE == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    if (*p == 0)
                    {
                        PRINTFLOG(BL_ERROR, "SET_SEND_BUFF_SIZE recv_buff_size_ must be !=0");
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                    }
                    send_buff_size_ = *p;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "SET_SEND_BUFF_SIZE config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            else if (GET_SEND_BUFF_SIZE == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    *p = send_buff_size_;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "GET_RECV_BUFF_SIZE config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            else if (SET_CONNECT_TIME_OUT_MS == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    connect_time_out_ms_ = *p;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "SET_CONNECT_TIME_OUT_MS config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            else if (GET_CONNECT_TIME_OUT_MS == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    *p = connect_time_out_ms_;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "GET_CONNECT_TIME_OUT_MS config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            else if (SET_RECV_TIME_OUT_MS == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    recv_time_out_ms_ = *p;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "SET_RECV_TIME_OUT_MS config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            else if (GET_RECV_TIME_OUT_MS == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    *p = recv_time_out_ms_;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "GET_RECV_TIME_OUT_MS config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            else if (SET_SEND_TIME_OUT_MS == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    send_time_out_ms_ = *p;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "SET_SEND_TIME_OUT_MS config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            else if (GET_SEND_TIME_OUT_MS == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    *p = send_time_out_ms_;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "GET_SEND_TIME_OUT_MS config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            PRINTFLOG(BL_DEBUG, "this channel no support :config");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        //��ȡ���
        virtual HNW_HANDLE get_handle()
        {
            return handle_;
        }

        virtual void* get_userdata()
        {
            return userdata_;
        }
        virtual HNW_BASE_ERR_CODE set_userdata(void* userdata)
        {
            userdata_ = userdata;
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
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

        //���ͻ���
        //�������ݻ�������С
        size_t send_buff_size_;

        //���ӳ�ʱ ���� 10s 0��ʱ�򲻼��㳬ʱ
        size_t connect_time_out_ms_ ;
        //���ճ�ʱ 5min
        size_t recv_time_out_ms_;
        //���ͳ�ʱ 10s
        size_t send_time_out_ms_;
    protected:
        HNW_HANDLE handle_;

        //�û�����
        void* userdata_;
    };
}
#endif