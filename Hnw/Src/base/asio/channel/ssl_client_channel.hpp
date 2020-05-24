/*
	ssl 支持
*/

#ifndef HNW_ASIO_SSL_CLIENT_HPP_
#define HNW_ASIO_SSL_CLIENT_HPP_

#include "../../define/channel.hpp"

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
        class ASIOSSLClientChannel :public hnw::Channel
        {
            
        public:
            ASIOSSLClientChannel(io_service& service, ssl_method m= ssl_method::sslv23)
                :service_(service), ctx_(new ssl_ctx(m)),
                socket_(service, *ctx_), bconn_(false)
            {
                
                ///boost::
            }
            ASIOSSLClientChannel(io_service& service, ssl_ctx& ctx)
                :service_(service), ctx_(nullptr), socket_(service, ctx), bconn_(false)
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

                auto self = shared_from_this();
                auto con_handle = [this, self, remote](boost::system::error_code ec)
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

                    auto handshake_handle = [this, self, remote](boost::system::error_code ec)
                    {
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

            //发送数据
            virtual HNW_BASE_ERR_CODE send(std::shared_ptr<void> message, size_t message_size)
            {
                return async_send(message, message_size);
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
                bconn_ = true;
                return true;
            }
            bool after_accept()
            {
                //auto self = shared_from_this();
                //auto handshake_handle = [this, self](boost::system::error_code ec)
                //{
                //    if (ec)
                //    {
                //        //减少打印
                //        PRINTFLOG(BL_DEBUG, "async_handshake error what()=%s", ec.message().c_str());
                //        //上层关闭
                //        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_SSL_CLIENT_HANDSHAKE_FAIL, "ssl client hand shake fail");
                //        //close();
                //        return;
                //    }
                //    bconn_ = true;
                //    //EVENT_OK(HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH);
                //    //接收数据
                //    async_recv();
                //};

                ////ssl 握手
                ////socket_.async_handshake(boost::asio::ssl::stream_base::handshake_type::server, handshake_handle);

                async_recv();
                return true;
            }

        protected:
            //异步发送数据
            HNW_BASE_ERR_CODE async_send(std::shared_ptr<void> buff, size_t buff_len, size_t beg = 0)
            {
                if (buff_len == 0 || nullptr == buff)
                {
                    EVENT_SEND_CB(buff, buff_len);
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }

                if (false == bconn_)
                {
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN, "channel is not conn ,don't send");
                    return HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN;
                }
                //写完数据后的回调函数
                auto self = shared_from_this();
                auto send_handler = \
                    [this, self, buff_len, beg, buff](boost::system::error_code ec, std::size_t s)
                {

                    if (ec)
                    {
                        PRINTFLOG(BL_DEBUG, "async_send error what()=%s", ec.message().c_str());
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_SEND_DATA_FAIL, "send data fail");
                        //close();
                        return;
                    }
                    //写完了
                    if (buff_len <= s)
                    {
                        //返回异步写,取下一个buff
                       // if (on_complate_)
                        //    on_complate_(true);
                        EVENT_SEND_CB(buff, buff_len);
                        return;
                    }
                    //未写完
                    //计算剩下的字节
                    int now_len = buff_len - s;
                    PRINTFLOG(BL_DEBUG, "async_send  %p:len[%d]", buff.get(), s);
                    //继续写
                    async_send(buff, now_len, beg + s);
                    return;
                };

                socket_.async_write_some(boost::asio::buffer((char*)buff.get() + beg, buff_len), \
                    send_handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //异步接收数据
            HNW_BASE_ERR_CODE async_recv(std::shared_ptr<void> buff = nullptr, size_t buff_len = 0)
            {
                if (false == bconn_)
                {
                    PRINTFLOG(BL_ERROR, "TcpClientChannel[%I64d] is not connected ", handle_);
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN, "channel no connect ,dont recv data");
                    return HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN;
                }

                if (nullptr == buff)
                {
                    buff_len = recv_buff_size_;
                    buff = MAKE_SHARED(recv_buff_size_);
                    if (nullptr == buff)
                    {
                        PRINTFLOG(BL_ERROR, "get cache error ch :%I64d", handle_);
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL, "not alloc data");
                        //close();
                        return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                    }

                }
                auto self = shared_from_this();
                auto recv_handler = [this, self, buff, buff_len]\
                    (boost::system::error_code ec, size_t recv_len)
                {
                    if (ec)
                    {
                        //
                        PRINTFLOG(BL_DEBUG, "async_read error what()=%s", ec.message().c_str());
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_RECV_DATA_FAIL, "cannt recv data");
                        //close();
                        return;
                    }
                    PRINTFLOG(BL_DEBUG, "async_read  %p:len[%d]", buff.get(), recv_len);
                    EVENT_RECV_CB(buff, recv_len);
                    //重复接收数据
                    async_recv(buff, buff_len);
                };
                socket_.async_read_some(boost::asio::buffer(buff.get(), buff_len), recv_handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
        private:
            //是否链接？
            std::atomic_bool bconn_;

            ssl_ctx *ctx_;

            ssl_socket socket_;

            io_service& service_;
        };
    }
}
#endif