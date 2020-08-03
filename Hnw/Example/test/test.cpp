#include <iostream>
#include <boost/asio.hpp>
#include <unordered_map>


int main()
{
    std::unordered_multimap<std::string, std::string > d ;
    d.insert(std::make_pair("1", "1"));
    d.insert(std::make_pair("1", "2"));
    d.insert(std::make_pair("2", "2"));
    d.erase("1");

    return 0;
}