
#include  <boost/interprocess/file_mapping.hpp>
#include  <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include "windows.h"
const std::string filename = "F:\\1.bin";
// 创建的文件总大小 = dwHigh * 4G + dwLow
// 当总大小小于4G时，dwHigh可以设置为0，dwLow数字最大为4G - 1
DWORD CreateNullFile(DWORD dwHigh, DWORD dwLow,const char* pszName)
{
  //  open()

    DWORD dwResult = 0;
    HANDLE hFile = CreateFile(pszName, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        return GetLastError();
    }

    HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, dwHigh, dwLow, NULL);
    if (NULL == hFileMap)
    {
        return GetLastError();
    }

    CloseHandle(hFileMap);
    CloseHandle(hFile);

    return dwResult;
}

int main(int argc, char* argv[])
{
    boost::filesystem::fstream::streampos;
    
    //fseek64
    //Create a file
   // boost::interprocess::file_mapping::remove(filename.c_str());

    CreateNullFile(25,0,filename.c_str());

    //std::streampos
	boost::interprocess::file_mapping fmap(filename.c_str(),
		boost::interprocess::mode_t::read_write);
    
	boost::interprocess::mapped_region map_region(fmap, boost::interprocess::mode_t::read_write,0,1024);
    
	std::cout << map_region.get_address() << " " << map_region.get_size();
	//memset(map_region.get_address(), 'a', 512);
	return 0;
}