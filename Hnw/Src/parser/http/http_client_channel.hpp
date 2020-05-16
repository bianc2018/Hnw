/*
    http 客户端
    hql 2020 04/14
*/
#ifndef HNW_ASIO_HTTP_CLIENT_HPP_
#define HNW_ASIO_HTTP_CLIENT_HPP_
#include "tcp_client_channel.hpp"

#include "../parser/http/http_request_parser.hpp"
#include "../parser/http/http_response_parser.hpp"

#include "protocol_base_channel.hpp"

namespace hnw
{
    namespace boost_asio
    {
        //解析器类型
        enum class EMHttpParserType
        {
            REQUEST,
            RESPONSE,
        };

        
        //class HttpClientChannel :public ProtocolClientBaseChannel<ASIOTcpClientChannel>
        class HttpClientChannel :public ProtocolClientBaseChannel<ASIOTcpClientChannel>
        {

        public:
            HttpClientChannel(io_service& service)
                :ProtocolClientBaseChannel<ASIOTcpClientChannel>(service)
            {
                set_recv_parser(hnw::boost_asio::EMHttpParserType::RESPONSE);
            }
            virtual ~HttpClientChannel()
            {

            }

        public:
            bool set_recv_parser(EMHttpParserType type)
            {
                switch (type)
                {
                case hnw::boost_asio::EMHttpParserType::REQUEST:
                    set_recv_parser_ptr(std::make_shared<parser::HttpRequestParser>(get_handle()));
                    break;
                case hnw::boost_asio::EMHttpParserType::RESPONSE:
                    set_recv_parser_ptr(std::make_shared<parser::HttpResponseParser>(get_handle()));
                    break;
                default:
                    return false;
                    break;
                }
                return true;
            }
           /* bool set_send_parser(EMHttpParserType type)
            {
                switch (type)
                {
                case hnw::boost_asio::EMHttpParserType::REQUEST:
                    set_send_parser_ptr(std::make_shared<parser::HttpRequestParser>());
                    break;
                case hnw::boost_asio::EMHttpParserType::RESPONSE:
                    set_send_parser_ptr(std::make_shared<parser::HttpResponseParser>());
                    break;
                default:
                    return false;
                    break;
                }
                return true;
            }*/
        };

        //class HttpClientChannel :public hnw::boost_asio::ASIOTcpClientChannel
        //{
        //    
        //public:
        //    HttpClientChannel(io_service& service)
        //        :ASIOTcpClientChannel(service),
        //        recv_parser_(std::make_shared<parser::HttpResponseParser>()),//默认回复解析
        //        send_parser_(std::make_shared<parser::HttpRequestParser>())
        //    {}
        //    virtual ~HttpClientChannel()
        //    {
        //       
        //    }

        //public:
        //    
        //    //设置事件回调
        ////    virtual HNW_BASE_ERR_CODE set_event_cb(HNW_EVENT_CB cb)
        //    {
        //        event_cb_ = [cb,this](std::int64_t handle, \
        //            int tt, std::shared_ptr<void> event_data)mutable
        //        {
        //            HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)tt;

        //            if (HNW_BASE_EVENT_TYPE::HNW_BASE_RECV_DATA == type)
        //            {
        //                auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
        //                if (recv_parser_)
        //                {
        //                    auto ret = recv_parser_->input_data((const unsigned char*)data->buff, data->buff_len);
        //                    if (0 == ret)
        //                    {
        //                        auto msg = recv_parser_->get_one_message();
        //                        if (msg)
        //                        {
        //                            EVENT_CB(recv_parser_->get_event_type(), msg);
        //                        }
        //                        return;
        //                        
        //                    }
        //                    else
        //                    {
        //                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_HTTP_PARSER_FAIL, "http parser error,ret="+std::to_string(ret));
        //                        return ;
        //                    }
        //                }
        //                else
        //                {
        //                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_HTTP_EMPTY_PARSER, "http recv_parser_ is empty");
        //                    return;
        //                }
        //            }
        //            
        //            //其他直接回调
        //            if (cb)
        //            {
        //                cb(handle, tt, event_data);
        //            }
        //        };
        //        return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        //    }
        //    
        //    //发送数据
        //    virtual HNW_BASE_ERR_CODE send(std::shared_ptr<void> message, size_t message_size)
        //    {
        //        if (send_parser_)
        //        {
        //            if (message&&message_size!=0)
        //            {
        //                auto b = PTR_CAST(HnwBaseEvent, message);
        //                if ((int)b->type == send_parser_->get_event_type())
        //                {
        //                    int ret = -1;
        //                    message =
        //                        send_parser_->struct_raw_package(message,
        //                             message_size, ret);
        //                    if (0 == ret)
        //                        return async_send(message, message_size);
        //                    else
        //                    {
        //                        EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_HTTP_STRUCT_MESSAGE_FAIL\
        //                            , "http struce message fail ret=" + std::to_string(ret));
        //                        return HNW_BASE_ERR_CODE::HNW_HTTP_STRUCT_MESSAGE_FAIL;
        //                    }
        //                }
        //                else
        //                {
        //                    EVENT_ERR_CB(HNW_BASE_ERR_CODE::HNW_HTTP_BAD_MESSAGE\
        //                        , "http bad message");
        //                    return HNW_BASE_ERR_CODE::HNW_HTTP_BAD_MESSAGE;
        //                }
        //            }
        //            return HNW_BASE_ERR_CODE::HNW_BASE_OK;
        //        }
        //        return HNW_BASE_ERR_CODE::HNW_HTTP_EMPTY_PARSER;
        //    }
        //public:
        //    bool set_recv_parser(EMHttpParserType type)
        //    {
        //        switch (type)
        //        {
        //        case hnw::boost_asio::EMHttpParserType::REQUEST:
        //            recv_parser_ = std::make_shared<parser::HttpRequestParser>();
        //            break;
        //        case hnw::boost_asio::EMHttpParserType::RESPONSE:
        //            recv_parser_ = std::make_shared<parser::HttpResponseParser>();
        //            break;
        //        default:
        //            return false;
        //            break;
        //        }
        //        return true;
        //    }
        //    bool set_send_parser(EMHttpParserType type)
        //    {
        //        switch (type)
        //        {
        //        case hnw::boost_asio::EMHttpParserType::REQUEST:
        //            send_parser_ = std::make_shared<parser::HttpRequestParser>();
        //            break;
        //        case hnw::boost_asio::EMHttpParserType::RESPONSE:
        //            send_parser_ = std::make_shared<parser::HttpResponseParser>();
        //            break;
        //        default:
        //            return false;
        //            break;
        //        }
        //        return true;
        //    }
        //private:
        //    //接收解析器
        //    std::shared_ptr<parser::ParserBase> recv_parser_,send_parser_;
        //};
    }
}
#endif
