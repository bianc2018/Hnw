#include "service.h"

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

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
        //ָ���ò�����Ĭ��ֵ 
        // "apple,a" : ָ��ѡ���ȫд��ʽΪ --apple, ��д��ʽΪ -a
        //value<type>(ptr) : ptrΪ��ѡ���Ӧ���ⲿ�����ĵ�ַ, ����ѡ�������, 
        //��ͨ�������notify()������ѡ���ֵ�������ⲿ����,�ñ�����ֵ���Զ�����
        //defaut_value(num) : numΪ��ѡ���Ĭ��ֵ, ����������δ�����ѡ��, ���ѡ���ֵȡΪnum
        ("host,h", 
            boost::program_options::value<std::string>(&param_.host)->default_value("http://0.0.0.0"),
            "����������ַ")
        ("accept_num,an",
            boost::program_options::value<size_t>(&param_.accept_num)->default_value(0),
            "�������߳���,0��ʶ���²��Զ�ȷ��")
        //�ò�����ʵ������Ϊvector,�����������������ֵ�����Ƕ��,
        //multitoken()�����þ��Ǹ��߱�����,��ѡ��ɽ��ܶ��ֵ  
        //("address", boost::program_options::value<std::vector<std::string> >()->multitoken(), "������")
        ("dir,d",
            boost::program_options::value<std::string>(&file_dir_)->default_value("F:/"),
            "������ļ�Ŀ¼")
        ("hnwlog", "����hnw����־")
        ("help", "����");

    boost::program_options::variables_map vm;

    try {
        boost::program_options::store(parse_command_line(argc, argv, opt), vm);
    }
    catch (std::exception &e) {
        std::cout << "�����������"<<e.what()<<std::endl;
        std::cout << opt << std::endl;
        return false;
    }
    //����������ɺ�֪ͨvariables_mapȥ�������е��ⲿ����
    //��仰ִ�к�, ��ʹ��apple_num��orange_num��ֵ�Զ�����Ϊѡ��ָ����ֵ   
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << opt << std::endl;
        return false;
    }
    if (vm.count("hnwlog")==0) 
    {
        //Ĭ�Ϲر���־
        //HnwHttp_SetLogCB(nullptr);
    }
    return true;
}
 
#define KB_P ((std::uint64_t)1024)
#define MB_P ((std::uint64_t)(1024)*KB_P)
#define GB_P ((std::uint64_t)(1024)*MB_P)
std::vector<file::FileNode> file::FileServce::get_file_node_list(const std::string& dir)
{
    std::vector<file::FileNode> nodes;
    boost::system::error_code ec;
    for (auto& fe : boost::filesystem::directory_iterator(dir,ec))
    {
        file::FileNode node;
        node.name = fe.path().filename().string();
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
        if (node.type == FILE)
        {
            boost::system::error_code ec;
            double size = boost::filesystem::file_size(fe, ec);
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
                node.file_size = size;
                node.size_type = FT_B;
            }
        }
        nodes.push_back(node);
    }
    return nodes;
}


std::string file::FileServce::list_to_html(std::vector<FileNode> list,const std::string& ppath)
{
    std::string html_data;
    html_data += "<html>";

    auto head = "<head>"
                    "<title> File Shared</title>"
                "</head>";
    html_data += head;
    html_data += "<body>";

    //head
    std::string ul = "<ul>";
    //�����ϼ�Ŀ¼
    const size_t li_size = 1024;
    char li[li_size] = {};
    snprintf(li, li_size, "<li><a href=\"%s\">..</a></li>", get_parent_path(ppath).c_str());
    ul += li;

    //Ŀ¼
    for (auto l : list)
    {
        char url[li_size] = {};
        snprintf(url, li_size, "%s%s%s", ppath.c_str(),
            (ppath.size() > 0 && (ppath[ppath.size() - 1] == '/')) ? "" : "/",
            l.name.c_str()
        );
        snprintf(li, li_size, "<li><a href=\"%s\">%s (%s[%0.2f %s])</a> (<a href=\"%s?action=show\">TODO</a>)</li>",
            url,
            l.name.c_str(),
            l.get_type().c_str(),
            l.file_size,
            l.get_size_type().c_str(),
            url);
        ul += li;
    }

    ul += "</ul>";
    html_data += ul;
    html_data += "</body>";
    html_data += "</html>";
    return HnwUtil_AnsiToUtf8(html_data);
}

