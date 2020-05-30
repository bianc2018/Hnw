/*
	ssl 服务端
*/
#ifndef HNW_ASIO_SSL_SERVER_HPP_
#define HNW_ASIO_SSL_SERVER_HPP_
#include "ssl_client_channel.hpp"
#include "tcp_server_channel.hpp"

namespace hnw
{
    std::string get_password(std::size_t size,
        boost::asio::ssl::context_base::password_purpose purpose)
    {
        return "test";
    }

    namespace boost_asio
    {
        class ASIOSSLServerChannel:public ASIOTcpServerChannel
        {
        public:
            ASIOSSLServerChannel(io_service& service, ACCEPT_CHANNEL_FN cb,ssl_method m = ssl_method::sslv23_server)
                :ASIOTcpServerChannel(service,cb),ctx_(m)
            {
                ctx_.set_options(
                    boost::asio::ssl::context::default_workarounds
                    | boost::asio::ssl::context::no_sslv2| boost::asio::ssl::context::single_dh_use);
                ctx_.set_verify_mode(boost::asio::ssl::verify_none);

                ctx_.set_password_callback(get_password);
            }
            virtual ~ASIOSSLServerChannel()
            {

            }
        public:
            virtual std::shared_ptr<ASIOSSLClientChannel>
                new_ssl_client()
            {
                //创建客户端
                auto cli = std::make_shared<ASIOSSLClientChannel>(service_,ctx_);
                cli->init(NET_INVALID_POINT);
                cli->set_shared_cb(make_shared_);
                cli->set_log_cb(log_cb_);

                cli->set_event_cb(event_cb_);
                return cli;
            }

            //异步接收链接
            virtual HNW_BASE_ERR_CODE async_one_accept()
            {
                //创建客户端
                auto cli = new_ssl_client();

                if (nullptr == cli)
                {
                    PRINTFLOG(BL_DEBUG, "make_shared client error");
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL, "accept not alloc client");
                    return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                }

                PRINTFLOG(BL_DEBUG, "%I64d accept one clent ", handle_);

                //异步接收连接
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
                            //协议栈异步处理连接
                            cli->before_accept();
                            PRINTFLOG(BL_DEBUG, "%I64d accept a clent[%I64d] "\
                                , handle_, cli->get_handle());

                            accept_cb_(cli);

                            EVENT_ACCEPT_CB(cli->get_handle());

                            cli->after_accept();
                        }
                        else
                        {
                            PRINTFLOG(BL_ERROR, "%I64d accept clent error,client is not open "\
                                , handle_);
                            //LOG_INFO << "获得连接：套接字错误 ";
                            //协议栈异步处理连接
                            EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ACCEPT_CLI_IS_NOT_OPEN, "accept cb is not set");
                        }

                        //重复接收
                        async_one_accept();

                    }

                });
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //配置
            virtual HNW_BASE_ERR_CODE config(int config_type, void* data, size_t data_len)
            {
                if (SET_SSL_SERVER_CERT_FILE_PATH == config_type)
                {
                    char* pth = (char*)data;
                    PRINTFLOG(BL_DEBUG, "server cert file path is setd %s", pth);
                   // ctx_.use_certificate_chain_file(pth);
                    ctx_.use_certificate_file(pth, boost::asio::ssl::context::file_format::pem);
                   // ctx_.load_verify_file(pth/*, boost::asio::ssl::context::file_format::pem*/);
                   // ctx_.use_private_key_file(pth, boost::asio::ssl::context::file_format::pem);
 
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                //SET_SSL_SERVER_PRI_KEY_FILE_PATH
                else if (SET_SSL_SERVER_PRI_KEY_FILE_PATH == config_type)
                {
                    char* pth = (char*)data;
                    PRINTFLOG(BL_DEBUG, "server private_key file path is setd %s", pth);
                    // ctx_.use_certificate_chain_file(pth);
                   // ctx_.use_certificate_file(pth, boost::asio::ssl::context::file_format::pem);
                    ctx_.use_private_key_file(pth, boost::asio::ssl::context::file_format::pem);

                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else if (SET_SSL_SERVER_TEMP_DH_FILE_PATH == config_type)
                {
                    char* pth = (char*)data;
                    PRINTFLOG(BL_DEBUG, "server private_key file path is setd %s", pth);
                    // ctx_.use_certificate_chain_file(pth);
                   // ctx_.use_certificate_file(pth, boost::asio::ssl::context::file_format::pem);
                    ctx_.use_tmp_dh_file(pth);

                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                else
                    return ASIOTcpServerChannel::config(config_type,data,data_len);
            }
        protected:
            ssl_ctx ctx_;
        };
    }
}
#endif