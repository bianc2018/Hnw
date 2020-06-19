#ifndef HNW_ASIO_UDP_HPP_
#define HNW_ASIO_UDP_HPP_

#include "../../define/channel.hpp"

#include <boost/asio.hpp>

namespace hnw
{
    namespace boost_asio
    {
        using namespace boost::asio;
        class ASIOUdpChannel :public hnw::Channel
        {
            
            typedef boost::asio::io_service io_service;
        public:
            ASIOUdpChannel(io_service& service) :service_(service), socket_(service)
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

            //发送数据
            virtual HNW_BASE_ERR_CODE send(std::shared_ptr<void> message, size_t message_size)
            {
                return async_send(message, message_size);
            }

            //发送数据
            virtual HNW_BASE_ERR_CODE send(HNW_SEND_CB cb)
            {
                return async_send(nullptr, 0, cb);
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
            //异步发送数据
            HNW_BASE_ERR_CODE async_send(std::shared_ptr<void> buff,
                size_t buff_len, size_t beg = 0,  HNW_CALL complete = nullptr)
            {
                //写完数据后的回调函数
                auto self = shared_from_this();
                auto send_handler = \
                    [this, self, buff_len, beg, buff, complete](boost::system::error_code ec, std::size_t s)
                {

                    if (ec)
                    {
                        PRINTFLOG(BL_DEBUG, "async_send error what()=%s", ec.message().c_str());
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_SEND_DATA_FAIL,"send fail "+ec.message());

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
                        if (complete)
                            complete();
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
                
                socket_.async_send_to(boost::asio::buffer((char*)buff.get() + beg, buff_len)
                    , remote_point_
                    , send_handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //异步发送数据
            HNW_BASE_ERR_CODE async_send(std::shared_ptr<void> buff, size_t buff_len,
                HNW_SEND_CB cb)
            {

                //申请缓存
                if (nullptr == buff || buff_len != send_buff_size_)
                {
                    buff_len = send_buff_size_;
                    buff = MAKE_SHARED(send_buff_size_);
                    if (nullptr == buff)
                    {
                        PRINTFLOG(BL_ERROR, "get cache error ch :%I64d", handle_);
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL, "not alloc data");
                        //close();
                        return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                    }
                }

                //读取数据
                size_t read_data_size = 0;
                if (cb)
                    read_data_size = cb(buff, buff_len);

                //完毕
                if (0 == read_data_size)
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;

                //发送
                return async_send(buff, read_data_size, 0, [this, buff, buff_len, cb]()mutable
                    {
                        //继续发送
                        async_send(buff, buff_len, cb);
                    });
            }

            //异步读数据
            HNW_BASE_ERR_CODE async_recv(std::shared_ptr<void> buff = nullptr, size_t buff_len = 0)
            {
                if (nullptr == buff)
                {
                    buff_len = recv_buff_size_;
                    buff = MAKE_SHARED(recv_buff_size_);
                    if (nullptr == buff)
                    {
                        PRINTFLOG(BL_ERROR, "get cache error ch :%I64d", handle_);
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL,"udp recv buff not alloc");
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
                        //EVENT_ERR_CB(HNW_BASE_EVENT_TYPE::HNW_BASE_RECV_DATA,
                        //  HNW_BASE_ERR_CODE::HNW_BASE_RECV_DATA_FAIL);
                        //close();
                        return;
                    }
                    PRINTFLOG(BL_DEBUG, "async_read  %p:len[%d]", buff.get(), recv_len);
                    EVENT_RECV_CB(buff, recv_len);
                    //重复接收数据
                    async_recv(buff, buff_len);
                };

                //异步读数据
                socket_.async_receive(boost::asio::buffer(buff.get(), buff_len), recv_handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
        private:
            io_service& service_;

            //套接字
            ip::udp::socket socket_;

            //
            ip::udp::endpoint remote_point_;
        };
    }
}
#endif