void file::FileServce::http_event_cb(std::int64_t handle, int t, std::shared_ptr<void> event_data)
{
    HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)t;
    //printf("Event CB handle-%lld,type-%d buff-%p\n",
    //    handle, type, event_data.get());
    
    if (HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED == type)
    {
        printf("handle:%lld is close\n", handle);
    }
    else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_REQUEST == type)
    {
        printf("handle:%lld is recv a request %p\n", handle, event_data.get());

        auto request = PTR_CAST(HnwHttpRequest, event_data);
        SPHnwHttpResponse response;
        auto ret = HnwHttp_GenerateResponse(handle, response);
        if (ret != HNW_BASE_ERR_CODE::HNW_BASE_OK)
        {
            printf("HnwHttp_GenerateResponse error,code=%d\n", ret);
            HnwHttp_Close(handle);
            return ;
        }
        response->head->keep_alive(request->head->keep_alive());

        //��֤ͷ
        auto req_auth = request->head->get_authorization();
        //����
        if (NoneAuth != req_auth.method&&req_auth.username=="admin")
        {
            if (request->head->check_auth(req_auth, "admin\0", request->line->method()))
            {
                ret = on_request(request, response);
                if (ret != HNW_BASE_ERR_CODE::HNW_BASE_OK)
                {
                    auto path = HnwUtil_Utf8ToAnsi(HnwUtil_UrlDecode(request->line->path()));
                    printf("on_request %s error,code=%d\n", path.c_str(), ret);
                    HnwHttp_Close(handle);
                    return;
                }
                ret = HnwHttp_Response(handle, response);
                if (ret != HNW_BASE_ERR_CODE::HNW_BASE_OK)
                {
                    printf("on_request error,code=%d\n", ret);
                    HnwHttp_Close(handle);
                    return;
                }
            }
            else
            {
                printf("check auth error\n");
            }
        }
        else
        {
            printf("no auth error\n");
        }
        
        //��Ҫ��Ȩ
        response->line->status_code("401");
        HnwWWWAuthenticate auth;
        auth.method = DigestAuth;
        auth.realm = "fileshared@hql";
        auth.nonce = std::string(32,'1');
        response->head->set_www_authenticate(auth);
        ret = HnwHttp_Response(handle, response);
        if (ret != HNW_BASE_ERR_CODE::HNW_BASE_OK)
        {
            printf("on_request error,code=%d\n", ret);
            HnwHttp_Close(handle);
            return;
        }
        return;

    }
    else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR == type)
    {
        auto error = PTR_CAST(HnwBaseErrorEvent, event_data);
        printf("handle:%lld is has error(%d,%s)\n", handle, (int)error->code, error->message.c_str());
    }
}

HNW_BASE_ERR_CODE file::FileServce::on_request(SPHnwHttpRequest request, SPHnwHttpResponse response)
{
    auto path = HnwUtil_Utf8ToAnsi(HnwUtil_UrlDecode(request->line->path()));

    auto action = request->line->get_query("action");

    std::string dir = file_dir_ + path;;
    if (!boost::filesystem::exists(dir))
    {
        return response_error(request, response,
            HNW_BASE_ERR_CODE::HNW_HTTP_RES_IS_INVALID,"������ļ�"+dir+"�޷��ҵ�");
    }
    if (action == "" || action == "donwload")
    {
        if (boost::filesystem::is_directory(dir))
        {
            return response_dir(request,response,dir,path);
        }
        else
        {
            HttpRange Range;
            if (request->head->get_range(Range))
            {
                //response->head->set_content_range(Range);
                response->line->status_code("206");
            }
            else
            {
                Range = HttpRange();
                response->line->status_code("200");
            }
            if (response->set_file_body(dir, Range))
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            return  response_error(request, response,
                HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR, "δ֪�Ĵ������� service.cpp 340 set_file_body");
        }
    }
    else if (action == "show")
    {
        return response_video_play_page(request, response, path);
    }
    else
    {
        return response_error(request, response,
            HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID, "��Ӧ·����֧�� action="+ action);
    }
   
}

