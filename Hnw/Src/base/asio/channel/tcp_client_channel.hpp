#ifndef HNW_ASIO_TCP_CLIENT_HPP_
#define HNW_ASIO_TCP_CLIENT_HPP_

#include "../service/asio_client_channel.hpp"

#include <boost/asio.hpp>

namespace hnw
{
    namespace boost_asio
    {
        using namespace boost::asio;
        typedef boost::asio::io_service io_service;
		class ASIOTcpClientChannel:public ASIOClientChannel
		{
			
		public:
			ASIOTcpClientChannel(io_service &service)
                :ASIOClientChannel(service),socket_(service)
			{}
            virtual ~ASIOTcpClientChannel()
			{
                close();
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
                catch (const std::exception&e)
                {
                    PRINTFLOG(BL_DEBUG, "create asio point error what()=%s", e.what());
                    return HNW_BASE_ERR_CODE::HNW_BASE_BIND_ADDRESS_FAIL;
                }
                
                auto timer = add_timer(connect_time_out_ms_, [this]()
                    {
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CONNECT_TIME_OUT, "connect time out");
                    });
                auto self = shared_from_this();
                auto con_handle = [this, timer, self, remote](boost::system::error_code ec)
                {
                    del_timer(timer);
                    if (ec)
                    {
                        //减少打印
                        PRINTFLOG(BL_DEBUG, "async_connect error what()=%s", ec.message().c_str());
                        //上层关闭
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CONNECT_FAIL,"connect fail");
                        //close();
                        return;
                    }
                    bconn_ = true;
                    PRINTFLOG(BL_DEBUG, "tcp client[%I64d] is connected remote[%s:%d]", \
                        get_handle(), remote.ip.c_str(), remote.port);
                    EVENT_OK(HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH);
                    //接收数据
                    async_recv();
                };
                socket_.async_connect(remote_point, con_handle);
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
                        socket_.shutdown(socket_base::shutdown_send);
                        socket_.shutdown(socket_base::shutdown_receive);

                    }
                    catch (std::exception& e)
                    {
                        PRINTFLOG(BL_ERROR, "exception:%s", e.what());
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR,"close fail");
                        return HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR;
                    }
                    socket_.close();
                    bconn_ = false;
                    PRINTFLOG(BL_DEBUG, "TcpClientChannel[%I64d] closed ", handle_);
                    EVENT_OK(HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED);
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }
                return HNW_BASE_ERR_CODE::HNW_BASE_EMPTY_SOCKET;
            }
		
        public:
            //获取socket
            ip::tcp::socket& get_socket()
            {
                return socket_;
            }

            //is open
            virtual bool is_open()
            {
                //套接字已打开
                return socket_.is_open();
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
                //异步读数据
                socket_.async_read_some(boost::asio::buffer(buff, buff_len), handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
        private:
            ip::tcp::socket socket_;
		};
    }
}
#endif