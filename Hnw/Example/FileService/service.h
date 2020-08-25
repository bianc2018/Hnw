/*
	文件服务器
	*/
#ifndef FILE_SERVCIE_H_
#define FILE_SERVCIE_H_
#include "hnw_http.h"

namespace file
{
	//节点类型
	enum FileNodeType
	{
		//目录
		DIR,
		//文件
		FILE,
		//链接
		LINK,
	};
	//数目类型
	enum FileSizeType
	{
		FT_B,
		FT_KB,
		FT_MB,
		FT_GB,
		FT_TB
	};
	//目录节点
	struct FileNode
	{
		FileNodeType type = FileNodeType::FILE;
		
		//文件有效
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
				return "目录";
				break;
			case file::FILE:
				return "文件";
				break;
			case file::LINK:
				return "链接";
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
		//初始化参数
		bool int_config_from_shell(int argc, char* argv[]);

		//获取文件列表
		std::vector<FileNode> get_file_node_list(const std::string& dir);

		//发布播放界面
		bool show_video_play(HNW_HANDLE cli_handle, const std::string& path);

		//根据文件列表生成html页面
		std::string list_to_html(std::vector<FileNode> list, const std::string& ppath);

		//http 回调函数
		void http_event_cb(std::int64_t handle,
			int t,
			std::shared_ptr<void> event_data);

	private:
		//请求回复
		HNW_BASE_ERR_CODE on_request(SPHnwHttpRequest request, \
			SPHnwHttpResponse response);

		//回复错误
		//请求回复
		HNW_BASE_ERR_CODE response_error(SPHnwHttpRequest request,
			SPHnwHttpResponse response,
			HNW_BASE_ERR_CODE code,const std::string &msg);
		
		//获取父目录
		std::string get_parent_path(const std::string& path);

		//回复目录
		HNW_BASE_ERR_CODE response_dir(SPHnwHttpRequest request,
			SPHnwHttpResponse response,
			const std::string& dir,const std::string &ppath="/"/*返回父目录*/);

		HNW_BASE_ERR_CODE response_video_play_page(SPHnwHttpRequest request,
			SPHnwHttpResponse response,
			const std::string& video_play_url);

		HNW_BASE_ERR_CODE response_html_page(SPHnwHttpResponse response,
			const std::string& html_data);
	private:

		HttpParam param_;

		//文件目录目录
		std::string file_dir_;

		//服务器句柄
		HNW_HANDLE http_server_handle_;

		//验证模式
		HnwHttpAuthMethod auth_mode_;

		//用户名和密码
		std::string username_, password_;
	};

}
#endif // !1
