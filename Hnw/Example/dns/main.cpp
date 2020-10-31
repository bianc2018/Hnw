#include "hnw_base.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    //HnwBase_Init();

    std::string host,p;
    std::cout << "input host:";
    std::cin >> host;
    std::cin >> p;
    std::vector<NetPoint> addrs;
    auto ret = HnwBase_QueryDNS(host, addrs,p);
    if (HNW_BASE_ERR_CODE::HNW_BASE_OK == ret)
    {
        std::cout << "DNS Ok Host :" << host << ",ip size=" << addrs.size()<<std::endl;
        for (auto& p : addrs)
        {
            std::cout << "ip "<<p.ip<<":"<<p.port << std::endl;
        }
    }
    else
    {
        std::cout << "DNS Fail Host :" << host << ",ret=" << (int)ret << std::endl;
    }

   // HnwBase_DInit();
    return 0;
}