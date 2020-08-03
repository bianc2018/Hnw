/*
	�ļ�������
	*/
#ifndef FILE_SERVCIE_H_
#define FILE_SERVCIE_H_
#include "hnw_http.h"

namespace file
{
	//�ڵ�����
	enum FileNodeType
	{
		//Ŀ¼
		DIR,
		//�ļ�
		FILE,
		//����
		LINK,
	};
	//��Ŀ����
	enum FileSizeType
	{
		FT_B,
		FT_KB,
		FT_MB,
		FT_GB,
		FT_TB
	};
	//Ŀ¼�ڵ�
	struct FileNode
	{
		FileNodeType type;
		
		//�ļ���Ч
		FileSizeType size_type;
		float file_size;

		std::string name;
	};

	class FileServce
	{
	public:
		FileServce();
		~FileServce();

		int start(int argc, char* argv[]);
	private:
		//��ʼ������
		bool int_config_from_shell(int argc, char* argv[]);


		//��ȡ�ļ��б�
		std::vector<FileNode> get_file_node_list(const std::string& dir);

		//�����ļ��б�
		bool show_list(HNW_HANDLE cli_handle, std::vector<FileNode> list);

		//�����ļ��б�����htmlҳ��
		std::string list_to_html(std::vector<FileNode> list);

		//������¼ҳ��
		bool show_login(HNW_HANDLE cli_handle);

		//http �ص�����
		void http_event_cb(std::int64_t handle,
			int t,
			std::shared_ptr<void> event_data);
	private:
		HttpParam param_;

		//�ļ�Ŀ¼Ŀ¼
		std::string file_dir_;

		//���������
		HNW_HANDLE http_server_handle_;
	};

}
#endif // !1
