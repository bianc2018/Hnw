/*
    �����ܻ�����
*/
#ifndef HNW_BASE_SERVICE_HPP_
#define HNW_BASE_SERVICE_HPP_

#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>

#include "channel.hpp"

namespace hnw
{
    //����������
    class Service
    {
        //��Žӿ�
    public:
        Service()
            :make_shared_(hnw::default_make_shared),
            log_cb_(hnw::default_log_print),
            recv_buff_size_(hnw::default_recv_buff_size),
            send_buff_size_(hnw::default_send_buff_size),
            accept_num_(hnw::default_accept_num)
        {}
        virtual ~Service()
        {}
        ////��ʼ��
        //virtual HNW_BASE_ERR_CODE  init() = 0;

        ////����ʼ��
        //virtual HNW_BASE_ERR_CODE de_init() = 0;

        //����ͨ��
        virtual HNW_BASE_ERR_CODE add_channel(HNW_CHANNEL_TYPE type\
            , const NetPoint& local, HNW_HANDLE& handle) = 0;

        //��������
        virtual HNW_BASE_ERR_CODE connect(HNW_HANDLE handle, const NetPoint& remote)
        {
            auto ch = get_channel(handle);
            if (ch)
            {
                PRINTFLOG(BL_INFO, "connect channel handle=%I64d remote=%s:%d", \
                    handle, remote.ip.c_str(), remote.port);
                return ch->connect(remote);
            }
            PRINTFLOG(BL_ERROR, "connect handle=%I64d not find,this handle is invaild", \
                handle);
            return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
        }

        //��������
        virtual HNW_BASE_ERR_CODE accept(HNW_HANDLE handle)
        {
            auto ch = get_channel(handle);
            if (ch)
            {
                PRINTFLOG(BL_INFO, "accept channel handle=%I64d ", \
                    handle);
                return ch->accept();
            }
            PRINTFLOG(BL_ERROR, "accept handle=%I64d not find,this handle is invaild", \
                handle);
            return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
        }

        //�ر�һ��ͨ��
        virtual HNW_BASE_ERR_CODE close(HNW_HANDLE handle)
        {
            auto ch = get_channel(handle);
            if (nullptr == ch)
            {
                PRINTFLOG(BL_ERROR, "ch[%lld] is no exist!", handle);
                return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
            }

            del_channel(handle);

            PRINTFLOG(BL_INFO, "close channel handle=%I64d", handle);
            return ch->close();
        }

        //����
        virtual HNW_BASE_ERR_CODE config(HNW_HANDLE handle, 
            int config_type, void* data, size_t data_len)
        {
            //ȫ�ֵ�
            if (handle == INVAILD_HANDLE)
            {
                return config(config_type, data, data_len);
            }

            //ͨ������
            auto ch = get_channel(handle);
            if (nullptr == ch)
            {
                PRINTFLOG(BL_ERROR, "ch[%lld] is no exist!", handle);
                return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
            }

            return ch->config(config_type, data, data_len);
        }

