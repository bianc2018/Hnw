#ifndef HNW_ASIO_TIMER_HPP_
#define HNW_ASIO_TIMER_HPP_

#include "../service/asio_client_channel.hpp"

#include <boost/asio.hpp>

namespace hnw
{
    namespace boost_asio
    {
        using namespace boost::asio;
        class ASIOTimerChannel :public ASIOClientChannel
        {

            typedef boost::asio::io_service io_service;
        public:
            ASIOTimerChannel(io_service& service)
                :ASIOClientChannel(service)
                , time_out_ms_(0), timer_(nullptr), call_(nullptr)
            {}
            virtual ~ASIOTimerChannel()
            {
                close();
            }

        public:
            //初始化
            virtual HNW_BASE_ERR_CODE init(const NetPoint& local)
            {
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //连接
            virtual HNW_BASE_ERR_CODE connect(const NetPoint& remote)
            {
                if (timer_)
                    del_timer(timer_);
                timer_ = add_timer(time_out_ms_, [this]() 
                    {
                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_BASE_TIMER_TRIGGER, "timer trigger");
                        if (call_)
                            call_();
                    });
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //关闭一个通道
            virtual HNW_BASE_ERR_CODE close()
            {
                del_timer(timer_);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //发送数据
            virtual HNW_BASE_ERR_CODE send(std::shared_ptr<void> message, size_t message_size)
            {
                return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
            }

            //发送数据
            virtual HNW_BASE_ERR_CODE send(HNW_SEND_CB cb)
            {
                return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
            }

            //设置
            virtual HNW_BASE_ERR_CODE set_time_out(size_t ms)
            {
                time_out_ms_ = ms;
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
            //设置
            virtual HNW_BASE_ERR_CODE set_cb(hnw::HNW_CALL cb)
            {
                call_ = cb;
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
            virtual HNW_BASE_ERR_CODE config(int config_type, void* data, size_t data_len)
            {
                if (SET_TIMER_TIME_OUT_MS == config_type)
                {
                    if (data)
                    {
                        auto p = (size_t*)data;
                        
                        return set_time_out(*p);
                    }
                    else
                    {
                        PRINTFLOG(BL_ERROR, "SET_TIMER_TIME_OUT_MS config error data must be size_t*");
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                    }
                }
                else if (SET_TIMER_CB == config_type)
                {
                    if (data)
                    {
                        auto p = (hnw::HNW_CALL*)data;
                        return set_cb(*p);
                    }
                    else
                    {
                        PRINTFLOG(BL_ERROR, "SET_TIMER_CB config error data must be hnw::HNW_CALL*");
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                    }
                }
                
                return ASIOClientChannel::config(config_type, data, data_len);
            }
        private:
            //直接写发送数据
            virtual  HNW_BASE_ERR_CODE write(char* buff,
                size_t buff_len,
                AsioHandler handler
            )
            {
                return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
            }

            //直接读取接收数据
            virtual  HNW_BASE_ERR_CODE read(char* buff,
                size_t buff_len,
                AsioHandler handler
            )
            {
                return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
            }
        private:
            size_t time_out_ms_;
            std::shared_ptr<boost::asio::steady_timer> timer_;
            //超时回调
            hnw::HNW_CALL call_;
        };
    }
}
#endif