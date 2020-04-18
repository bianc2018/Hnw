#include "hnw_base.h"

#ifdef USE_BOOST_ASIO
#include "asio/service/asio_service.hpp"
static hnw::boost_asio::AsioService service;
#endif

//HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Init()
//{
//    return  service.init();
//}
//
//HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_DInit()
//{
//    return  service.de_init();
//}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Add_Channnel(HNW_CHANNEL_TYPE type, const NetPoint& local, HNW_HANDLE& handle)
{
    return  service.add_channel(type,local,handle);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Connect(HNW_HANDLE handle, const NetPoint& remote)
{
    return  service.connect(handle,remote);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Accept(HNW_HANDLE handle)
{
    
    return  service.accept(handle);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Close(HNW_HANDLE handle)
{
    return  service.close(handle);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Send(HNW_HANDLE handle, std::shared_ptr<char> message, size_t message_size)
{
    return service.send(handle,message,message_size);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Send_String(HNW_HANDLE handle, const std::string& message)
{
    return  service.send_string(handle,message);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetEvntCB(HNW_HANDLE handle, HNW_EVENT_CB cb)
{
    return  service.set_event_cb(handle,cb);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetLogCB(HNW_LOG_CB cb)
{
    return  service.set_log_cb(cb);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetMakeSharedCB(HNW_MAKE_SHARED_CB cb)
{
    return  service.set_make_shared_cb(cb);
}

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_QueryDNS(const std::string& host, std::vector<NetPoint>& addr, const std::string& service_d)
{
    return service.query_dns(host, addr, service_d);
}

