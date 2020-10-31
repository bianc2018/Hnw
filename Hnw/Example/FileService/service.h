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
		FileNodeType type = FileNodeType::FILE;
		
		//�ļ���Ч
		FileSizeType size_type= FileSizeType::FT_B;
		float file_size=0;

		std::string name;

		std::string get_size_type()
		{
			switch (size_type)
			{
			case file::FT_B:
				return "Byte";
				break;
			case file::FT_KB:
				return "KB";
				break;
			case file::FT_MB:
				return "MB";
				break;
			case file::FT_GB:
				return "GB";
				break;
			case file::FT_TB:
				return "TB";
				break;
			default:
				break;
			}
			return " ";
		}
		std::string get_type()
		{
			switch (type)
			{
			case file::DIR:
				return "Ŀ¼";
				break;
			case file::FILE:
				return "�ļ�";
				break;
			case file::LINK:
				return "����";
				break;
			default:
				break;
			}
			return " ";
		}
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

		//�������Ž���
		bool show_video_play(HNW_HANDLE cli_handle, const std::string& path);

		//�����ļ��б�����htmlҳ��
		std::string list_to_html(std::vector<FileNode> list, const std::string& ppath);

		//http �ص�����
		void http_event_cb(std::int64_t handle,
			int t,
			std::shared_ptr<void> event_data);

	private:
		//����ظ�
		HNW_BASE_ERR_CODE on_request(SPHnwHttpRequest request, \
			SPHnwHttpResponse response);

		//�ظ�����
		//����ظ�
		HNW_BASE_ERR_CODE response_error(SPHnwHttpRequest request,
			SPHnwHttpResponse response,
			HNW_BASE_ERR_CODE code,const std::string &msg);
		
		//��ȡ��Ŀ¼
		std::string get_parent_path(const std::string& path);

		//�ظ�Ŀ¼
		HNW_BASE_ERR_CODE response_dir(SPHnwHttpRequest request,
			SPHnwHttpResponse response,
			const std::string& dir,const std::string &ppath="/"/*���ظ�Ŀ¼*/);

		HNW_BASE_ERR_CODE response_video_play_page(SPHnwHttpRequest request,
			SPHnwHttpResponse response,
			const std::string& video_play_url);

		HNW_BASE_ERR_CODE response_html_page(SPHnwHttpResponse response,
			const std::string& html_data);
	private:

		HttpParam param_;

		//�ļ�Ŀ¼Ŀ¼
		std::string file_dir_;

		//���������
		HNW_HANDLE http_server_handle_;

		//��֤ģʽ
		HnwHttpAuthMethod auth_mode_;

		//�û���������
		std::string username_, password_;
	};

}
#endif // !1
