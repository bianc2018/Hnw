#include "hnw_http.h"

#include <iostream>
#include <string>

void event_cb(std::int64_t handle, \
    int t,
    std::shared_ptr<void> event_data)
{
    HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)t;
    printf("Event CB handle-%lld,type-%d buff-%p\n",
        handle, type, event_data.get());

    if (HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH == type)
    {
        printf("handle:%lld is connnect \n", handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT == type)
    {
        //auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
        printf("handle:%lld accept  \n", handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_SEND_COMPLETE == type)
    {
        printf("handle:%lld is send data complete\n", handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED == type)
    {
        printf("handle:%lld is close\n", handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_REQUEST == type)
    {
        printf("handle:%lld is recv a request %p\n", handle, event_data.get());

        auto request = PTR_CAST(HnwHttpRequest, event_data);
        printf("recv request %s\n:%s\n", request->url.c_str(), request->body.c_str());
        auto response = std::make_shared<HnwHttpResponse>();
        response->body ="\r\n--beg req--\r\n"+request->body+"\r\n--end req--\r\n";
        HnwHttp_Response(handle, response);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR == type)
    {
        auto error = PTR_CAST(HnwBaseErrorEvent, event_data);
        printf("handle:%lld is has error(%d,%s)\n", handle, (int)error->code, error->message.c_str());
    }

}

HNW_HANDLE handle = -1;
int main(int argc, char* argv[])
{
    //HnwBase_Init();
    /*
    context_.use_certificate_chain_file("D:\\lib\\cert\\server-cert.pem");
        context_.use_private_key_file("D:\\lib\\cert\\server-key.pem", boost::asio::ssl::context::pem);
        context_.use_tmp_dh_file("D:\\lib\\cert\\dh1024.pem");
    */
    HttpParam param;
    param.host = "https://127.0.0.1:8081/";
    param.peer_type = Server;
    param.cert_file = "D:\\lib\\cert\\server-cert.pem";
    param.pri_key_file = "D:\\lib\\cert\\server-key.pem";
    //初始化通道
    auto ret = HnwHttp_Start(param, event_cb, handle);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
    {
        printf("HnwHttp_StartServer ok,handle=%lld\n", handle);
    }
    else
    {
        printf("HnwHttp_StartServer fail,ret =%d \n", ret);
        return (int)ret;
    }

    system("pause");

    HnwHttp_Close(handle);

    //HnwBase_DInit();
    return 0;
}