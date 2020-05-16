/*
	ssl �����
*/
#ifndef HNW_ASIO_SSL_SERVER_HPP_
#define HNW_ASIO_SSL_SERVER_HPP_
#include "ssl_client_channel.hpp"
#include "tcp_server_channel.hpp"

namespace hnw
{
    namespace boost_asio
    {
        class ASIOSSLServerChannel:public ASIOTcpServerChannel
        {
        public:
            ASIOSSLServerChannel(io_service& service, ACCEPT_CHANNEL_FN cb,ssl_method m = ssl_method::sslv23)
                :ASIOTcpServerChannel(service,cb),ctx_(m)
            {

            }
            virtual ~ASIOSSLServerChannel()
            {

            }
        public:
            virtual std::shared_ptr<ASIOSSLClientChannel>
                new_ssl_client()
            {
                //�����ͻ���
                auto cli = std::make_shared<ASIOSSLClientChannel>(service_,ctx_);
                cli->init(NET_INVALID_POINT);
                cli->set_shared_cb(make_shared_);
                cli->set_log_cb(log_cb_);
                cli->set_event_cb(event_cb_);
                return cli;
            }

            //�첽��������
            virtual HNW_BASE_ERR_CODE async_one_accept()
            {
                //�����ͻ���
                auto cli = new_ssl_client();

                if (nullptr == cli)
                {
                    PRINTFLOG(BL_DEBUG, "make_shared client error");
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL, "accept not alloc client");
                    return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                }

                PRINTFLOG(BL_DEBUG, "%I64d accept one clent ", handle_);

                //�첽��������
                auto self = shared_from_this();
                acceptor_.async_accept(cli->get_socket(), [this, self, cli]\
                    (const boost::system::error_code & error)
                {
                    if (error)
                    {
                        PRINTFLOG(BL_DEBUG, "async accept error what %s", error.message().c_str());

                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ACCEPT_FAIL, "accept fail what=" + error.message());
                        return;
                    }
                    else
                    {
                        if (cli->is_open())
                        {
                            //Э��ջ�첽��������
                            cli->after_accept();
                            PRINTFLOG(BL_DEBUG, "%I64d accept a clent[%I64d] "\
                                , handle_, cli->get_handle());

                            accept_cb_(cli);

                            EVENT_ACCEPT_CB(cli->get_handle());
                        }
                        else
                        {
                            PRINTFLOG(BL_ERROR, "%I64d accept clent error,client is not open "\
                                , handle_);
                            //LOG_INFO << "������ӣ��׽��ִ��� ";
                            //Э��ջ�첽��������
                            EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ACCEPT_CLI_IS_NOT_OPEN, "accept cb is not set");
                        }

                        //�ظ�����
                        async_one_accept();

                    }

                });
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
        protected:
            ssl_ctx ctx_;
        };
    }
}
#endif