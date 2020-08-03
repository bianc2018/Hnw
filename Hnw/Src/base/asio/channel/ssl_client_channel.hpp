/*
	ssl 支持
*/

#ifndef HNW_ASIO_SSL_CLIENT_HPP_
#define HNW_ASIO_SSL_CLIENT_HPP_

#include "../service/asio_client_channel.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
namespace hnw
{
    namespace boost_asio
    {
        using namespace boost::asio;
        typedef boost::asio::io_service io_service;

        typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;
        typedef boost::asio::ssl::context ssl_ctx;
        typedef boost::asio::ssl::context::context_base::method ssl_method;
        class ASIOSSLClientChannel :public ASIOClientChannel
        {
            
        public:
            ASIOSSLClientChannel(io_service& service, ssl_method m= ssl_method::sslv23)
                :ASIOClientChannel(service), ctx_(new ssl_ctx(m)),
                socket_(service, *ctx_)
            {
                
                ///boost::
            }
            ASIOSSLClientChannel(io_service& service, ssl_ctx& ctx)
                :ASIOClientChannel(service), ctx_(nullptr), socket_(service, ctx)
            {

                ///boost::
            }
            virtual ~ASIOSSLClientChannel()
            {
                close();
                if (ctx_)
                    delete ctx_;
            }

        public:
            //初始化
            virtual HNW_BASE_ERR_CODE init(const NetPoint& local)
            {
                if (INVAILD_HANDLE == handle_)
                {
                    PRINTFLOG(BL_ERROR, "invaild handle");
                    return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
                }
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //连接
            virtual HNW_BASE_ERR_CODE connect(const NetPoint& remote)
            {
                if (false == check_endpoint(remote))
                {
                    PRINTFLOG(BL_ERROR, "ERROR,POINT[%s:%d]", remote.ip.c_str(), remote.port);
                    return HNW_BASE_ERR_CODE::HNW_BASE_ADDRESS_ENDPOINT_ERROR;
                }

                ip::tcp::endpoint remote_point;
                try
                {
                    remote_point = ip::tcp::endpoint(ip::address::from_string(remote.ip), remote.port);
                }
                catch (const std::exception& e)
                {
                    PRINTFLOG(BL_DEBUG, "create asio point error what()=%s", e.what());
                    return HNW_BASE_ERR_CODE::HNW_BASE_BIND_ADDRESS_FAIL;
                }

               /* socket_.async_handshake()*/
                auto timer = add_timer(connect_time_out_ms_, [this]() 
                    {
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CONNECT_TIME_OUT, "connect time out");
                    });

                auto self = shared_from_this();
                auto con_handle = [this, timer, self, remote](boost::system::error_code ec)
                {
                    if (ec)
                    {
                        //减少打印
                        PRINTFLOG(BL_DEBUG, "async_connect error what()=%s", ec.message().c_str());
                        //上层关闭
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CONNECT_FAIL, "connect fail");
                        //close();
                        return;
                    }

                    auto handshake_handle = [this, timer, self, remote](boost::system::error_code ec)
                    {
                        //删除计时
                        del_timer(timer);

                        if (ec)
                        {
                            //减少打印
                            PRINTFLOG(BL_DEBUG, "async_handshake error what()=%s", ec.message().c_str());
                            //上层关闭
                            EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_SSL_CLIENT_HANDSHAKE_FAIL, "ssl client hand shake fail");
                            //close();
                            return;
                        }
                        bconn_ = true;
                        PRINTFLOG(BL_DEBUG, "tcp client[%I64d] is async_handshake remote[%s:%d]", \
                            get_handle(), remote.ip.c_str(), remote.port);
                        EVENT_OK(HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH);
                        //接收数据
                        async_recv();
                    };

                    //ssl 握手
                    socket_.async_handshake(boost::asio::ssl::stream_base::handshake_type::client, handshake_handle);
                };
                socket_.lowest_layer().async_connect(remote_point, con_handle);
                PRINTFLOG(BL_DEBUG, "tcp client[%I64d] begin connect to remote[%s:%d]", \
                    get_handle(), remote.ip.c_str(), remote.port);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //关闭一个通道
            virtual HNW_BASE_ERR_CODE close()
            {
                if (bconn_)
                {
                    try
                    {
                        socket_.shutdown();
                       /* socket_.shutdown(socket_base::shutdown_send);
                        socket_.shutdown(socket_base::shutdown_receive);*/
                        //ctx_.

                    }
                    catch (std::exception& e)
                    {
                        PRINTFLOG(BL_ERROR, "exception:%s", e.what());
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR, "close fail");
                        return HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR;
                    }
                 //   socket_.close();
                    bconn_ = false;
                    PRINTFLOG(BL_DEBUG, "TcpClientChannel[%I64d] closed ", handle_);
                    EVENT_OK(HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED);
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                return HNW_BASE_ERR_CODE::HNW_BASE_EMPTY_SOCKET;
            }

        public:
            //获取socket
            ssl_socket::lowest_layer_type& get_socket()
            {
                return socket_.lowest_layer();
            }

            //is open
            bool is_open()
            {
                //socket_.lowest_layer().
                //套接字已打开
                return socket_.lowest_layer().is_open();
            }

            bool before_accept()
            {
                //同步，为了统一借口
                boost::system::error_code ec;
                socket_.handshake(boost::asio::ssl::stream_base::handshake_type::server, ec);

                if (ec)
                {
                    //减少打印
                    PRINTFLOG(BL_DEBUG, "handshake error what()=%s", ec.message().c_str());
                    //上层关闭
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_SSL_SERVER_HANDSHAKE_FAIL, "ssl server hand shake fail");
                    //close();
                    return false;
                }
                return ASIOClientChannel::before_accept();
            }

        protected:
            //直接写发送数据
            virtual  HNW_BASE_ERR_CODE write(char* buff,
                size_t buff_len,
                AsioHandler handler
            )
            {
                socket_.async_write_some(boost::asio::buffer((char*)buff, buff_len), \
                    handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //直接读取接收数据
            virtual  HNW_BASE_ERR_CODE read(char* buff,
                size_t buff_len,
                AsioHandler handler
            )
            {
                socket_.async_read_some(boost::asio::buffer(buff, buff_len), handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
        private:
            
            ssl_ctx *ctx_;

            ssl_socket socket_;
        };
    }
}
#endif