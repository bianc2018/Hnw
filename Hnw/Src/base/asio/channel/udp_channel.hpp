#ifndef HNW_ASIO_UDP_HPP_
#define HNW_ASIO_UDP_HPP_

#include "../service/asio_client_channel.hpp"

#include <boost/asio.hpp>

namespace hnw
{
    namespace boost_asio
    {
        using namespace boost::asio;
        class ASIOUdpChannel :public ASIOClientChannel
        {
            
            typedef boost::asio::io_service io_service;
        public:
            ASIOUdpChannel(io_service& service) 
                :ASIOClientChannel(service), socket_(service)
            {}
            virtual ~ASIOUdpChannel()
            {
                close();
            }

        public:
            //初始化
            virtual HNW_BASE_ERR_CODE init(const NetPoint& local)
            {
                if (false == check_endpoint(local))
                {
                    PRINTFLOG(BL_ERROR, "ERROR,POINT[%s:%d]", local.ip.c_str(), local.port);
                    return HNW_BASE_ERR_CODE::HNW_BASE_ADDRESS_ENDPOINT_ERROR;
                }
                boost::system::error_code ec;
                boost::asio::ip::udp::endpoint endpoint;

                try
                {
                    boost::asio::ip::udp::resolver resolver(service_);
                    endpoint = *resolver.resolve(local.ip, std::to_string(local.port)).begin();
                }
                catch (const std::exception& e)
                {
                    PRINTFLOG(BL_ERROR, "resolver error message=%s", e.what());
                    return HNW_BASE_ERR_CODE::HNW_BASE_ADDRESS_ENDPOINT_ERROR;
                }

                //打开连接
                socket_.open(endpoint.protocol(), ec);
                if (ec)
                {
                    PRINTFLOG(BL_ERROR, "udp socket open error local %s:%d what %s", \
                        local.ip.c_str(), local.port, ec.message().c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_SOCKET_OPEN_FAIL;
                }
                PRINTFLOG(BL_INFO, "udp socket is opened");

                //绑定地址
                socket_.bind(endpoint, ec);
                if (ec)
                {
                    PRINTFLOG(BL_ERROR, "udp socket bind error local %s:%d what %s", local.ip.c_str(), \
                        local.port, ec.message().c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_BIND_ADDRESS_FAIL;
                }
                PRINTFLOG(BL_INFO, "udp socket is bind local %s:%d", local.ip.c_str(), \
                    local.port);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //连接
            virtual HNW_BASE_ERR_CODE connect(const NetPoint& remote)
            {
                //绑定远程端点
                if (false == check_endpoint(remote))
                {
                    PRINTFLOG(BL_ERROR, "ERROR,POINT[%s:%d]", remote.ip.c_str(), remote.port);
                    return HNW_BASE_ERR_CODE::HNW_BASE_ADDRESS_ENDPOINT_ERROR;
                }

                boost::system::error_code ec;
                try
                {
                   // boost::asio::ip::address_v4::broadcast();
                    boost::asio::ip::udp::resolver resolver(service_);
                    remote_point_ = *resolver.resolve(remote.ip, std::to_string(remote.port)).begin();
                }
                catch (const std::exception& e)
                {
                    PRINTFLOG(BL_ERROR, "resolver error message=%s", e.what());
                    return HNW_BASE_ERR_CODE::HNW_BASE_ADDRESS_ENDPOINT_ERROR;
                }

                //应该不用链接
                //异步读数据
                EVENT_OK(HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH);
                return async_recv();
            }

            //关闭一个通道
            virtual HNW_BASE_ERR_CODE close()
            {
                
                if (socket_.is_open())
                {
                    try
                    {
                        socket_.shutdown(socket_base::shutdown_send);
                        socket_.shutdown(socket_base::shutdown_receive);

                    }
                    catch (std::exception& e)
                    {
                        PRINTFLOG(BL_ERROR, "exception:%s", e.what());
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR,"udp closed fail");
                        return  HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR;
                    }
                    socket_.close();
                    PRINTFLOG(BL_DEBUG, "UdpChannel[%I64d] closed ", handle_);
                    EVENT_OK(HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED);
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                return HNW_BASE_ERR_CODE::HNW_BASE_EMPTY_SOCKET;
            }
            
        private:
            //直接写发送数据
            virtual  HNW_BASE_ERR_CODE write(char* buff,
                size_t buff_len,
                AsioHandler handler
            )
            {
                socket_.async_send_to(boost::asio::buffer(buff, buff_len)
                    , remote_point_
                    , handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //直接读取接收数据
            virtual  HNW_BASE_ERR_CODE read(char* buff,
                size_t buff_len,
                AsioHandler handler
            )
            {
                //异步读数据
                socket_.async_receive(boost::asio::buffer(buff, buff_len), handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
        private:
            //套接字
            ip::udp::socket socket_;

            //
            ip::udp::endpoint remote_point_;
        };
    }
}
#endif