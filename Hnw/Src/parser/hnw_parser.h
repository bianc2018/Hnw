/*
    基于Hnw 解析库 外放接口
    hql 2020/01/04
*/
#ifndef HNW_PARSER_H_
#define HNW_PARSER_H_

//定义导出符号
#ifdef _WIN32
#define HNW_PARSER_EXPORT_SYMBOLS extern "C" __declspec(dllexport)
#else
#define HNW_PARSER_EXPORT_SYMBOLS 
#endif

#include <functional>
#include <memory>

//解析句柄
typedef void* PARSER_HANDLE;

enum PERROR
{
    OK,
    ERROR
};

//解析回调
typedef std::function<void(PERROR error, std::shared_ptr<void> parser_struct)> PARSER_CB;
//创建解析器
HNW_PARSER_EXPORT_SYMBOLS PARSER_HANDLE Hnw_CreateParser(int type, PARSER_CB cb);

//输入数据
HNW_PARSER_EXPORT_SYMBOLS PERROR Hnw_InputData(PARSER_HANDLE handle,const char*data,size_t data_len);

//释放解析器
HNW_PARSER_EXPORT_SYMBOLS PARSER_HANDLE Hnw_ReleaseParser(PARSER_HANDLE &handle);


#endif