HNW_BASE_ERR_CODE file::FileServce::response_error(SPHnwHttpRequest request, 
    SPHnwHttpResponse response, HNW_BASE_ERR_CODE code, const std::string& msg)
{
    std::string html_data;
    html_data += "<html>";

    auto head = "<head>"
        "<title> Http Error </title>"
        "</head>";
    html_data += head;

    html_data += "<body>";

    auto path = HnwUtil_Utf8ToAnsi(HnwUtil_UrlDecode(request->line->path()));

    std::string message = "<p>������:" + std::to_string((int)code) + "</p>"
        "<p>����:" + msg + "</p>"
        "<p>����:<a href=\"" + get_parent_path(path) + "\">��һ��</a></p>";

    html_data += message;
    html_data += "</body>";
    html_data += "</html>";

    html_data = HnwUtil_AnsiToUtf8(html_data);

    return response_html_page(response, html_data);
}

std::string file::FileServce::get_parent_path(const std::string& path)
{
    //�Ѿ��Ǹ�Ŀ¼��
    if (path.empty() || path == "/")
        return "/";
    //���� /
    int i = path.size() - 1;
    int p = i;//���� /��β
    for (; i >= 0; --i)
    {
        if (path[i] == '/')
        {
            if (i == p)
            {
                --p;
                continue;
            }
            else
            {
                break;
            }
        }
    }
    //auto p = temp.find_last_of('/');
    
    return i==0?"/":path.substr(0,i);
}

HNW_BASE_ERR_CODE file::FileServce::response_dir(SPHnwHttpRequest request,
    SPHnwHttpResponse response,
    const std::string& dir, const std::string& ppath)
{
    auto file_list = get_file_node_list(dir);
    auto data = list_to_html(file_list, ppath);

    return response_html_page(response, data);
}

HNW_BASE_ERR_CODE file::FileServce::response_video_play_page(SPHnwHttpRequest request,
    SPHnwHttpResponse response, const std::string& video_play_url)
{
    std::string html_data;
    html_data += "<html>";

    auto head = "<head>"
        "<title> File Play</title>"
        "</head>";
    html_data += head;
    /*
    <link href=\"//vjs.zencdn.net/7.8.2/video-js.min.css\" rel=\"stylesheet\">
    <script src=\"//vjs.zencdn.net/7.8.2/video.min.js\"></script>
    */
    /*html_data += "<link href=\"//vjs.zencdn.net/7.8.2/video-js.min.css\" rel=\"stylesheet\">";
    html_data += "<script src =\"//vjs.zencdn.net/7.8.2/video.min.js\"></script>";*/
    html_data += "<body>";//id=\"my-player\" class=\"video-js\"  data-setup='{}'
    auto video_play = "<video  width=\"100%\" height=\"90%\" webkit-playsinline=\"true\""
        " src=\"" + video_play_url + "\" controls=\"controls\"></video>";
    html_data += video_play;
    html_data += "</body>";
    html_data += "</html>";
    html_data = HnwUtil_AnsiToUtf8(html_data);

    return response_html_page(response,html_data);
}

HNW_BASE_ERR_CODE file::FileServce::response_html_page(SPHnwHttpResponse response, const std::string& html_data)
{
    if (html_data.empty())
    {
        printf("HnwUtil_AnsiToUtf8 error \n");
        return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
    }
    response->head->content_type("text/html;charset=UTF-8");
    response->line->status_code("200");
    if (html_data.size() == response->body->write_body(html_data.c_str(), html_data.size()))
        return HNW_BASE_ERR_CODE::HNW_BASE_OK;
    printf("write body error");
    return HNW_BASE_ERR_CODE::HNW_BASE_NUKNOW_ERROR;
}