        //ȫ������
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
            else if (SET_SEND_BUFF_SIZE == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    if (*p == 0)
                    {
                        PRINTFLOG(BL_ERROR, "SET_RECV_BUFF_SIZE recv_buff_size_ must be !=0");
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                    }
                    send_buff_size_ = *p;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "SET_RECV_BUFF_SIZE config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            else if (SET_SERVER_ACCEPT_NUM == config_type)
            {
                if (data)
                {
                    auto p = (size_t*)data;
                    if (*p == 0)
                    {
                        PRINTFLOG(BL_ERROR, "SET_RECV_BUFF_SIZE recv_buff_size_ must be !=0");
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                    }
                    accept_num_ = *p;
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "SET_RECV_BUFF_SIZE config error data must be size_t*");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
            }
            PRINTFLOG(BL_DEBUG, "this channel no support :config");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        //��������
        virtual HNW_BASE_ERR_CODE send(HNW_HANDLE handle, \
            std::shared_ptr<char> message, size_t message_size)
        {
            auto ch = get_channel(handle);
            if (nullptr == ch)
            {
                PRINTFLOG(BL_ERROR, "ch[%lld] is no exist!", handle);
                return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
            }
            PRINTFLOG(BL_INFO, "channel[%I64d] send data size:%u", handle, message_size);
            return ch->send(message, message_size);
        }

        //��������
        virtual HNW_BASE_ERR_CODE send_string(HNW_HANDLE handle, const std::string& message)
        {
            if (message.empty())
            {
                PRINTFLOG(BL_WRAN, "channel[%I64d] send empty message", handle);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
            auto message_size = message.size();
            auto cache = get_cache(message_size);
            if (nullptr == cache)
            {
                PRINTFLOG(BL_ERROR, "channel[%I64d] get_cache error,size=%d", handle, message_size);
                return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
            }
            memcpy(cache.get(), message.c_str(), message_size);
            return send(handle, cache, message_size);
        }

        //��������
        virtual HNW_BASE_ERR_CODE send_cb(HNW_HANDLE handle,  HNW_SEND_CB cb)
        {
            auto ch = get_channel(handle);
            if (nullptr == ch)
            {
                PRINTFLOG(BL_ERROR, "ch[%lld] is no exist!", handle);
                return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
            }
            if (nullptr == cb)
            {
                PRINTFLOG(BL_ERROR, "ch[%lld] send cb is nullptr", handle);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
            PRINTFLOG(BL_INFO, "channel[%I64d] send data cb", handle);
            return ch->send(cb);
        }

        //�����¼��ص�
        virtual HNW_BASE_ERR_CODE set_event_cb(HNW_HANDLE handle, HNW_EVENT_CB cb)
        {
            //�ҵ���Ӧ��ͨ��
            auto ch = get_channel(handle);
            if (nullptr == ch)
            {
                PRINTFLOG(BL_ERROR, "ch[%I64d] is no exist!", handle);
                return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
            }

            //ע����Ϣ
            return ch->set_event_cb(std::bind(&Service::event_cb, this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3,
                cb));
        }

        //������־�ص�
        virtual HNW_BASE_ERR_CODE set_log_cb(HNW_LOG_CB cb)
        {
            log_cb_ = cb;
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        }

        //���û���ص�
        virtual HNW_BASE_ERR_CODE set_make_shared_cb(HNW_MAKE_SHARED_CB cb)
        {
            if (cb)
            {
                make_shared_ = cb;
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
            PRINTFLOG(BL_ERROR, "make_shared_ dont set,make_shared_ is empty!");
            return HNW_BASE_ERR_CODE::HNW_BASE_EMPYTY_MAKE_SHARED_CB;
        }

        virtual HNW_BASE_ERR_CODE query_dns(const std::string& host,
            std::vector<NetPoint>& addr, const std::string& service)
        {
            PRINTFLOG(BL_DEBUG, " no support :query_dns");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        virtual HNW_BASE_ERR_CODE broad_cast(std::string& ip)
        {
            PRINTFLOG(BL_DEBUG, " no support :broad_cast");
            return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
        }

        virtual void* get_userdata(HNW_HANDLE handle)
        {
            //�ҵ���Ӧ��ͨ��
            auto ch = get_channel(handle);
            if (nullptr == ch)
            {
                PRINTFLOG(BL_ERROR, "ch[%I64d] is no exist!", handle);
                return nullptr;
            }
            return ch->get_userdata();
        }
        virtual HNW_BASE_ERR_CODE set_userdata(HNW_HANDLE handle, void* userdata)
        {
            //�ҵ���Ӧ��ͨ��
            auto ch = get_channel(handle);
            if (nullptr == ch)
            {
                PRINTFLOG(BL_ERROR, "ch[%I64d] is no exist!", handle);
                return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
            }
            return ch->set_userdata(userdata);
        }

        //��ʱ������
        virtual HNW_BASE_ERR_CODE add_timer(size_t time_out_ms,
            std::function<void()> call,
            HNW_HANDLE& handle)
        {
            auto ret = add_channel(HNW_CHANNEL_TYPE::TIMER, NET_INVALID_POINT, handle);
            if (ret != HNW_BASE_ERR_CODE::HNW_BASE_OK)
                return ret;
            config(handle, SET_TIMER_TIME_OUT_MS, &time_out_ms, sizeof(time_out_ms));
            config(handle, SET_TIMER_CB, &call, sizeof(call));
            return connect(handle, NET_INVALID_POINT);
        }
    private:

        //�¼��ص� ���Կ����첽����
        void event_cb(std::int64_t handle, \
            int type, \
            std::shared_ptr<void> event_data, HNW_EVENT_CB cb)
        {
            //��������͹ر�
            if ((int)HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR== type)
            {
                auto error = PTR_CAST(HnwBaseErrorEvent, event_data);
                PRINTFLOG(BL_ERROR, "event_cb ch[%I64d] error,opt=%d,error=%d try to close!"\
                    , handle, type, error->code);
                
                //�ÿ�
                HnwBase_SetEvntCB(handle, nullptr);
                HnwBase_Close(handle);

                //�������ú���
                if (cb)
                    cb(handle, (int)HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED, event_data);
            }

            //����ͳ����

            //�������ú���
            if (cb)
                cb(handle, type, event_data);
        }
    
    protected:
        //��ӡ��־
        virtual void printf_log(BLOG_LEVEL lv, const std::string& message)
        {
            if (log_cb_)
                log_cb_(lv, message);
        }

        //���뻺����
        virtual std::shared_ptr<char> get_cache(size_t size)
        {
            if (make_shared_)
                return make_shared_(size);
            return nullptr;
        }

        //ͨ������
        std::shared_ptr<Channel> get_channel(HNW_HANDLE handle)
        {
            std::lock_guard<std::mutex> lk(channel_map_lock_);
            auto p = channel_map_.find(handle);
            if (channel_map_.end() == p)
                return nullptr;
            return p->second;
        }

        void del_channel(HNW_HANDLE handle)
        {
            std::lock_guard<std::mutex> lk(channel_map_lock_);
            channel_map_.erase(handle);
        }

        HNW_BASE_ERR_CODE add_channel_to_map(std::shared_ptr<Channel> channel)
        {
            std::lock_guard<std::mutex> lk(channel_map_lock_);
            channel_map_[channel->get_handle()] = channel;
            PRINTFLOG(BL_DEBUG, "channel[%I64d] is added", channel->get_handle());
            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        }

        //�ر�����ͨ��
        void clear_and_close_channels()
        {
            std::lock_guard<std::mutex> lk(channel_map_lock_);
            PRINTFLOG(BL_DEBUG, "BnsService release all channel size=%d", channel_map_.size());
            for (auto p = channel_map_.begin(); p != channel_map_.end();)
            {
                auto  ch = p->second;
                if (ch)
                {
                    PRINTFLOG(BL_DEBUG, "Service close channel[%I64d]", p->first);
                    ch->close();
                }
                else
                {
                    PRINTFLOG(BL_WRAN, "Service close channel[%I64d] is nullptr", p->first);
                }

                p = channel_map_.erase(p);
            }
        }

    protected:

        //�ص�����
        HNW_MAKE_SHARED_CB make_shared_;
        HNW_LOG_CB log_cb_;

        //�������ݻ�������С
        size_t recv_buff_size_;

        //�������ݻ�������С
        size_t send_buff_size_;

        //���ܵ��̴߳�С
        size_t accept_num_;
    private:
        //ͨ����
        std::mutex channel_map_lock_;
        //bns_channel_map_;
        std::unordered_map<HNW_HANDLE, std::shared_ptr<Channel> > channel_map_;

    };
}
#endif