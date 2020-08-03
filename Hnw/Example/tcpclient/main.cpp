#include "hnw_base.h"

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
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_RECV_DATA == type)
    {
        auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
        printf("handle:%lld is recv data :%s \n", handle,std::string(data->buff
            , data->buff_len).c_str());
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_SEND_COMPLETE == type)
    {
        printf("handle:%lld is send data complete\n",handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED == type)
    {
        printf("handle:%lld is close\n", handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR == type)
    {
        auto error = PTR_CAST(HnwBaseErrorEvent, event_data);
        printf("handle:%lld is has error(%d,%s)\n", handle,(int)error->code,error->message.c_str());
    }

}

HNW_HANDLE handle = -1;
int main(int argc, char* argv[])
{
    //HnwBase_Init();
    
    //初始化通道
    auto ret = HnwBase_Add_Channnel(HNW_CHANNEL_TYPE::TCP_CLIENT, NET_INVALID_POINT, handle);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
    {
        printf("add channel ok,handle=%lld\n", handle);
    }
    else
    {
        printf("add channel fail,ret =%d \n", ret);
        return (int)ret;
    }

    //设置事件回调
    HnwBase_SetEvntCB(handle, event_cb);

    //连接
    NetPoint dst; 
    dst.ip = "127.0.0.1";
    dst.port = 8081;
    ret = HnwBase_Connect(handle, dst);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
    {
        printf("connect %s:%d ok,handle=%lld\n",dst.ip.c_str(),dst.port, handle);
    }
    else
    {
        printf("connect %s:%d fail,handle=%lld\n", dst.ip.c_str(), dst.port, handle);
        return (int)ret;
    }

    system("pause");

    HnwBase_Close(handle);

    //HnwBase_DInit();
    return 0;
}