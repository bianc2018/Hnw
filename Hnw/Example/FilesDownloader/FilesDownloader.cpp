/*
* ������
*/

#include <signal.h>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "hnw_http.h"

/// <summary>
/// ���������ļ� һ��һ������
/// </summary>
static std::string urls_file="F:/picture/out/���ꥸ�ʥ�10000users���.txt";

/// <summary>
/// ���󼯺�
/// </summary>
static std::vector<std::string> urls;

/// <summary>
/// ���·��
/// </summary>
static std::string out_dir = "F:/picture/out";

/// <summary>
/// ������ʱ ����
/// </summary>
static size_t request_delay = 100;

/// <summary>
/// ��ʱ ����
/// </summary>
static size_t request_timeout = 100000;

/// <summary>
/// ����ͷ
/// </summary>
static SPHnwHttpHead head=nullptr;

/*
* ��־���
*/
static bool log_output = false;

/*
* �Ƿ񸲸��ļ�overlay file
*/
static bool overlay_file = false;

static std::atomic_bool exit_flag = false;

//��������
static std::vector<std::string> split(const std::string& src, const std::string& splitor)
{
    std::vector<std::string> res;
    auto beg = 0;
    auto pos = std::string::npos;
    do
    {
        pos = src.find(splitor, beg);

        if (std::string::npos == pos)
        {
            res.push_back(src.substr(beg));
            break;
        }
        else
        {
            //auto t = src.substr(beg, pos - beg);
            //��ȡ
            res.push_back(src.substr(beg, pos - beg));

            //����
            beg = pos + splitor.size();
        }

    } while (true);

    return res;
}
/*
* ��ʼ�������в���
*/
bool  init_commond(int argc, char* argv[])
{
    boost::program_options::options_description opt("FilesDownloader options");
    //request_delay
    opt.add_options()
        ("urls_file,u",boost::program_options::value<std::string>(&urls_file)->default_value(urls_file),
            "urls �ļ�")
        ("help", "����");

    boost::program_options::variables_map vm;

    try {
        boost::program_options::store(parse_command_line(argc, argv, opt), vm);
    }
    catch (std::exception& e) {
        std::cout << "�����������" << e.what() << std::endl;
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
    return true;
}

/// <summary>
/// ����һ��
/// </summary>
/// <param name="result"></param>
/// <returns></returns>
bool parse_urls_line(const std::string &line)
{
    if (line.empty())
        return false;

    if (line[0] == '#')
    {
        try
        {
            //ָ�� out_dir= request_delay= head=, request_timeout
            auto pos = line.find_first_of('=');
            if (pos == std::string::npos)
            {
                return false;
            }
            auto cmd = line.substr(1, pos-1);
            auto param = line.substr(pos + 1);
            if ("out_dir" == cmd)
            {
                out_dir = param;
            }
            else if ("request_delay" == cmd)
            {
                request_delay =std::stoi(param);
            }
            else if ("request_timeout" == cmd)
            {
                request_timeout = std::stoi(param);
            }
            else if ("log_output" == cmd)
            {
                if (param == "on")
                    log_output = true;
                else if (param == "off")
                    log_output = false;
            }
            else if ("overlay_file" == cmd)
            {
                if (param == "on")
                    overlay_file = true;
                else if (param == "off")
                    overlay_file = false;
            }
            else if ("head" == cmd)
            {
                if (head == nullptr)
                {
                    std::cout << "head is nullptr " << std::endl;
                    return false;
                }

                pos = param.find_first_of(',');//���ָ�
                if (pos == std::string::npos)
                {
                    std::cout << "couldnt parse line " << line << std::endl;
                    return false;
                }
                auto key = param.substr(0, pos);
                auto value = param.substr(pos + 1);
                head->add_head(key, value);

            }
            else
            {
                std::cout << "couldnt parse line " << line << std::endl;
                return false;
            }
            return true;
        }
        catch (const std::exception&e)
        {
            std::cout << "parse_urls_line error line=" << line << " msg=" << e.what()<<std::endl;
            return false;
        }
       
    }
    else
    {
        //url
        urls.push_back(line);
    }
    return true;
}

/// <summary>
/// �����ļ�
/// </summary>
/// <param name="result"></param>
/// <returns></returns>
void parse_urls_txt()
{
    //std::vector<std::string> result;
    std::fstream file(urls_file, std::ios::in| std::ios::binary);
    if (!file)
    {
        std::cout << "dont open urls_file=" << urls_file << std::endl;
        return ;
    }
    const size_t buff_len = 4 * 1024*1024;
    char *buff =new char[buff_len];
    std::string temp;

    while (!file.eof())
    {
        file.read(buff, buff_len);
        auto read_len = file.gcount();
       // std::cout << "read:\n" << std::string(buff, read_len) << std::endl;
        if (read_len > 0)
        {
            /// <summary>
            /// �ض�
            /// </summary>
            auto url = std::string(buff, read_len);
            for (auto u : url)
            {
                /// <summary>
                /// ɾ�� �ո�ͻ���
                /// </summary>
                if (u == ' ' || u == '\r' || u == '\n')
                {
                    auto ret = parse_urls_line(temp);
                    std::cout << "parse_urls_line " << temp << (ret?" true":" false") << std::endl;
                    temp.clear();
                }
                else
                {
                    temp += u;
                }
            }
        }
        /*else
        {
            break;
        }*/
    }
    
    auto ret = parse_urls_line(temp);
    std::cout << "parse_urls_line " << temp << (ret ? " true" : " false") << std::endl;
    temp.clear();
    delete[]buff;
    return ;
}


static size_t request_count_=0;
static size_t response_count_=0;
static size_t success_count_ = 0;
void on_response(SPHnwHttpResponse response, HNW_BASE_ERR_CODE err,
    const std::string&output, const std::string& url)
{
    //����ظ�
    if (err == HNW_BASE_ERR_CODE::HNW_BASE_OK)
    {
        if (response->line->status_code() == "200")
        {
            if (false == overlay_file)
            {
                //������,���ھͲ��ظ���������
                if (boost::filesystem::exists(output))
                {
                    std::cout << "file " << output << " exist,skip " << url << std::endl;
                    return;
                }
            }

            //�����
            if (response->body->body_size()!=0&&response->save_body_as_file(output, HTTP_FILE_FLAG_CREATE))
            {
                std::cout << "save url=" << url << "->output " << output << ",size=" << response->body->body_size() << " OK " << std::endl;
                ++success_count_;
            }
            else
            {
                std::cout << "save url=" << url << "->output " << output << " FAIL " << std::endl;
            }
        }
        else
        {
            std::cout << "response url=" << url << "->output " << output << " status=" << response->line->status_code() << std::endl;
        }
    }
    else
    {
        std::cout << "response url=" << url << "->output " << output << " ret=" << (int)err << std::endl;
    }

    ++response_count_;
    if (response_count_ >= urls.size())
    {
        std::cout << "download all " << urls.size() << " success " << success_count_ << std::endl;
        //�˳�
        exit_flag = true;
    }
    std::cout << "request_count_=" << request_count_ << ",response_count_=" << response_count_ << ",success_count_=" << success_count_ << std::endl;
}

int main(int argc, char* argv[])
{
	//�����˳��źŴ�����
	signal(SIGINT, [](int) { exit_flag = true; });// �����˳��ź�

    HnwHttp_GenerateHead(0, head);

    if (!init_commond(argc, argv))
    {
        std::cout << "init_commond fail"<<std::endl;
        return -1;
    }

    /// <summary>
    /// ��ȡ�ļ��е���������
    /// </summary>
    parse_urls_txt();

    if (urls.empty())
    {
        std::cout << "urls empty" << std::endl;
        return -2;
    }

    //��־
    if(!log_output)
        HnwHttp_SetLogCB(nullptr);

    //�������·��
    boost::system::error_code ec;
    if (!boost::filesystem::exists(out_dir))
    {
        if (!boost::filesystem::create_directories(out_dir, ec))
        {
            std::cout << "create_directories " << out_dir << " error ec.what=" << ec.message() << std::endl;
            return -3;
        }
    }
    //���߳�����
    std::cout << "try download urls " << urls.size() << std::endl;
    for (auto url : urls)
    {
        ++request_count_;
        std::string output = out_dir + url.substr(url.find_last_of('/'));
        
        if (false == overlay_file)
        {
            //������,���ھͲ��ظ���������
            if (boost::filesystem::exists(output))
            {
                std::cout << "file "<<output<<" exist,skip "<<url << std::endl;
                continue;
            }
        }
        auto ret = HnwHttp_RequestGet(url,
            std::bind(on_response,
                std::placeholders::_1,
                std::placeholders::_2, output, url), request_timeout, head);
        
        std::cout << "request url=" << url << "->output " << output << " ret=" << (int)ret << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(request_delay));
    }
    std::cout << "request urls " << urls.size() <<" sended "<< std::endl;

	while (!exit_flag)
	{
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
    std::cout << "request urls " << urls.size() << " and response " <<response_count_<<std::endl;
    getchar();
	return 0;
}