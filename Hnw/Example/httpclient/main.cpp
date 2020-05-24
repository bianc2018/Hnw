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

        auto request = std::make_shared<HnwHttpRequest>();
        request->url = "/";
        auto ret = HnwHttp_Request(handle, request);
        printf("send a request %d\n", ret);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_RECV_DATA == type)
    {
        /*auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
        printf("handle:%lld is recv data :%s \n", handle, std::string(data->buff
            , data->buff_len).c_str());*/
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_SEND_COMPLETE == type)
    {
        printf("handle:%lld is send data complete\n", handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED == type)
    {
        printf("handle:%lld is close\n", handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_RESPONSE == type)
    {
        printf("handle:%lld is recv a response %p\n", handle, event_data.get());

        auto response = PTR_CAST(HnwHttpResponse, event_data);
        printf("%s:%s\n", response->status_code.c_str(), response->body.c_str());
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

    HttpParam param;
    param.host = "https://www.baidu.com/";
    param.peer_type = Client;
    param.cert_file = "D:\\code\\Hnw\\out\\build\\x64-Debug\\Bin\\Debug\\server-cert.pem";
    param.pri_key_file = "D:\\code\\Hnw\\out\\build\\x64-Debug\\Bin\\Debug\\server-key.pem";
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