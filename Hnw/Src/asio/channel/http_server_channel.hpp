/*
    http 服务端
*/
#ifndef HNW_ASIO_HTTP_SERVER_HPP_
#define HNW_ASIO_HTTP_SERVER_HPP_
#include "http_client_channel.hpp"
#include "tcp_server_channel.hpp"
namespace hnw
{
    namespace boost_asio
    {
        class HttpServerChannel :public hnw::boost_asio::ASIOTcpServerChannel
        {
        public:
            HttpServerChannel(io_service& service, ACCEPT_CHANNEL_FN cb)
                :ASIOTcpServerChannel(service, cb), service_(service)
            {}
            virtual ~HttpServerChannel()
            {

            }

        protected:
            virtual std::shared_ptr<ASIOTcpClientChannel>
                new_client()override
            {
                //创建客户端
                auto cli = std::make_shared<HttpClientChannel>(service_);
                cli->init(NET_INVALID_POINT);
                cli->set_shared_cb(make_shared_);
                cli->set_log_cb(log_cb_);
                cli->set_event_cb(event_cb_);
                cli->set_recv_parser(EMHttpParserType::REQUEST);
                //cli->set_send_parser(EMHttpParserType::RESPONSE);
                return cli;
            }
        private:
            io_service& service_;
        };
    }
}

#endif