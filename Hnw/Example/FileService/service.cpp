#include "service.h"

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

//void event_cb(std::int64_t handle, \
//    int t,
//    std::shared_ptr<void> event_data)
//{
//    HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)t;
//    printf("Event CB handle-%lld,type-%d buff-%p\n",
//        handle, type, event_data.get());
//
//    if (HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH == type)
//    {
//        printf("handle:%lld is connnect \n", handle);
//    }
//    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT == type)
//    {
//        //auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
//        printf("handle:%lld accept  \n", handle);
//    }
//    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_SEND_COMPLETE == type)
//    {
//        printf("handle:%lld is send data complete\n", handle);
//    }
//    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED == type)
//    {
//        printf("handle:%lld is close\n", handle);
//    }
//    else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_REQUEST == type)
//    {
//        printf("handle:%lld is recv a request %p\n", handle, event_data.get());
//
//        auto request = PTR_CAST(HnwHttpRequest, event_data);
//        static std::string dir = "F:/backup";
//
//        HnwHttp_FileResponse(handle, "200", dir + request->line->url());
//    }
//    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR == type)
//    {
//        auto error = PTR_CAST(HnwBaseErrorEvent, event_data);
//        printf("handle:%lld is has error(%d,%s)\n", handle, (int)error->code, error->message.c_str());
//    }
//
//}

file::FileServce::FileServce()
    :http_server_handle_(HNW_INVALID_HANDLE)
{
    param_.peer_type = Server;
}

file::FileServce::~FileServce()
{
    if (HNW_INVALID_HANDLE != http_server_handle_)
    {
        HnwHttp_Close(http_server_handle_);
        http_server_handle_ = HNW_INVALID_HANDLE;
    }
}

int file::FileServce::start(int argc, char* argv[])
{
    if (!int_config_from_shell(argc, argv))
        return -1;

    auto ret = HnwHttp_Start(param_, \
        std::bind(&FileServce::http_event_cb, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        http_server_handle_);
    if (ret != HNW_BASE_ERR_CODE::HNW_BASE_OK)
    {
        std::cout << "start http server[" << param_.host << "] fail! ret=" <<(int) ret << std::endl;
        return -1;
    }
    std::cout << "start http server[" << param_.host << "] ok!"<<std::endl;
  /*  run_flag_ = true;
    while (run_flag_)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }*/
    return 0;
}

bool file::FileServce::int_config_from_shell(int argc, char* argv[])
{
    
    boost::program_options::options_description opt("file shared service options");
    
    opt.add_options()
        //指定该参数的默认值 
        // "apple,a" : 指定选项的全写形式为 --apple, 简写形式为 -a
        //value<type>(ptr) : ptr为该选项对应的外部变量的地址, 当该选项被解析后, 
        //可通过下面的notify()函数将选项的值赋给该外部变量,该变量的值会自动更新
        //defaut_value(num) : num为该选项的默认值, 若命令行中未输入该选项, 则该选项的值取为num
        ("host,h", 
            boost::program_options::value<std::string>(&param_.host)->default_value("http://127.0.0.1"),
            "服务主机地址")
        ("accept_num,an",
            boost::program_options::value<size_t>(&param_.accept_num)->default_value(0),
            "监听的线程数,0标识由下层自动确定")
        //该参数的实际类型为vector,所以命令行中输入的值可以是多个,
        //multitoken()的作用就是告诉编译器,该选项可接受多个值  
        //("address", boost::program_options::value<std::vector<std::string> >()->multitoken(), "生产地")
        ("dir,d",
            boost::program_options::value<std::string>(&file_dir_)->default_value("./"),
            "共享的文件目录")
        ("hnwlog", "开启hnw库日志")
        ("help", "帮助");

    boost::program_options::variables_map vm;

    try {
        boost::program_options::store(parse_command_line(argc, argv, opt), vm);
    }
    catch (std::exception &e) {
        std::cout << "输入参数错误，"<<e.what()<<std::endl;
        std::cout << opt << std::endl;
        return false;
    }
    //参数解析完成后，通知variables_map去更新所有的外部变量
    //这句话执行后, 会使得apple_num和orange_num的值自动更新为选项指定的值   
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << opt << std::endl;
        return false;
    }
    if (vm.count("hnwlog")==0) 
    {
        //默认关闭日志
        HnwHttp_SetLogCB(nullptr);
    }
    return true;
}
#define KB_P 1024
#define MB_P 1024*1024
#define GB_P 1024*1024*1024
std::vector<file::FileNode> file::FileServce::get_file_node_list(const std::string& dir)
{
    std::vector<file::FileNode> nodes;
    for (auto& fe : boost::filesystem::directory_iterator(dir))
    {
        file::FileNode node;
        node.name = fe.path().string();
        if (boost::filesystem::is_directory(fe))
        {
            node.type = DIR;
        }
        else if (boost::filesystem::is_symlink(fe))
        {
            node.type = LINK;
        }
        else
        {
            node.type = FILE;
        }
        double size = boost::filesystem::file_size(fe);
        if (size > GB_P)
        {
            node.file_size = size / GB_P;
            node.size_type = FT_GB;
        }
        else if (size > MB_P)
        {
            node.file_size = size / MB_P;
            node.size_type = FT_MB;
        }
        else if (size > KB_P)
        {
            node.file_size = size / KB_P;
            node.size_type = FT_KB;
        }
        else
        {
            node.file_size = size ;
            node.size_type = FT_B;
        }
            
        nodes.push_back(node);
    }
    return nodes;
}

void file::FileServce::http_event_cb(std::int64_t handle, int t, std::shared_ptr<void> event_data)
{
    HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)t;
    printf("Event CB handle-%lld,type-%d buff-%p\n",
        handle, type, event_data.get());
    
    if (HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED == type)
    {
        printf("handle:%lld is close\n", handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_REQUEST == type)
    {
        printf("handle:%lld is recv a request %p\n", handle, event_data.get());

        auto request = PTR_CAST(HnwHttpRequest, event_data);
        
        auto path = HnwUtil_Utf8ToAnsi(HnwUtil_UrlDecode(request->line->path()));

        
        boost::filesystem::path dir = file_dir_ + path;;
        if (boost::filesystem::is_directory(dir))
        {
            show_list(handle, get_file_node_list(dir.string()));
            return;
        }
        else
        {
            if (boost::filesystem::exists(dir))
            {
                HnwHttp_FileResponse(handle, "200", dir.string());
            }
            else
            {
                HnwHttp_StatusResponse(handle, "404");
            }
            return;
        }
        
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR == type)
    {
        auto error = PTR_CAST(HnwBaseErrorEvent, event_data);
        printf("handle:%lld is has error(%d,%s)\n", handle, (int)error->code, error->message.c_str());
    }
}

