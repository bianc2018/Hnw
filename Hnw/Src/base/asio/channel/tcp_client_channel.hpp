#ifndef HNW_ASIO_TCP_CLIENT_HPP_
#define HNW_ASIO_TCP_CLIENT_HPP_

#include "../../define/channel.hpp"

#include <boost/asio.hpp>

namespace hnw
{
    namespace boost_asio
    {
        using namespace boost::asio;
        typedef boost::asio::io_service io_service;
		class ASIOTcpClientChannel:public hnw::Channel
		{
			
		public:
			ASIOTcpClientChannel(io_service &service)
                :service_(service),socket_(service), bconn_(false)
			{}
            virtual ~ASIOTcpClientChannel()
			{
                close();
            }

		public:
            //��ʼ��
            virtual HNW_BASE_ERR_CODE init(const NetPoint& local)
            {
                if (INVAILD_HANDLE == handle_)
                {
                    PRINTFLOG(BL_ERROR, "invaild handle");
                    return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
                }
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //����
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
                

                auto self = shared_from_this();
                auto con_handle = [this, self, remote](boost::system::error_code ec)
                {
                    if (ec)
                    {
                        //���ٴ�ӡ
                        PRINTFLOG(BL_DEBUG, "async_connect error what()=%s", ec.message().c_str());
                        //�ϲ�ر�
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CONNECT_FAIL,"connect fail");
                        //close();
                        return;
                    }
                    bconn_ = true;
                    PRINTFLOG(BL_DEBUG, "tcp client[%I64d] is connected remote[%s:%d]", \
                        get_handle(), remote.ip.c_str(), remote.port);
                    EVENT_OK(HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH);
                    //��������
                    async_recv();
                };
                socket_.async_connect(remote_point, con_handle);
                PRINTFLOG(BL_DEBUG, "tcp client[%I64d] begin connect to remote[%s:%d]", \
                    get_handle(), remote.ip.c_str(), remote.port);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //��������
            virtual HNW_BASE_ERR_CODE send(std::shared_ptr<void> message, size_t message_size)
            {
                return async_send(message, message_size);
            }

            //��������
            virtual HNW_BASE_ERR_CODE send(HNW_SEND_CB cb)
            {
               return async_send(nullptr,0,cb);
            }

            //�ر�һ��ͨ��
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
            //��ȡsocket
            ip::tcp::socket& get_socket()
            {
                return socket_;
            }

            //is open
            bool is_open()
            {
                //�׽����Ѵ�
                return socket_.is_open();
            }
            bool before_accept()
            {
                //�����ѽ���
                bconn_ = true;
                return true;
            }
            bool after_accept()
            {
               
                //bconn_ = true;
                //EVENT_OK_CB(BNS_NET_EVENT_TYPE::BNS_CONNECT_ESTABLISH);
                //��������
                async_recv();
                return true;
            }

        protected:
            //�첽��������
            HNW_BASE_ERR_CODE async_send(std::shared_ptr<void> buff, 
                size_t buff_len, size_t beg = 0,HNW_CALL complete=nullptr)
            {
                if (buff_len == 0 || nullptr == buff)
                {
                    EVENT_SEND_CB(buff, buff_len);
                    if (complete)
                        complete();
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;
                }

                if (false == bconn_)
                {
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN,"channel is not conn ,don't send");
                    return HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN;
                }
                //д�����ݺ�Ļص�����
                auto self = shared_from_this();
                auto send_handler = \
                    [this, self, buff_len, beg, buff,complete](boost::system::error_code ec, std::size_t s)
                {

                    if (ec)
                    {
                        PRINTFLOG(BL_DEBUG, "async_send error what()=%s", ec.message().c_str());
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_SEND_DATA_FAIL,"send data fail");
                        //close();
                        return;
                    }
                    //д����
                    if (buff_len <= s)
                    {
                        //�����첽д,ȡ��һ��buff
                       // if (on_complate_)
                        //    on_complate_(true);
                        EVENT_SEND_CB(buff, buff_len);
                        if (complete)
                            complete();
                        return;
                    }
                    //δд��
                    //����ʣ�µ��ֽ�
                    int now_len = buff_len - s;
                    PRINTFLOG(BL_DEBUG, "async_send  %p:len[%d]", buff.get(), s);
                    //����д
                    async_send(buff, now_len, beg + s, complete);
                    return;
                };

                socket_.async_write_some(boost::asio::buffer((char*)buff.get() + beg, buff_len), \
                    send_handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //�첽��������
            HNW_BASE_ERR_CODE async_send(std::shared_ptr<void> buff, size_t buff_len,
                HNW_SEND_CB cb)
            {
                if (false == bconn_)
                {
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN, "channel is not conn ,don't send");
                    return HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN;
                }

                //���뻺��
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
                
                //��ȡ����
                size_t read_data_size = 0;
                if (cb)
                    read_data_size = cb(buff, buff_len);

                //���
                if (0 == read_data_size)
                    return HNW_BASE_ERR_CODE::HNW_BASE_OK;

                //����
                return async_send(buff, read_data_size,0,[this,buff, buff_len,cb]()mutable
                    {
                        //��������
                        async_send(buff, buff_len, cb);
                    });
            }
            //�첽��������
            HNW_BASE_ERR_CODE async_recv(std::shared_ptr<void> buff = nullptr, size_t buff_len = 0)
            {
                if (false == bconn_)
                {
                    PRINTFLOG(BL_ERROR, "TcpClientChannel[%I64d] is not connected ", handle_);
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN,"channel no connect ,dont recv data");
                    return HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN;
                }

                if (nullptr == buff||buff_len!=recv_buff_size_)
                {
                    buff_len = recv_buff_size_;
                    buff = MAKE_SHARED(recv_buff_size_);
                    if (nullptr == buff)
                    {
                        PRINTFLOG(BL_ERROR, "get cache error ch :%I64d", handle_);
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL,"not alloc data");
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
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_RECV_DATA_FAIL,"cannt recv data");
                        //close();
                        return;
                    }
                    PRINTFLOG(BL_DEBUG, "async_read  %p:len[%d]", buff.get(), recv_len);
                    EVENT_RECV_CB(buff, recv_len);
                    //�ظ���������
                    async_recv(buff, buff_len);
                };
                socket_.async_read_some(boost::asio::buffer(buff.get(), buff_len), recv_handler);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
        private:
            //�Ƿ����ӣ�
            std::atomic_bool bconn_;

            ip::tcp::socket socket_;

            io_service& service_;
		};
    }
}
#endif