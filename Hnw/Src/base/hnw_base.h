/*
    基于boost.asio 封装的基础网络库 外放接口
    hql 2020/01/04
*/
#ifndef HNW_BASE_H_
#define HNW_BASE_H_

#include "hnw_define.h"

//增加通道
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Add_Channnel(HNW_CHANNEL_TYPE type\
    , const NetPoint& local, HNW_HANDLE& handle);

//建立链接
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Connect(HNW_HANDLE handle, const NetPoint& remote);

//建立链接
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Accept(HNW_HANDLE handle);

//关闭一个通道
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Close(HNW_HANDLE handle);

//发送数据
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Send(HNW_HANDLE handle, \
    std::shared_ptr<char> message, size_t message_size);

//发送数据
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Send_String(HNW_HANDLE handle, const std::string& message);

//send call back 返回=0结束
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Send_Cb(HNW_HANDLE handle, HNW_SEND_CB cb);

//设置事件回调
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetEvntCB(HNW_HANDLE handle, HNW_EVENT_CB cb);

//设置日志回调
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetLogCB(HNW_LOG_CB cb);

//设置缓存回调
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetMakeSharedCB(HNW_MAKE_SHARED_CB cb);

//配置项
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Config(HNW_HANDLE handle, int config_type, void* data, size_t data_len);

//设置获取用户数据
HNW_BASE_EXPORT_SYMBOLS void* HnwBase_GetUserData(HNW_HANDLE handle);

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetUserData(HNW_HANDLE handle, void* userdata);

//新增一个定时器 close 关闭
HNW_BASE_ERR_CODE HnwBase_AddTimer(size_t time_out_ms,
    std::function<void()> call,
    HNW_HANDLE& handle);
//查询dns
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_QueryDNS(const std::string &host,
    std::vector<NetPoint> &addr, const std::string& service="http");

//获取广播地址
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_BroadCast(std::string &ip);
#endif // !BNS_H_


