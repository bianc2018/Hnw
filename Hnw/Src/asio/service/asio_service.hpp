#ifndef HNW_ASIO_SERVICE_HPP_
#define HNW_ASIO_SERVICE_HPP_

#include "../../define/service.hpp"

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

//֧�ֵ�channel
#include "../channel/tcp_client_channel.hpp"
#include "../channel/tcp_server_channel.hpp"
#include "../channel/udp_channel.hpp"
#include "../channel/http_client_channel.hpp"
#include "../channel/http_server_channel.hpp"

namespace hnw
{
    namespace boost_asio
    {
        //����������
        class AsioService:public hnw::Service
        {
            //using namespace boost::asio;
            typedef boost::asio::io_service io_service;
            //��Žӿ�
        public:
            AsioService() :service_(), work_(service_), \
                run_flag_(true),thread_num_(get_default_thread_num())
            {
                PRINTFLOG(BL_DEBUG, "BnsService init thread_num=%d", thread_num_);
                for (int i = 0; i < thread_num_; ++i)
                {
                    PRINTFLOG(BL_DEBUG, "BnsService  run_worker %d", i);
                    thread_vec_.push_back(std::thread(&AsioService::run_worker, this));
                }
                PRINTFLOG(BL_DEBUG, "BnsService init ok");
            }

            virtual ~AsioService()
            {
                //��ֹ�ظ��ͷ�
                if (run_flag_)
                {
                    run_flag_ = false;

                    //ɾ���ͷ����е��豸
                    clear_and_close_channels();

                    //����
                    service_.stop();
                    service_.reset();

                    PRINTFLOG(BL_DEBUG, "BnsService asio service is stoped");

                    //�ͷ�
                    work_.~work();
                    PRINTFLOG(BL_DEBUG, "BnsService asio work is release");

                    for (auto& t : thread_vec_)
                    {
                        if (t.joinable())
                            t.join();
                    }
                    PRINTFLOG(BL_DEBUG, "all work thread is join !");
                }
                PRINTFLOG(BL_DEBUG, "BnsService is release !");
            }
            ////��ʼ��
            //virtual HNW_BASE_ERR_CODE init()override
            //{
            //    PRINTFLOG(BL_DEBUG, "BnsService init thread_num=%d", thread_num_);
            //    for (int i = 0; i < thread_num_; ++i)
            //    {
            //        PRINTFLOG(BL_DEBUG, "BnsService  run_worker %d", i);
            //        thread_vec_.push_back(std::thread(&AsioService::run_worker, this));
            //    }
            //    PRINTFLOG(BL_DEBUG, "BnsService init ok");
            //    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            //}

            ////����ʼ��
            //virtual HNW_BASE_ERR_CODE de_init()override
            //{
            //    //��ֹ�ظ��ͷ�
            //    if (run_flag_)
            //    {
            //        run_flag_ = false;

            //        //ɾ���ͷ����е��豸
            //        clear_and_close_channels();

            //        //����
            //        service_.stop();
            //        service_.reset();

            //        PRINTFLOG(BL_DEBUG, "BnsService asio service is stoped");

            //        //�ͷ�
            //        work_.~work();
            //        PRINTFLOG(BL_DEBUG, "BnsService asio work is release");

            //        for (auto& t : thread_vec_)
            //        {
            //            if (t.joinable())
            //                t.join();
            //        }
            //        PRINTFLOG(BL_DEBUG, "all work thread is join !");
            //    }
            //    PRINTFLOG(BL_DEBUG, "BnsService is release !");
            //    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            //}

