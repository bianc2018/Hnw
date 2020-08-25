#include "hnw_http.h"

#include "hnw_base.h"
#include "service/http_service.hpp"
static hnw::http::Service service;

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_SetLogCB(HNW_HTTP_LOG_CB cb)
{
    service.set_log_cb(cb);
    hnw::hnw_g_log_cb = cb;
	return HnwBase_SetLogCB(HNW_LOG_CB(cb));
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Start(const HttpParam& param, HNW_EVENT_CB cb, HNW_HANDLE& handle)
{
    return service.start(param, cb, handle);
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_GenerateRequest(HNW_HANDLE handle, SPHnwHttpRequest& req)
{
    req = hnw::http::RequestImpl::generate();
    if(req)
        return  HNW_BASE_ERR_CODE::HNW_BASE_OK;
    else
        return  HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_GenerateResponse(HNW_HANDLE handle, SPHnwHttpResponse& res)
{
    res = hnw::http::ResponseImpl::generate();
    if (res)
        return  HNW_BASE_ERR_CODE::HNW_BASE_OK;
    else
        return  HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_GenerateHead(HNW_HANDLE handle, SPHnwHttpHead& head)
{
    head = hnw::http::HeadImpl::generate();
    if (head)
        return  HNW_BASE_ERR_CODE::HNW_BASE_OK;
    else
        return  HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
}


HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Request(HNW_HANDLE handle, SPHnwHttpRequest req)
{
    auto sp = service.get_session(handle);
    if (sp)
    {
        return sp->send(req);
    }
    return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Response(HNW_HANDLE handle, SPHnwHttpResponse res)
{
    auto sp = service.get_session(handle);
    if (sp)
    {
        return sp->send(res);
    }
    return HNW_BASE_ERR_CODE::HNW_BASE_INVAILD_HANDLE;
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Close(HNW_HANDLE handle)
{
    return  HnwBase_Close(handle);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_Config(HNW_HANDLE handle, int config_type, void* data, size_t data_len)
{
    return HnwBase_Config(handle,config_type,data,data_len);
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_RawRequest(HNW_HANDLE handle,
    const std::string& method,
    const std::string& uri,
    const std::string& data,
    SPHnwHttpHead ext_head)
{
    return service.raw_request(handle, method, uri, data, ext_head);
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_RawResponse(HNW_HANDLE handle, 
    const std::string& code, const std::string& data, SPHnwHttpHead ext_head)
{
    return service.raw_response(handle, code,  data, ext_head);
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_StatusResponse(HNW_HANDLE handle, const std::string& code)
{
    return HnwHttp_RawResponse(handle,code,"");
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_FileRequest(HNW_HANDLE handle,
    const std::string& method, const std::string& uri,
    const std::string& filepath, SPHnwHttpHead ext_head)
{
    return service.file_request(handle,method,uri,filepath, ext_head);
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_FileResponse(HNW_HANDLE handle,
    const std::string& code, const std::string& filepath, SPHnwHttpHead ext_head)
{
    return service.file_response(handle,code,filepath,ext_head);
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_RecvFileRequest(SPHnwHttpRequest request, const std::string& filepath,
    int flag )
{
    return service.recv_file_request(request,filepath, flag);
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_RecvFileResponse(SPHnwHttpResponse response, const std::string& filepath,
    int flag)
{
    return service.recv_file_response(response, filepath,flag);
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_GetResourceStatus(const std::string& url, HTTP_RES_STATUS_CB cb)
{
    return service.get_resource_status(url,cb);
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwHttp_DownLoad(const std::string& url,
    const std::string& output, HTTP_DOWNLOAD_CB cb, size_t con_num, 
    const HttpRange range, int file_flag)
{
    return service.download(url, output,cb,con_num,range,file_flag);
}



//uri ±à½âÂë
HNW_HTTP_EXPORT_SYMBOLS std::string HnwUtil_UrlEncode(const std::string& src)
{
    return hnw::util::encode_url(src);
}
HNW_HTTP_EXPORT_SYMBOLS std::string HnwUtil_UrlDecode(const std::string& src)
{
    return hnw::util::decode_url(src);
}

//utf-8ºÍANSI×ª»»
HNW_HTTP_EXPORT_SYMBOLS std::string HnwUtil_Utf8ToAnsi(const std::string& src)
{
    return hnw::util::utf8_to_ansi(src);
}
HNW_HTTP_EXPORT_SYMBOLS std::string HnwUtil_AnsiToUtf8(const std::string& src)
{
    return hnw::util::ansi_to_utf8(src);
}

HNW_HTTP_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwUtil_Md5(const std::string& src, std::string& dst)
{
    dst = hnw::util::md5(src);
    if(dst.empty())
        return HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR;
    return  HNW_BASE_ERR_CODE::HNW_BASE_OK;
}
