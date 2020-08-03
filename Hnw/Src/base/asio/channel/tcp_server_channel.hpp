#ifndef HNW_ASIO_TCP_SERVER_HPP_
#define HNW_ASIO_TCP_SERVER_HPP_

#include "tcp_client_channel.hpp"

#include <boost/asio.hpp>

namespace hnw
{
    namespace boost_asio
    {
        //接收链接
        typedef std::function<HNW_BASE_ERR_CODE(std::shared_ptr<Channel> ch)> ACCEPT_CHANNEL_FN;

        using namespace boost::asio;

        class ASIOTcpServerChannel :public hnw::Channel
        {
            
           // typedef boost::asio::io_service io_service;
        public:
            ASIOTcpServerChannel(io_service& service, ACCEPT_CHANNEL_FN cb) 
                :service_(service), acceptor_(service), accept_num_(1), accept_cb_(cb)
            {}
            virtual ~ASIOTcpServerChannel()
            {
                close();
            }

        public:
            //初始化
            virtual HNW_BASE_ERR_CODE init(const NetPoint& local)
            {
                if (!accept_cb_)
                {
                    PRINTFLOG(BL_ERROR, "ERROR,accept_cb_ is empty");
                    return HNW_BASE_ERR_CODE::HNW_BASE_EMPYTY_ACCEPT_CB;
                }

                if (false == check_endpoint(local))
                {
                    PRINTFLOG(BL_ERROR, "ERROR,POINT[%s:%d]", local.ip.c_str(), local.port);
                    return HNW_BASE_ERR_CODE::HNW_BASE_ADDRESS_ENDPOINT_ERROR;
                }
                boost::system::error_code ec;
                boost::asio::ip::tcp::endpoint endpoint;

                try
                {
                    boost::asio::ip::tcp::resolver resolver(service_);
                    endpoint = *resolver.resolve(local.ip, std::to_string(local.port)).begin();
                }
                catch (const std::exception& e)
                {
                    PRINTFLOG(BL_ERROR, "resolver error message=%s", e.what());
                    return HNW_BASE_ERR_CODE::HNW_BASE_ADDRESS_ENDPOINT_ERROR;
                }

                //打开连接
                acceptor_.open(endpoint.protocol(), ec);
                if (ec)
                {
                    //printf("%s\n", ec.message().c_str());
                    //LOG_ERR << "open error " << ec.value() << ec.message();
                    PRINTFLOG(BL_ERROR, "acceptor open error local %s:%d what %s", \
                        local.ip.c_str(), local.port, ec.message().c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_SOCKET_OPEN_FAIL;
                }
                PRINTFLOG(BL_INFO, "acceptor is opened");
                //设置参数，地址可重用
               /* acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
                if (ec)
                {
                    PRINTFLOG(BL_ERROR, "set_option reuse address error local %s:%d what %s", local.ip.c_str(), \
                        local.port, ec.message().c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_SET_REUSE_ADDRESS_FAIL;
                }
                PRINTFLOG(BL_INFO, "acceptor is set reuse_address");*/
                //绑定地址
                acceptor_.bind(endpoint, ec);
                if (ec)
                {
                    PRINTFLOG(BL_ERROR, "bind error local %s:%d what %s", local.ip.c_str(), \
                        local.port, ec.message().c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_BIND_ADDRESS_FAIL;
                }
                PRINTFLOG(BL_INFO, "acceptor is bind local %s:%d", local.ip.c_str(), \
                    local.port);
                //监听
                acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
                if (ec)
                {
                    PRINTFLOG(BL_ERROR, "listen error local %s:%d what %s", local.ip.c_str(), \
                        local.port, ec.message().c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_LISTEN_FAIL;
                }
                PRINTFLOG(BL_INFO, "acceptor is listen");
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //连接
            virtual HNW_BASE_ERR_CODE accept()
            {
                PRINTFLOG(BL_INFO, "[%I64d] start async_one_accept thread num=%d", handle_, accept_num_);
                for (size_t i = 0; i < accept_num_; ++i)
                {
                    auto ret = async_one_accept();
                    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                    {
                        PRINTFLOG(BL_ERROR, "start async_one_accept error,[%I64d]", handle_);
                        return ret;
                    }
                }
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //关闭一个通道
            virtual HNW_BASE_ERR_CODE close()
            {
                if (acceptor_.is_open())
                {
                    try
                    {
                        boost::system::error_code ec;
                        acceptor_.cancel(ec);
                        acceptor_.close(ec);

                    }
                    catch (std::exception& e)
                    {
                        PRINTFLOG(BL_ERROR, "exception:%s", e.what());
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR,"close fail");
                        return HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR;
                    }
                    PRINTFLOG(BL_INFO, "[%I64d] TcpServerChannel is closed", handle_);
                    EVENT_OK(HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED);
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                return HNW_BASE_ERR_CODE::HNW_BASE_EMPTY_SOCKET;
            }
        
            //配置参数
            virtual HNW_BASE_ERR_CODE config(int config_type, void* data, size_t data_len)
            {
                if (SET_SERVER_ACCEPT_NUM == config_type)
                {
                    if (data)
                    {
                        auto p = (size_t*)data;
                        if (*p == 0)
                        {
                            PRINTFLOG(BL_ERROR, "SET_RECV_BUFF_SIZE recv_buff_size_ must be !=0");
                            return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                        }
                        set_accept_num(*p);
                        return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                    }
                    else
                    {
                        PRINTFLOG(BL_ERROR, "SET_RECV_BUFF_SIZE config error data must be size_t*");
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                    }
                }
                else
                {
                    return Channel::config(config_type, data, data_len);
                }


            }
        public:
            HNW_BASE_ERR_CODE set_accept_num(size_t num)
            {
                if (num <= 0)
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                accept_num_ = num;
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

        protected:
            virtual std::shared_ptr<ASIOTcpClientChannel>
                new_client()
            {
                //创建客户端
                auto cli = std::make_shared<ASIOTcpClientChannel>(service_);
                cli->init_accept(shared_from_this(), NET_INVALID_POINT);
                return cli;
            }

            //异步接收链接
            virtual HNW_BASE_ERR_CODE async_one_accept()
            {
                //创建客户端
                auto cli = new_client();

                if (nullptr == cli)
                {
                    PRINTFLOG(BL_DEBUG, "make_shared client error");
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL,"accept not alloc client");
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

                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ACCEPT_FAIL,"accept fail what="+ error.message());
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
                            EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ACCEPT_CLI_IS_NOT_OPEN,"accept cb is not set");
                        }

                        //重复接收
                        async_one_accept();

                    }

                });
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

       // private:
            //接收请求数目
            size_t accept_num_;

            //add channel 将接收的通道加到系统中去
            ACCEPT_CHANNEL_FN accept_cb_;

            ip::tcp::acceptor acceptor_;

            io_service& service_;
        };
    }
}
#endif