#include "hnw_http.h"

#include <time.h>

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
//https://en.softonic.com/download-launch?token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJkb3dubG9hZFR5cGUiOiJpbnRlcm5hbERvd25sb2FkIiwiZG93bmxvYWRVcmwiOiJodHRwczovL2dzZi1mbC5zb2Z0b25pYy5jb20vYzRlLzc3ZS80ZWRhMDkzOTQwNDRkMjdhMTMyNGZjOTI4YmQ2M2Y4YmQzL2ZpbGU_RXhwaXJlcz0xNTkwODg0ODA4JlNpZ25hdHVyZT02ZjRhNzUwYmY0OTZjMzY3OGNhOGIyZDI0NjM5YmZkOTk3NTQ4OGY3JnVybD1odHRwczovL2RpcmVjdHgtcmVkaXN0cmlidXRhYmxlLmVuLnNvZnRvbmljLmNvbSZGaWxlbmFtZT1kaXJlY3R4X2p1bjIwMDdfcmVkaXN0LmV4ZSIsImFwcElkIjoiZGQyZGFkYmMtOTZkNC0xMWU2LTg2ZmQtMDAxNjNlZDgzM2U3IiwiaWF0IjoxNTkwODQ3OTM5LCJleHAiOjE1OTA4NTE1Mzl9.jJfTV6iKl0gXcElD2RRfW3__62fpV5022urX1tocxrw
static std::string path = "/";

static clock_t time_ms = ::clock();
static std::atomic_int64_t req_num = 0;
static std::atomic_int64_t con_num = 0;
static std::vector<HNW_HANDLE> handles;
static HttpParam param;
void event_cb(std::int64_t handle, \
    int t,
    std::shared_ptr<void> event_data)
{
    HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)t;
   /* printf("Event CB handle-%lld,type-%d buff-%p\n",
        handle, type, event_data.get());*/

    if (HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH == type)
    {
       /* printf("handle:%lld is connnect \n", handle);*/

        auto request = std::make_shared<HnwHttpRequest>();
        request->head.insert(std::make_pair("Connection", "Close"));
        request->url = path;
       // request->body = std::string(1024*1024*20 , 'c');
        req_num = 0;
        time_ms = clock();
        auto ret = HnwHttp_Request(handle, request);
        con_num++;
        /*printf("send a request %d\n", ret);*/
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_RECV_DATA == type)
    {
        /*auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
        printf("handle:%lld is recv data :%s \n", handle, std::string(data->buff
            , data->buff_len).c_str());*/
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_SEND_COMPLETE == type)
    {
       /* printf("handle:%lld is send data complete\n", handle);*/
       /* auto request = std::make_shared<HnwHttpRequest>();
        request->url = path;
        request->head.insert(std::make_pair("Connection", "Keep-Alive"));
        request->body = std::string(1024 * 1024 * 10, 'data');
        auto ret = HnwHttp_Request(handle, request);*/
      
       /* printf("send a request %d\n", ret);*/
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED == type)
    {
        /*printf("handle:%lld is close\n", handle);*/
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_RESPONSE == type)
    {
       /* printf("handle:%lld is recv a response %p\n", handle, event_data.get());*/

        auto response = PTR_CAST(HnwHttpResponse, event_data);
        //printf("%s:%s\n", response->status_code.c_str(), response->body.c_str());

        //++req_num;
        req_num += response->body.size();
        auto data = req_num.load();
        auto t = (clock() - time_ms);
        printf("[%lld]commit all %lld byte,usd %d ms %f KB/s\n", con_num.load(),
            data, t, data / float(t));
        HnwHttp_Close(handle);
        //HnwHttp_Start(param, event_cb, handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR == type)
    {
        auto error = PTR_CAST(HnwBaseErrorEvent, event_data);
        //HnwHttp_Start(param, event_cb, handle);
        printf("handle:%lld is has error(%d,%s)\n", handle, (int)error->code, error->message.c_str());
    }

}


std::atomic_bool flag = false;
void check_time()
{
    while (flag)
    {
        auto data = req_num.load();
        auto t = (clock() - time_ms);
        printf("commit all %d byte,usd %d ms %f KB/s\n", data, t, data/float(t));

        //清零
        //req_num = 0;
        //time_ms = clock();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main(int argc, char* argv[])
{
    //HnwHttp_SetLogCB(NULL);

    std::cout << "input thread num:";
    size_t thread_num = 10000;
   // std::cin >> thread_num;

    std::cout << "input home:";
    
    param.host = "www.baidu.com";
    param.peer_type = Client;
    //std::cin >> param.host;

    std::cout << "input path:";
    //std::cin >> path;

    
    while (handles.size()<thread_num)
    {
        //初始化通道
        HNW_HANDLE handle = -1;
        auto ret = HnwHttp_Start(param, event_cb, handle);
        if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
        {
            printf("HnwHttp_StartServer ok,handle=%lld\n", handle);
            handles.push_back(handle);
        }
        else
        {
            printf("HnwHttp_StartServer fail,ret =%d \n", ret);
            return (int)ret;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::thread th(check_time);
    system("pause");
    flag = false;
    if (th.joinable())
        th.join();
    for(auto &h:handles)
        HnwHttp_Close(h);

    //HnwBase_DInit();
    return 0;
}