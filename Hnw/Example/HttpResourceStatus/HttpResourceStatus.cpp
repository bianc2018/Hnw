#include "hnw_http.h"

#include <iostream>
#include <string>
#include <time.h>

void on_status(HttpResourceStatus status,
    HNW_BASE_ERR_CODE error)
{
    printf("HttpResourceStatus\n"
        " host[%s],path[%s],location[%s],"
        " size[%llu],partial_support[%d],status[%s]"
        "\n",
        status.host.c_str(),
        status.path.c_str(),
        status.location.c_str(),
        status.size,
        status.partial_support,
        status.status.c_str());
}
HNW_HANDLE handle = -1;
int main(int argc, char* argv[])
{
    //HnwHttp_SetLogCB(nullptr);

   std::string url = "http://www.baidu.com/index.html";
   std::cin >> url;
   HnwHttp_GetResourceStatus(url, on_status);

    //HnwHttp_DownLoad("https://dl.360safe.com/setup.exe", "D://setup.exe", nullptr);
    system("pause");
   
    //HnwBase_DInit();
    return 0;
}