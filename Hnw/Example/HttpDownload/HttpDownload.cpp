#include "hnw_http.h"

#include <iostream>
#include <string>
#include <time.h>
#include <atomic>
#include <mutex>
struct DownloadStaus
{
    HttpRange range;/*正在下载的分段*/
    std::uint64_t pos=0;
    std::uint64_t last_pos=0;
    HNW_BASE_ERR_CODE status;
};
//start post
std::map<std::uint64_t, DownloadStaus> DownloadStausMap;
std::mutex DownloadStausMapLock;
int re_try = 0;
clock_t last_print= ::clock();
//上一次统计的
std::uint64_t last_down=0;
//下载失败的
//std::uint64_t s_down = 0;
void update_status(const HttpRange& range,/*正在下载的分段*/
    std::uint64_t pos,
    HNW_BASE_ERR_CODE status)
{
    std::lock_guard<std::mutex> lk(DownloadStausMapLock);

    if (status != HNW_BASE_ERR_CODE::HNW_BASE_OK)
    {
        printf("error happen %llu-%llu status %d", range.section_start(), range.section_end(), status);
        //s_down += pos;
        //DownloadStausMap.erase(range.section_start());
        return;
    }

    auto d = DownloadStausMap.find(range.section_start());
    if (d != DownloadStausMap.end())
    {
        d->second.range = range;
        d->second.pos = pos;
        d->second.status = status;
    }
    else
    {
        DownloadStaus temp;
        temp.range = range;
        temp.pos = pos;
        temp.status = status;
        DownloadStausMap.insert(std::make_pair(temp.range.section_start(), temp));
    }
}
void printf_status(const std::string& url,
    const std::string& output)
{
    system("cls");
    std::lock_guard<std::mutex> lk(DownloadStausMapLock);
    printf("-----------------------------------------------------\n");
    std::uint64_t all_down = 0,all=0;
    //更新时间
    auto d_print_ms = ::clock() - last_print;
    last_print = ::clock();
    printf("HangUp:\n");
    for (auto& it : DownloadStausMap)
    {
        if (it.second.status == HNW_BASE_ERR_CODE::HNW_BASE_OK)
            continue;
        all_down += it.second.pos;
        all = it.second.range.total;
        double ps = (double(it.second.pos) / it.second.range.section_size()) * 100;
        auto kbs = (double(it.second.pos - it.second.last_pos) / 1024) / (d_print_ms / 1000);
        it.second.last_pos = it.second.pos;
        std::string pro;
        int p = ps / 10;
        for (int i = 0; i < p && i < 10; ++i)
        {
            pro += '#';
        }
        printf("[%1d][%10llu B - %10llu B/%10llu B][%-10s][%.2f%%][%10llu Kb]-- %.2f KB/S\n",
            it.second.status,
            it.second.range.section_start(), it.second.range.section_end(), it.second.range.section_size()
            , pro.c_str(), ps, it.second.pos / 1024, kbs);

    }
    printf("Active:\n");
    for (auto &it : DownloadStausMap)
    {
        if (it.second.status != HNW_BASE_ERR_CODE::HNW_BASE_OK)
            continue;
        all_down += it.second.pos;
        all = it.second.range.total;
        double ps = (double(it.second.pos) / it.second.range.section_size())*100;
        auto kbs = (double(it.second.pos - it.second.last_pos)/1024) / (d_print_ms / 1000);
        it.second.last_pos = it.second.pos;
        std::string pro;
        int p = ps / 10;
        for (int i = 0; i < p && i < 10; ++i)
        {
            pro += '#';
        }
        printf("[%1d][%10llu B - %10llu B/%10llu B][%-10s][%.2f%%][%10llu Kb]-- %.2f KB/S\n",
        it.second.status,
            it.second.range.section_start(),it.second.range.section_end(),it.second.range.section_size()
        , pro.c_str(), ps, it.second.pos /1024, kbs);

    }
    //总的情况
    double ps = (double(all_down) / all) * 100;
    std::string pro;
    int p = ps / 10;
    for (int i = 0; i < p && i < 10; ++i)
    {
        pro += '#';
    }
    //增量
    auto d_down = all_down - last_down;
    auto kd_down = double(d_down) / 1024;//kb
    last_down = all_down;
    //kbs
    auto kbs = kd_down / (d_print_ms / 1000);
    printf("\r\nall :%s->%s [%-10s][%.2f%%][%10llu Kb] retry %d -- %.2f KB/S\n"
        ,url.c_str()
        ,output.c_str(),pro.c_str(),ps,all_down/1024,re_try, kbs);
    printf("-----------------------------------------------------\n");
}
//文件下载接口 返回 false 终止 con_num >=1 并发的请求数 
bool down(const std::string& url,
    const std::string& output,
    const HttpRange& range,/*正在下载的分段*/
    std::uint64_t pos,
    HNW_BASE_ERR_CODE status)
{
    /*printf("down %s [%d] [%llu-%llu] %.2f%%[%llu kb/%llu kb]\n", \
        url.c_str(),status,range.start, range.end,
        (double(pos)/range.section_size())*100,
        pos/1024, range.section_size()/1024);*/
    update_status(range, pos, status);
    if (status != HNW_BASE_ERR_CODE::HNW_BASE_OK)
    {
        HttpRange now = range;
        now.start = now.start + pos;
        //重来
        re_try++;
        auto ret = HnwHttp_DownLoad(url, output, down, 1, now,HTTP_FILE_FLAG_OPEN);
        printf("retry %d ret %d\n", re_try, ret);
    }
   // printf_status(url, output);
    return true;
}

HNW_HANDLE handle = -1;
int main(int argc, char* argv[])
{
    HnwHttp_SetLogCB(nullptr);
    std::cout << "use as: url output connnum:";

    std::string url = "http://mirror.worria.com/centos/7.8.2003/isos/x86_64/CentOS-7-x86_64-Everything-2003.iso";
    std::cin >> url;
    std::string output = "./CentOS-7-x86_64-Everything-2003.iso";
    std::cin >> output;
    int connnum=12;
    std::cin >> connnum;
    if (connnum <= 0)
        connnum = 1;
    std::atomic_bool flag = true;
    std::thread temp([&]()
        {
            while (flag)
            {
                printf_status(url, output);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    //17,264,608 
    HnwHttp_DownLoad(url, output, down, connnum);
    system("pause");
    flag = false;
    if(temp.joinable())
        temp.join();    
    //HnwBase_DInit();
    return 0;
}