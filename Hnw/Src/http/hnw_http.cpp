#include "hnw_http.h"

#include "hnw_base.h"

#include "asio/parser/http/http_request_parser.hpp"
#include "asio/parser/http/http_response_parser.hpp"

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartServer(unsigned int local_port, HNW_EVENT_CB cb, HNW_HANDLE& handle)
{
    NetPoint local;
    local.ip = "0.0.0.0";
    local.port = local_port;
    handle = HNW_INVALID_HANDLE;
    auto ret =  HnwBase_Add_Channnel(HNW_CHANNEL_TYPE::HTTP_SERVER, local, handle);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
    {
        HnwBase_Close(handle);
        return ret;
    }

    HnwBase_SetEvntCB(handle, cb);

    ret = HnwBase_Accept(handle);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
    {
        HnwBase_Close(handle);
        return ret;
    }
    return ret;
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSession(const std::string& host, HNW_EVENT_CB cb, HNW_HANDLE& handle)
{
    //创建会话
    auto ret = HnwBase_Add_Channnel(HNW_CHANNEL_TYPE::HTTP_CLIENT, NET_INVALID_POINT, handle);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
    {
        HnwBase_Close(handle);
        return ret;
    }

    HnwBase_SetEvntCB(handle, cb);

    //dns
    std::vector<NetPoint> point_ver;
    ret = HnwBase_QueryDNS(host, point_ver);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
    {
        HnwBase_Close(handle);
        return ret;
    }

    //连接
    for (auto p : point_ver)
    {
       return HnwBase_Connect(handle, p);
    }
    return HNW_BASE_ERR_CODE::HNW_HTTP_HOST_IS_IVAILD;
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Request(HNW_HANDLE handle, std::shared_ptr<HnwHttpRequest> req)
{
    auto ret = HNW_BASE_ERR_CODE::HNW_BASE_OK;
    auto str = hnw::parser::HttpRequestParser::struct_raw_package(req, ret);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
    {
        return HnwBase_Send_String(handle, str);
    }
    return ret;
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Response(HNW_HANDLE handle, std::shared_ptr<HnwHttpResponse> res)
{
    auto ret = HNW_BASE_ERR_CODE::HNW_BASE_OK;
    auto str = hnw::parser::HttpResponseParser::struct_raw_package(res, ret);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
    {
        return HnwBase_Send_String(handle, str);
    }
    return ret;
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Close(HNW_HANDLE handle)
{
    return  HnwBase_Close(handle);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSimpleServer(unsigned int local_port, HTTP_SERVER_CB cb)
{
    HNW_HANDLE handle;
    auto event_cb = [cb](std::int64_t handle, \
        int t,
        std::shared_ptr<void> event_data)
    {
        HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)t;
      
        if (HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT == type)
        {
            //auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
            printf("handle:%lld accept  \n", handle);
        }
        else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_REQUEST == type)
        {
            auto request = PTR_CAST(HnwHttpRequest, event_data);
            
            auto response = std::make_shared<HnwHttpResponse>();

            response->body = request->body;

            if (cb)
            {
                cb(request, response);
            }

            HnwHttp_Response(handle, response);
        }
    };
    //初始化通道
    auto ret = HnwHttp_StartServer(8080, event_cb, handle);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
    {
        printf("HnwHttp_StartServer ok,handle=%lld\n", handle);
    }
    else
    {
        printf("HnwHttp_StartServer fail,ret =%d \n", ret);
        return ret;
    }
    return  ret;
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StartSimpleSession(const std::string host, HTTP_CLIENT_CB cb)
{
    HNW_HANDLE handle;
    auto event_cb = [cb](std::int64_t handle, \
        int t,
        std::shared_ptr<void> event_data)
    {
        HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)t;

        if (HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT == type)
        {
            //auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
            printf("handle:%lld accept  \n", handle);
        }
        else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_RESPONSE == type)
        {
            auto response = PTR_CAST( HnwHttpResponse, event_data);

            auto request = std::make_shared<HnwHttpRequest>();

            response->body = request->body;

            if (cb)
            {
                cb(response, request);
            }

            HnwHttp_Request(handle, request);
        }
    };
    //初始化通道
    auto ret = HnwHttp_StartSession(host, event_cb, handle);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
    {
        printf("HnwHttp_StartServer ok,handle=%lld\n", handle);
    }
    else
    {
        printf("HnwHttp_StartServer fail,ret =%d \n", ret);
        return ret;
    }
    return  ret;
}

//HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Get(const std::string& url,
//    std::string& body)
//{
//    auto request = std::make_shared<HnwHttpRequest>();
//    
//    request->url = url;
//
//    HNW_HANDLE handle;
//    auto event_cb = [body](std::int64_t handle, \
//        int t,
//        std::shared_ptr<void> event_data)
//    {
//        HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)t;
//
//        if (HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT == type)
//        {
//            //auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
//            printf("handle:%lld accept  \n", handle);
//        }
//        else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_RESPONSE == type)
//        {
//            auto response = PTR_CAST(HnwHttpResponse, event_data);
//
//            auto request = std::make_shared<HnwHttpRequest>();
//
//            response->body = request->body;
//
//            if (cb)
//            {
//                cb(response, request);
//            }
//
//            HnwHttp_Request(handle, request);
//        }
//    };
//    //初始化通道
//    auto ret = HnwHttp_StartSession(host, event_cb, handle);
//    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
//    {
//        printf("HnwHttp_StartServer ok,handle=%lld\n", handle);
//    }
//    else
//    {
//        printf("HnwHttp_StartServer fail,ret =%d \n", ret);
//        return ret;
//    }
//    return  ret;
//}
//
//HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Post(const std::string& url, const std::string& msg, std::string& response)
//{
//    return HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE();
//}
