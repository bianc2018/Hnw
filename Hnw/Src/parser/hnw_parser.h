/*
    ����Hnw ������ ��Žӿ�
    hql 2020/01/04
*/
#ifndef HNW_PARSER_H_
#define HNW_PARSER_H_

//���嵼������
#ifdef _WIN32
#define HNW_PARSER_EXPORT_SYMBOLS extern "C" __declspec(dllexport)
#else
#define HNW_PARSER_EXPORT_SYMBOLS 
#endif

#include <functional>
#include <memory>

//�������
typedef void* PARSER_HANDLE;

enum PERROR
{
    OK,
    ERROR
};

//�����ص�
typedef std::function<void(PERROR error, std::shared_ptr<void> parser_struct)> PARSER_CB;
//����������
HNW_PARSER_EXPORT_SYMBOLS PARSER_HANDLE Hnw_CreateParser(int type, PARSER_CB cb);

//��������
HNW_PARSER_EXPORT_SYMBOLS PERROR Hnw_InputData(PARSER_HANDLE handle,const char*data,size_t data_len);

//�ͷŽ�����
HNW_PARSER_EXPORT_SYMBOLS PARSER_HANDLE Hnw_ReleaseParser(PARSER_HANDLE &handle);


#endif