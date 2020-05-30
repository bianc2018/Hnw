#include "hnw_http.h"

#include "hnw_base.h"

#include "parser/http_request_parser.hpp"
#include "parser/http_response_parser.hpp"

//一个http Session回调
//struct HttpSession
//{
//    HNW_HANDLE handle=HNW_INVALID_HANDLE;
//    std::shared_ptr<hnw::parser::ParserBase>  recv_parser=nullptr;
//    //
//    HNW_EVENT_CB cb;
//
//};

static HNW_EVENT_CB bind_parser(std::shared_ptr<hnw::parser::ParserBase>  recv_parser,
    HNW_EVENT_CB cb)
{
    return [cb, recv_parser](std::int64_t handle, \
        int tt, std::shared_ptr<void> event_data)mutable
    {
        HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)tt;

        if (HNW_BASE_EVENT_TYPE::HNW_BASE_RECV_DATA == type && recv_parser)
        {
            auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
           // printf("\n%s\n", std::string(data->buff,data->buff_len).c_str());
            auto t = clock();
            auto ret = recv_parser->input_data((unsigned char*)data->buff,
                data->buff_len);
            printf("recv parser %u used %d\n", data->buff_len, clock() - t);
        }
        if (HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT == type)
        {
            auto data = PTR_CAST(HnwBaseAcceptEvent, event_data);
            auto parser = std::make_shared<hnw::parser::HttpRequestParser>(data->client);
            parser->set_event_cb(cb);
            parser->set_log_cb(hnw::default_log_print);
            HnwBase_SetEvntCB(data->client, bind_parser(parser, cb));
           

        }
        //其他直接回调
        if (cb)
        {
            cb(handle, tt, event_data);
        }
    };
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_SetLogCB(HNW_HTTP_LOG_CB cb)
{
	return HnwBase_SetLogCB(HNW_LOG_CB(cb));
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Start(const HttpParam& param, HNW_EVENT_CB cb, HNW_HANDLE& handle)
{
    hnw::parser::UrlParam url_param;
    hnw::parser::parser_url(param.host, url_param);
    HNW_CHANNEL_TYPE chn_type = HNW_CHANNEL_TYPE::TCP_SERVER;
    //默认 http
    if ("" == url_param.protocol)
    {
        url_param.protocol = "http";
        if ("" == url_param.port)
            url_param.port = "80";
    }

    if ("http" == url_param.protocol)
    {
        if (param.peer_type == Server)
            chn_type = HNW_CHANNEL_TYPE::TCP_SERVER;
        else if (param.peer_type == Client)
            chn_type = HNW_CHANNEL_TYPE::TCP_CLIENT;
        else
            return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
    }
    else if ("https" == url_param.protocol)
    {
        if (param.peer_type == Server)
            chn_type = HNW_CHANNEL_TYPE::SSL_SERVER;
        else if (param.peer_type == Client)
            chn_type = HNW_CHANNEL_TYPE::SSL_CLIENT;
        else
            return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
    }

    //dns
    std::vector<NetPoint> point_ver;
    auto ret = HnwBase_QueryDNS(url_param.host, point_ver, url_param.protocol);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
    {
        HnwBase_Close(handle);
        return ret;
    }
    if (point_ver.size() == 0)
        return HNW_BASE_ERR_CODE::HNW_HTTP_HOST_IS_IVAILD;

    NetPoint point = point_ver[0];
    //非空
    if (!url_param.port.empty())
    {
        try
        {
            point.port = std::stoi(url_param.port);
        }
        catch (const std::exception&)
        {
            return HNW_BASE_ERR_CODE::HNW_HTTP_HOST_IS_IVAILD;
        }

    }

    ret = HnwBase_Add_Channnel(chn_type, point, handle);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
    {
        HnwBase_Close(handle);
        return ret;
    }
   
    //注册回调
    std::shared_ptr<hnw::parser::ParserBase>  recv_parser = nullptr;
    
    if (param.peer_type == Client)
    {
        recv_parser = std::make_shared<hnw::parser::HttpResponseParser>(handle);
        recv_parser->set_event_cb(cb);
    }

    auto event_cb = bind_parser(recv_parser, cb);
    HnwBase_SetEvntCB(handle, event_cb);

    //设置证书
    /*
    HnwBase_Config(handle,SET_SSL_SERVER_CERT_FILE_PATH,
        (void*)cert_file.c_str(),cert_file.size());
     return  HnwBase_Config(handle, SET_SSL_SERVER_PRI_KEY_FILE_PATH,
         (void*)pri_key_file.c_str(), pri_key_file.size());
    */
    if(param.cert_file.size()!=0)
        HnwBase_Config(handle, SET_SSL_SERVER_CERT_FILE_PATH,
            (void*)param.cert_file.c_str(), param.cert_file.size());
    if (param.pri_key_file.size() != 0)
        HnwBase_Config(handle, SET_SSL_SERVER_PRI_KEY_FILE_PATH,
            (void*)param.pri_key_file.c_str(), param.pri_key_file.size());
    if (param.temp_dh_file.size() != 0)
        HnwBase_Config(handle, SET_SSL_SERVER_TEMP_DH_FILE_PATH,
            (void*)param.temp_dh_file.c_str(), param.temp_dh_file.size());
    HnwBase_Config(handle, SET_SERVER_ACCEPT_NUM,
        (void*)&(param.accept_num), sizeof(param.accept_num));
    //SET_SSL_SERVER_TEMP_DH_FILE_PATH

    if (param.peer_type == Server)
        ret = HnwBase_Accept(handle);
    else if (param.peer_type == Client)
        ret = HnwBase_Connect(handle, point);
    else
        ret=HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;

    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
    {
        HnwBase_Close(handle);
        return ret;
    }
    return ret;
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Request(HNW_HANDLE handle, std::shared_ptr<HnwHttpRequest> req)
{
    auto ret = HNW_BASE_ERR_CODE::HNW_BASE_OK;
    auto str = hnw::parser::HttpRequestParser::struct_raw_package(req, ret);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
    {
        return HnwBase_Send_String(handle, str);
    }
    return ret;
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Response(HNW_HANDLE handle, std::shared_ptr<HnwHttpResponse> res)
{
    auto ret = HNW_BASE_ERR_CODE::HNW_BASE_OK;
    auto str = hnw::parser::HttpResponseParser::struct_raw_package(res, ret);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
    {
        return HnwBase_Send_String(handle, str);
    }
    return ret;
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Close(HNW_HANDLE handle)
{
    return  HnwBase_Close(handle);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Config(HNW_HANDLE handle, int config_type, void* data, size_t data_len)
{
    return HnwBase_Config(handle,config_type,data,data_len);
}


