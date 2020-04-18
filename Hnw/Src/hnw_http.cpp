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
