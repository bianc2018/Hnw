/*
    ����boost.asio ��װ�Ļ�������� ��Žӿ�
    hql 2020/01/04
*/
#ifndef HNW_BASE_H_
#define HNW_BASE_H_

#include "hnw_define.h"

//��־�ص�
typedef std::function<void(BLOG_LEVEL lv, const std::string& log_message)> HNW_LOG_CB;

//�ڴ�����ص�
typedef std::function<std::shared_ptr<char>(size_t memory_size)> HNW_MAKE_SHARED_CB;

////��ʼ��
//HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Init();
//
////����ʼ��
//HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_DInit();

//����ͨ��
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Add_Channnel(HNW_CHANNEL_TYPE type\
    , const NetPoint& local, HNW_HANDLE& handle);

//��������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Connect(HNW_HANDLE handle, const NetPoint& remote);

//��������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Accept(HNW_HANDLE handle, const NetPoint& remote);

//�ر�һ��ͨ��
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Close(HNW_HANDLE handle);

//��������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Send(HNW_HANDLE handle, \
    std::shared_ptr<char> message, size_t message_size);

//��������
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_Send_String(HNW_HANDLE handle, const std::string& message);

//�����¼��ص�
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetEvntCB(HNW_HANDLE handle, HNW_EVENT_CB cb);

//������־�ص�
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetLogCB(HNW_LOG_CB cb);

//���û���ص�
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_SetMakeSharedCB(HNW_MAKE_SHARED_CB cb);

//��ѯdns
HNW_BASE_EXPORT_SYMBOLS HNW_BASE_ERR_CODE HnwBase_QueryDNS(const std::string &host,
    std::vector<NetPoint> &addr, const std::string& service="http");
#endif // !BNS_H_


