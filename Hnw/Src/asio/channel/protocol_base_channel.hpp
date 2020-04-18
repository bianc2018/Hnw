/*
    协议基类
    hql 2020 04/14
*/
#ifndef HNW_PROTOCOL_BASE_HPP_
#define HNW_PROTOCOL_BASE_HPP_

#include "../../define/parser.hpp"

namespace hnw
{
    namespace boost_asio
    {
        template<typename Protocol>
        class ProtocolClientBaseChannel :public Protocol
        {

        public:
            ProtocolClientBaseChannel(io_service& service)
                :ASIOTcpClientChannel(service),
                recv_parser_(nullptr)//,//默认回复解析
                //send_parser_(nullptr)
            {}
            virtual ~ProtocolClientBaseChannel()
            {

            }

        public:

            //设置事件回调
            virtual HNW_BASE_ERR_CODE set_event_cb(HNW_EVENT_CB cb)
            {
                event_cb_ = [cb, this](std::int64_t handle, \
                    int tt, std::shared_ptr<void> event_data)mutable
                {
                    HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)tt;

                    if (HNW_BASE_EVENT_TYPE::HNW_BASE_RECV_DATA == type)
                    {
                        auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
                        if (recv_parser_)
                        {
                            auto ret = recv_parser_->input_data((const unsigned char*)data->buff, data->buff_len);
                            if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                            {
                                EVENT_ERR_CB(ret, "http parser error,ret=" + std::to_string((int)ret));
                                return;
                            }
                            return;
                        }
                        else
                        {
                            EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_HTTP_EMPTY_PARSER, "http recv_parser_ is empty");
                            return;
                        }
                    }
                    else
                    {
                        //其他直接回调
                        if (cb)
                        {
                            cb(handle, tt, event_data);
                        }
                    }
                };
                if (recv_parser_)
                    recv_parser_->set_event_cb(event_cb_);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //日志回调
            virtual HNW_BASE_ERR_CODE set_log_cb(HNW_LOG_CB cb)
            {
                log_cb_ = cb;
                if (recv_parser_)
                    recv_parser_->set_log_cb(log_cb_);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //buff申请回调
            virtual HNW_BASE_ERR_CODE set_shared_cb(HNW_MAKE_SHARED_CB cb)
            {
                make_shared_ = cb;
                if (recv_parser_)
                    recv_parser_->set_make_shared_cb(make_shared_);
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }
        public:
            bool set_recv_parser_ptr(std::shared_ptr<parser::ParserBase> p)
            {
                recv_parser_ = p;
                recv_parser_->set_log_cb(log_cb_);
                recv_parser_->set_make_shared_cb(make_shared_);
                recv_parser_->set_event_cb(event_cb_);
                return true;
            }
            //bool set_send_parser_ptr(std::shared_ptr<parser::ParserBase> p)
            //{
            //    send_parser_ = p;
            //    send_parser_->set_log_cb(log_cb_);
            //    send_parser_->set_make_shared_cb(make_shared_);
            //    recv_parser_->set_event_cb(event_cb_);
            //    return true;
            //}
        private:
            //接收解析器
            std::shared_ptr<parser::ParserBase> recv_parser_/*, send_parser_*/;
        };
    }
}
#endif