            //����ͨ��
            virtual HNW_BASE_ERR_CODE add_channel(HNW_CHANNEL_TYPE type\
                , const NetPoint& local, HNW_HANDLE& handle)override
            {
                PRINTFLOG(BL_INFO, "add a channel type=%d local=%s:%d", type, local.ip.c_str(), local.port);
                //�����׽���
                std::shared_ptr<Channel>ch = nullptr;
                if (HNW_CHANNEL_TYPE::TCP_CLIENT == type)
                {
                    ch = std::make_shared<ASIOTcpClientChannel>(service_);
                    if (ch)
                        PRINTFLOG(BL_DEBUG, "new a TcpClientChannel handle[%I64d]", ch->get_handle());
                }
                else if (HNW_CHANNEL_TYPE::TCP_SERVER == type)
                {
                    ch = std::make_shared<ASIOTcpServerChannel>(service_, \
                        std::bind(&AsioService::add_channel_to_map, this, std::placeholders::_1));
                    if (ch)
                        PRINTFLOG(BL_DEBUG, "new a TcpServerChannel handle[%I64d]", ch->get_handle());
                }
                else if (HNW_CHANNEL_TYPE::UDP == type)
                {
                    ch = std::make_shared<ASIOUdpChannel>(service_);
                    if (ch)
                        PRINTFLOG(BL_DEBUG, "new a UdpChannel handle[%I64d]", ch->get_handle());
                }
                else if (HNW_CHANNEL_TYPE::HTTP_CLIENT == type)
                {
                    ch = std::make_shared<HttpClientChannel>(service_);
                    if (ch)
                        PRINTFLOG(BL_DEBUG, "new a HttpClientChannel handle[%I64d]", ch->get_handle());
                }
                else if (HNW_CHANNEL_TYPE::HTTP_SERVER == type)
                {
                    ch = std::make_shared<HttpServerChannel>(service_, \
                        std::bind(&AsioService::add_channel_to_map, this, std::placeholders::_1));
                    if (ch)
                        PRINTFLOG(BL_DEBUG, "new a HttpServerChannel handle[%I64d]", ch->get_handle());
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "no support type=%d", type);
                    return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
                }

                if (nullptr == ch)
                {
                    PRINTFLOG(BL_ERROR, "channel cannt create  type=%d", type);
                    return HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_CREATE_FAIL;
                }

                PRINTFLOG(BL_DEBUG, "channel init ");

                ch->set_log_cb(std::bind(&AsioService::printf_log, \
                    this, std::placeholders::_1, std::placeholders::_2));
                ch->set_shared_cb(std::bind(&AsioService::get_cache, this, std::placeholders::_1));

                auto err = ch->init(local);

                if (HNW_BASE_ERR_CODE::HNW_BASE_OK != err)
                {
                    PRINTFLOG(BL_ERROR, "channel[%I64d] init fail ret=%d", ch->get_handle(), err);
                    return err;
                }

                handle = ch->get_handle();
                PRINTFLOG(BL_DEBUG, "try add channel[%I64d] ", handle);
                return add_channel_to_map(ch);
            }

            //��ѯdns
            virtual HNW_BASE_ERR_CODE query_dns(const std::string& host
                , std::vector<NetPoint>& addrs, const std::string& service)override
            {
                try
                { 
                    //����resolver����
                    ip::tcp::resolver slv(service_);
                    //����query����
                    ip::tcp::resolver::query qry(host, service);
                    //ʹ��resolve�����˵�
                    ip::tcp::resolver::iterator it = slv.resolve(qry);
                    ip::tcp::resolver::iterator end;
                    for (; it != end; it++)
                    {
                        NetPoint p;
                        p.ip = (*it).endpoint().address().to_string();
                        p.port = (*it).endpoint().port();
                        addrs.push_back(p);
                    }
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                catch (std::exception& e)
                {
                    PRINTFLOG(BL_ERROR, "Dns query fail what =%s", e.what());
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
                
            }
        private:
            //������
            void run_worker()
            {
                PRINTFLOG(BL_DEBUG, "run_worker is start!");
                service_.run();
                PRINTFLOG(BL_DEBUG, "run_worker is end!");
            }

        private:
           
            //����
            io_service service_;
            io_service::work work_;

            //�����߳�
            std::atomic_bool run_flag_;
            int thread_num_;
            std::vector<std::thread> thread_vec_;
        };
    }
}
#endif