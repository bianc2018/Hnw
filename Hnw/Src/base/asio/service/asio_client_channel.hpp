/*
	asio 客户端通道基类
*/

#ifndef HNW_ASIO_CLIENT_HPP_
#define HNW_ASIO_CLIENT_HPP_

#include "../../define/channel.hpp"

#include <boost/asio.hpp>

namespace hnw
{
    namespace boost_asio
    {
        using namespace boost::asio;
        
        typedef boost::asio::io_service io_service;

        typedef std::function<void(boost::system::error_code ec, std::size_t s)> AsioHandler;

        class ASIOClientChannel :public hnw::Channel
        {

        public:
            ASIOClientChannel(io_service& service)
                :service_(service), bconn_(false)
            {

            }
            virtual ~ASIOClientChannel()
            {
            }

        public:
           
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

        public:
         
            //is open
            virtual bool is_open()
            {
                return true;
            };

            //
            //接收之前的初始化
            virtual bool init_accept(std::shared_ptr<Channel> server, const NetPoint& local)
            {
                if (server)
                {
                    init(local);
                    set_shared_cb(server->get_shared_cb());
                    set_log_cb(server->get_log_cb());
                    set_event_cb(server->get_event_cb());
                    
                    server->config(GET_RECV_BUFF_SIZE, (void*)&recv_buff_size_, sizeof(recv_buff_size_));
                    server->config(GET_SEND_BUFF_SIZE, (void*)&send_buff_size_, sizeof(send_buff_size_));
                    return true;
                }
                else
                {
                    PRINTFLOG(BL_ERROR, "init_accept error,input server channel is nullptr");
                    return false;
                }
            };

            //接收回调成功 上层回调调用之前
            virtual bool before_accept()
            {
                bconn_ = true;
                return true;
            };

            //接收回调成功 上层回调调用之后
            virtual bool after_accept()
            {
                async_recv();
                return true;
            };

        protected:
            //异步发送数据
            virtual HNW_BASE_ERR_CODE async_send(std::shared_ptr<void> buff,
                size_t buff_len, size_t beg = 0, HNW_CALL complete = nullptr)
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
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN, "channel is not conn ,don't send");
                    return HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN;
                }
                auto timer = add_timer(send_time_out_ms_, [this]()
                    {
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_SEND_DATA_TIMEOUT, "send data time out");
                    });
                //写完数据后的回调函数
                auto self = shared_from_this();
                auto send_handler = \
                    [this, self, buff_len, timer, beg, buff, complete](boost::system::error_code ec, std::size_t s)
                {
                    del_timer(timer);
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
                        if (complete)
                            complete();
                        return;
                    }
                    //未写完
                    //计算剩下的字节
                    int now_len = buff_len - s;
                    PRINTFLOG(BL_DEBUG, "async_send  %p:len[%d]", buff.get(), s);
                    //继续写
                    async_send(buff, now_len, beg + s, complete);
                    return;
                };

                return write((char*)buff.get(), buff_len, send_handler);
            }

            //异步发送数据
            virtual HNW_BASE_ERR_CODE async_send(std::shared_ptr<void> buff, size_t buff_len,
                HNW_SEND_CB cb)
            {
                if (false == bconn_)
                {
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN, "channel is not conn ,don't send");
                    return HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN;
                }

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
                    read_data_size = cb((char*)buff.get(), buff_len);

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

            //异步接收数据
            virtual HNW_BASE_ERR_CODE async_recv(std::shared_ptr<void> buff = nullptr,
                size_t buff_len = 0)
            {
                if (false == bconn_)
                {
                    PRINTFLOG(BL_ERROR, "TcpClientChannel[%I64d] is not connected ", handle_);
                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN, "channel no connect ,dont recv data");
                    return HNW_BASE_ERR_CODE::HNW_BASE_CHANNEL_NOT_CONN;
                }

                if (nullptr == buff || buff_len != recv_buff_size_)
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
                auto timer = add_timer(recv_time_out_ms_, [this]()
                    {
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_RECV_DATA_TIMEOUT, "recv data time out");
                    });
                auto self = shared_from_this();
                auto recv_handler = [this, timer, self, buff, buff_len]\
                    (boost::system::error_code ec, size_t recv_len)
                {
                    del_timer(timer);
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
                return read((char*)buff.get(), buff_len, recv_handler);
            }

            //直接写发送数据
            virtual  HNW_BASE_ERR_CODE write(char* buff,
                size_t buff_len,
                AsioHandler handler
                )=0;

            //直接读取接收数据
            virtual  HNW_BASE_ERR_CODE read(char* buff,
                size_t buff_len,
                AsioHandler handler
            ) = 0;

            //新增定时器 
            std::shared_ptr<boost::asio::steady_timer> add_timer(size_t time_out,
                std::function<void()> call)
            {
                if (time_out == 0)
                    return nullptr;
                //定时器
                auto timer =\
                    std::make_shared<boost::asio::steady_timer>(service_);

                //设置超时
                auto time_out_handler = \
                    [this, call](boost::system::error_code ec)
                {
                    
                    if (!ec)
                    {
                        if(call)
                            call();
                    }
                };

                timer->expires_from_now(std::chrono::milliseconds(time_out));
                timer->async_wait(time_out_handler);
                return timer;
            }

            //删除定时器
            int del_timer(std::shared_ptr<boost::asio::steady_timer> timer)
            {
                if (nullptr == timer)
                    return -1;
                boost::system::error_code ec;
                timer->cancel(ec);
                return ec.value();
            }
        protected:
            //是否链接？
            std::atomic_bool bconn_;

            io_service& service_;
        };
    }
}
#endif