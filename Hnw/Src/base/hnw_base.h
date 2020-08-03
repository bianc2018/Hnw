/*
    ����boost.asio ��װ�Ļ�������� ��Žӿ�
    hql 2020/01/04
*/
#ifndef HNW_BASE_H_
#define HNW_BASE_H_

#include "hnw_define.h"

//����ͨ��
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Add_Channnel(HNW_CHANNEL_TYPE type\
    , const NetPoint& local, HNW_HANDLE& handle);

//��������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Connect(HNW_HANDLE handle, const NetPoint& remote);

//��������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Accept(HNW_HANDLE handle);

//�ر�һ��ͨ��
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Close(HNW_HANDLE handle);

//��������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Send(HNW_HANDLE handle, \
    std::shared_ptr<char> message, size_t message_size);

//��������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Send_String(HNW_HANDLE handle, const std::string& message);

//send call back ����=0����
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Send_Cb(HNW_HANDLE handle, HNW_SEND_CB cb);

//�����¼��ص�
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetEvntCB(HNW_HANDLE handle, HNW_EVENT_CB cb);

//������־�ص�
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetLogCB(HNW_LOG_CB cb);

//���û���ص�
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetMakeSharedCB(HNW_MAKE_SHARED_CB cb);

//������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Config(HNW_HANDLE handle, int config_type, void* data, size_t data_len);

//���û�ȡ�û�����
HNW_BASE_EXPORT_SYMBOLS void* HnwBase_GetUserData(HNW_HANDLE handle);

HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetUserData(HNW_HANDLE handle, void* userdata);

//����һ����ʱ�� close �ر�
HNW_BASE_ERR_CODE HnwBase_AddTimer(size_t time_out_ms,
    std::function<void()> call,
    HNW_HANDLE& handle);
//��ѯdns
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_QueryDNS(const std::string &host,
    std::vector<NetPoint> &addr, const std::string& service="http");

//��ȡ�㲥��ַ
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_BroadCast(std::string &ip);
#endif // !BNS_H_


