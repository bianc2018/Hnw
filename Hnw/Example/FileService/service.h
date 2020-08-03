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
		FileNodeType type;
		
		//文件有效
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
		//初始化参数
		bool int_config_from_shell(int argc, char* argv[]);


		//获取文件列表
		std::vector<FileNode> get_file_node_list(const std::string& dir);

		//发布文件列表
		bool show_list(HNW_HANDLE cli_handle, std::vector<FileNode> list);

		//根据文件列表生成html页面
		std::string list_to_html(std::vector<FileNode> list);

		//发布登录页面
		bool show_login(HNW_HANDLE cli_handle);

		//http 回调函数
		void http_event_cb(std::int64_t handle,
			int t,
			std::shared_ptr<void> event_data);
	private:
		HttpParam param_;

		//文件目录目录
		std::string file_dir_;

		//服务器句柄
		HNW_HANDLE http_server_handle_;
	};

}
#endif // !1
