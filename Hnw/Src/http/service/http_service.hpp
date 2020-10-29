/*
	http 基础服务
*/
#ifndef HNW_HTTP_SERVICE_HPP_
#define HNW_HTTP_SERVICE_HPP_
//#include "hnw_base.h"
#include "../hnw_http.h"
#include "../session/http_session.hpp"
namespace hnw
{

	namespace http
	{
        //下载任务 内部使用
        struct DownloadTask
        {
            std::string url = "";
            std::string output = "";
            HTTP_DOWNLOAD_CB cb;
            SPHnwHttpResponse response = nullptr;
            int fflag= HTTP_FILE_FLAG_CREATE_NEW;/*文件创建指标*/
            int conn_num = 1;
            HttpRange total = HttpRange();/*总的下载范围*/
            HNW_BASE_ERR_CODE error=HNW_BASE_ERR_CODE::HNW_BASE_OK;
        };
        typedef std::shared_ptr<DownloadTask> SharedDownloadTask;
		//引用
        class Service :public parser::ParserBase
        {
        public:
            Service() :parser::ParserBase(INVAILD_HANDLE)
            {}
            ~Service()
            {}

            HNW_BASE_ERR_CODE start(const HttpParam& param, HNW_EVENT_CB cb, HNW_HANDLE& handle)
            {
                util::UrlParam url_param;
                util::parser_url(param.host, url_param);
                HNW_CHANNEL_TYPE chn_type = HNW_CHANNEL_TYPE::TCP_SERVER;

                auto ret = get_base_channel_type(url_param.protocol, param.peer_type, chn_type);
                if (ret != HNW_BASE_ERR_CODE::HNW_BASE_OK)
                    return ret;

                auto point = choose_point(url_param);

                if (NET_INVALID_POINT == point)
                {
                    return HNW_BASE_ERR_CODE::HNW_HTTP_HOST_IS_IVAILD;
                }

                ret = HnwBase_Add_Channnel(chn_type, point, handle);
                if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                {
                    HnwBase_Close(handle);
                    return ret;
                }

                //创建会话
                auto sp = add_session(handle, param.peer_type == Client ? Client : None, bind_sp(cb));
                if (nullptr == sp)
                {
                    PRINTFLOG(BL_ERROR, "cannt create session handle =%llu",handle);
                    HnwBase_Close(handle);
                    return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                }
                sp->add_session_head(util::HTTP_HOST, url_param.host);
                //设置证书
                /*
                HnwBase_Config(handle,SET_SSL_SERVER_CERT_FILE_PATH,
                    (void*)cert_file.c_str(),cert_file.size());
                 return  HnwBase_Config(handle, SET_SSL_SERVER_PRI_KEY_FILE_PATH,
                     (void*)pri_key_file.c_str(), pri_key_file.size());
                */
                if (param.cert_file.size() != 0)
                    HnwBase_Config(handle, SET_SSL_SERVER_CERT_FILE_PATH,
                        (void*)param.cert_file.c_str(), param.cert_file.size());
                if (param.pri_key_file.size() != 0)
                    HnwBase_Config(handle, SET_SSL_SERVER_PRI_KEY_FILE_PATH,
                        (void*)param.pri_key_file.c_str(), param.pri_key_file.size());
                if (param.temp_dh_file.size() != 0)
                    HnwBase_Config(handle, SET_SSL_SERVER_TEMP_DH_FILE_PATH,
                        (void*)param.temp_dh_file.c_str(), param.temp_dh_file.size());
                HnwBase_Config(handle, SET_SERVER_ACCEPT_NUM,
                    (void*)&(param.accept_num), sizeof(param.accept_num));
                //SET_SSL_SERVER_TEMP_DH_FILE_PATH

                if (param.peer_type == Server)
                    ret = HnwBase_Accept(handle);
                else if (param.peer_type == Client)
                    ret = HnwBase_Connect(handle, point);
                else
                    ret = HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;

                if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                {
                    HnwBase_Close(handle);
                    return ret;
                }
                return ret;
            }

            HNW_BASE_ERR_CODE raw_request(HNW_HANDLE handle,
                const std::string& method,
                const std::string& uri,
                const std::string& data,
                SPHnwHttpHead ext_head)
            {
                SPHnwHttpRequest req;
                auto ret = HnwHttp_GenerateRequest(handle, req);
                if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                {
                    PRINTFLOG(BL_ERROR, "HnwHttp_GenerateRequest bad %d", ret);
                    return ret;
                }
                //不做检查
                req->line->method(method);
                req->line->url(uri);

                //合并
                req->head->merge(ext_head);

                req->body = http::RawBodyImpl::generate();
                if (nullptr == req->body)
                {
                    PRINTFLOG(BL_ERROR, "http::RawBodyImpl::generate fail");
                    return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                }
                if (data.size() != req->body->write_body(data.c_str(), data.size()))
                {
                    PRINTFLOG(BL_ERROR, "write req body error %s", data.c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_SEND_DATA_FAIL;
                }
                //发送
                return HnwHttp_Request(handle, req);
            }

            HNW_BASE_ERR_CODE raw_response(HNW_HANDLE handle,
                const std::string& code,//状态码
                const std::string& data,//数据
                SPHnwHttpHead ext_head = nullptr)
            {
                SPHnwHttpResponse res;
                auto ret = HnwHttp_GenerateResponse(handle, res);
                if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                {
                    PRINTFLOG(BL_ERROR, "HnwHttp_GenerateResponse bad %d", ret);
                    return ret;
                }
                //不做检查
                res->line->status_code(code);

                //合并
                res->head->merge(ext_head);

                res->body = http::RawBodyImpl::generate();
                if (nullptr == res->body)
                {
                    PRINTFLOG(BL_ERROR, "http::RawBodyImpl::generate fail");
                    return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                }
                if (data.size() != res->body->write_body(data.c_str(), data.size()))
                {
                    PRINTFLOG(BL_ERROR, "write req body error %s", data.c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_SEND_DATA_FAIL;
                }
                //发送
                return HnwHttp_Response(handle, res);
            }

            HNW_BASE_ERR_CODE file_request(HNW_HANDLE handle,
                const std::string& method,
                const std::string& uri,
                const std::string& filepath,
                SPHnwHttpHead ext_head)
            {
                SPHnwHttpRequest req;
                auto ret = HnwHttp_GenerateRequest(handle, req);
                if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                {
                    PRINTFLOG(BL_ERROR, "HnwHttp_GenerateRequest bad %d", ret);
                    return ret;
                }
                //不做检查
                req->line->method(method);
                req->line->url(uri);

                //合并
                req->head->merge(ext_head);

                /* if (0 == req->head->get_head_count(util::HTTP_CT))
                 {
                     req->head->add_head(util::HTTP_CT, util::get_http_mime_by_uri(filepath));
                 }*/
                HttpRange range;
                req->head->get_range(range);
                req->body = http::FileBodyImpl::generate(filepath, range);
                if (nullptr == req->body)
                {
                    PRINTFLOG(BL_ERROR, "http::FileBodyImpl::generate fail");
                    return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                }
                //发送
                return HnwHttp_Request(handle, req);
            }

            HNW_BASE_ERR_CODE file_response(HNW_HANDLE handle,
                const std::string& code,//状态码
                const std::string& filepath,//数据
                SPHnwHttpHead ext_head = nullptr)
            {
                SPHnwHttpResponse res;
                auto ret = HnwHttp_GenerateResponse(handle, res);
                if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                {
                    PRINTFLOG(BL_ERROR, "HnwHttp_GenerateResponse bad %d", ret);
                    return ret;
                }
                //不做检查
                res->line->status_code(code);

                //合并
                res->head->merge(ext_head);

                if (0 == res->head->get_head_count(util::HTTP_CT))
                {
                    res->head->add_head(util::HTTP_CT, util::get_http_mime_by_uri(filepath));
                }
                HttpRange range;
                res->head->get_content_range(range);
                res->body = http::FileBodyImpl::generate(filepath, range);
                if (nullptr == res->body)
                {
                    PRINTFLOG(BL_ERROR, "http::RawBodyImpl::generate fail");
                    return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                }
                //发送
                return HnwHttp_Response(handle, res);
            }

            HNW_BASE_ERR_CODE recv_file_request(SPHnwHttpRequest request,
                const std::string& filepath, int flag)
            {
                if (nullptr == request)
                {
                    PRINTFLOG(BL_ERROR, "input empty request bad");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }

                HttpRange range;
                auto size = request->head->get_head_int64(util::HTTP_LEN, -1);
                if (size < 0)
                {
                    //考虑 http range
                    if (false == request->head->get_range(range))
                    {
                        PRINTFLOG(BL_ERROR, "not found %s", util::HTTP_LEN.c_str());
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                    }
                }
                else
                {
                    range.total = size;
                }

                request->body = http::FileBodyImpl::generate(filepath, range, flag);
                if (nullptr == request->body)
                {
                    PRINTFLOG(BL_ERROR, " http::FileBodyImpl::generate bad %s", filepath.c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                }
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            HNW_BASE_ERR_CODE recv_file_response(SPHnwHttpResponse response,
                const std::string& filepath, int flag)
            {
                if (nullptr == response)
                {
                    PRINTFLOG(BL_ERROR, "input empty response bad");
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }

                HttpRange range;
                auto size = response->head->get_head_int64(util::HTTP_LEN, -1);
                if (size < 0)
                {
                    //考虑 http range
                    if (false == response->head->get_content_range(range))
                    {
                        PRINTFLOG(BL_ERROR, "not found %s", util::HTTP_LEN.c_str());
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                    }
                }
                else
                {
                    range.total = size;
                }

                response->body = http::FileBodyImpl::generate(filepath, range, flag);
                if (nullptr == response->body)
                {
                    PRINTFLOG(BL_ERROR, " http::FileBodyImpl::generate bad %s", filepath.c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
                }
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //获取对应的会话指针
            /*http::Session* get_session(HNW_HANDLE handle)
            {
                return (http::Session*)HnwBase_GetUserData(handle);
            }*/

            HNW_BASE_ERR_CODE download(const std::string& url,
                const std::string& output,
                HTTP_DOWNLOAD_CB cb,
                size_t con_num,
                const HttpRange range
                , int file_flag)
            {
                if (con_num == 0 || url == "" || output == "")
                {
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
                SharedDownloadTask task = make_shared_safe<DownloadTask>();
                task->cb = cb;
                task->conn_num = con_num;
                task->fflag = file_flag;
                task->output = output;
                task->total.use_start_endpoint = range.use_start_endpoint;
                task->total.use_end_endpoint = range.use_end_endpoint;
                task->total.total = range.total;
                task->total.start = range.start;
                task->total.end = range.end;
                task->url = url;

                return download(task, true);
            }

            HNW_BASE_ERR_CODE download(SharedDownloadTask task, bool bMainTask = false)
            {
                if (nullptr == task)
                {
                    PRINTFLOG(BL_ERROR, "task is nullptr");
                }

                if (true == bMainTask || task->total.total == 0)
                {
                    HTTP_RES_STATUS_CB cb = [task, this](HttpResourceStatus status,
                        HNW_BASE_ERR_CODE error)
                    {
                        if (HNW_BASE_ERR_CODE::HNW_BASE_OK != error)
                        {
                            task->error = error;
                            on_download_status_change(HNW_INVALID_HANDLE, task);
                        }
                        else
                        {
                            if (status.status == util::HTTP_STATUS_OK)
                            {
                                util::do_file_by_flag(task->output, status.size, task->fflag);
                                if (status.size == 0)
                                {
                                    on_download_status_change(HNW_INVALID_HANDLE, task);
                                    return;
                                }
                                //单线程下载
                                task->total.use_start_endpoint = true;
                                task->total.use_end_endpoint = true;
                                task->total.total = status.size;
                                task->conn_num = 1;
                                task->fflag = HTTP_FILE_FLAG_OPEN;
                                download(task);
                            }
                            else if (status.status == util::HTTP_STATUS_MOVE_P ||
                                status.status == util::HTTP_STATUS_MOVE_T)
                            {
                                //redirect
                                PRINTFLOG(BL_WRAN, "redirect %s->%s%s", \
                                    task->url.c_str(), status.host.c_str(), status.location.c_str());
                                task->url = status.host + status.location;
                                download(task);
                            }
                            else if (status.status == util::HTTP_STATUS_PARTIAL)
                            {
                                util::do_file_by_flag(task->output, status.size, task->fflag);
                                if (status.size == 0)
                                {
                                    on_download_status_change(HNW_INVALID_HANDLE, task);
                                    return;
                                }

                                task->total.total = status.size;
                                //每个线程的下载大小
                                auto s_size = task->total.section_size() / task->conn_num;

                                if (s_size == 0)
                                {
                                    PRINTFLOG(BL_ERROR, "s_size ==0");
                                    task->error = HNW_BASE_ERR_CODE::HNW_HTTP_BAD_MESSAGE;
                                    on_download_status_change(HNW_INVALID_HANDLE, task);
                                    return;
                                }

                                std::uint64_t start = task->total.section_start();
                                std::uint64_t end = task->total.section_end();
                                //文件创建和下载
                                for (int i = 0; i < task->conn_num - 1; ++i)
                                {
                                    SharedDownloadTask subtask = make_shared_safe<DownloadTask>();
                                    subtask->cb = task->cb;
                                    subtask->conn_num = 1;
                                    subtask->fflag = HTTP_FILE_FLAG_OPEN;
                                    subtask->output = task->output;
                                    subtask->total.use_start_endpoint = false;
                                    subtask->total.use_end_endpoint = false;
                                    subtask->total.total = task->total.total;
                                    subtask->total.start = start;
                                    subtask->total.end = start + s_size;
                                    subtask->url = task->url;
                                    //前进
                                    start = subtask->total.end + 1;
                                    auto ret = download(subtask);
                                    if (ret != HNW_BASE_ERR_CODE::HNW_BASE_OK)
                                    {
                                        PRINTFLOG(BL_ERROR, "download sub task error i=%d", i);
                                        subtask->error = ret;
                                        on_download_status_change(HNW_INVALID_HANDLE, subtask);
                                    }
                                }

                                SharedDownloadTask subtask = make_shared_safe<DownloadTask>();
                                subtask->cb = task->cb;
                                subtask->conn_num = 1;
                                subtask->fflag = HTTP_FILE_FLAG_OPEN;
                                subtask->output = task->output;
                                subtask->total.use_start_endpoint = false;
                                // subtask->total.use_end_endpoint = false;
                                subtask->total.total = task->total.total;
                                subtask->total.start = start;
                                subtask->total.use_end_endpoint = true;
                                subtask->url = task->url;
                                auto ret = download(subtask);
                                if (ret != HNW_BASE_ERR_CODE::HNW_BASE_OK)
                                {
                                    PRINTFLOG(BL_ERROR, "download sub last task error ");
                                    subtask->error = ret;
                                    on_download_status_change(HNW_INVALID_HANDLE, subtask);
                                }
                            }
                            else
                            {
                                PRINTFLOG(BL_ERROR, "HttpResourceStatus %s is %s",
                                    task->url.c_str(), status.status.c_str());
                                task->error = HNW_BASE_ERR_CODE::HNW_HTTP_RES_IS_INVALID;
                                on_download_status_change(HNW_INVALID_HANDLE, task);
                            }
                        }
                    };
                    return get_resource_status(task->url, cb);
                }
                else
                {
                    HttpParam param;
                    param.host = task->url;
                    param.peer_type = Client;

                    HNW_HANDLE handle = HNW_INVALID_HANDLE;
                    HNW_EVENT_CB event_cb = std::bind(&Service::download_cb,
                        this, task,
                        std::placeholders::_1,
                        std::placeholders::_2,
                        std::placeholders::_3);

                    return start(param, event_cb, handle);
                }
            }

            //获取资源状态
            HNW_BASE_ERR_CODE get_resource_status(const std::string& url,
                HTTP_RES_STATUS_CB cb)
            {
                HttpResourceStatus status;
                status.host = util::get_host_for_url(url);
                status.path = util::get_path_for_url(url);
                if (status.path.empty())
                    status.path = "/";

                if (status.host.empty())
                {
                    PRINTFLOG(BL_ERROR,
                        "get_resource_status %s get_host_for_url %s fail"
                        , url.c_str(), status.host.c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
                HttpParam param;
                param.host = url;
                param.peer_type = Client;

                HNW_HANDLE handle = HNW_INVALID_HANDLE;
                HNW_EVENT_CB event_cb = std::bind(&Service::resource_status_cb,
                    this, status, cb,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3);

                return start(param, event_cb, handle);
            }

            HNW_BASE_ERR_CODE request(const std::string& method,
                const std::string& url, HNW_ON_RESPONSE cb, const std::string& body="",
                size_t time_out_ms = 10000, SPHnwHttpHead ext_head = nullptr)
            {
               /* HttpResourceStatus status;
                status.host = util::get_host_for_url(url);
                status.path = util::get_path_for_url(url);
                if (status.path.empty())
                    status.path = "/";

                if (status.host.empty())
                {
                    PRINTFLOG(BL_ERROR,
                        "get_resource_status %s get_host_for_url %s fail"
                        , url.c_str(), status.host.c_str());
                    return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }*/

                HttpParam param;
                param.host = url;
                param.peer_type = Client;

                HNW_HANDLE handle = HNW_INVALID_HANDLE;
                HNW_EVENT_CB event_cb = std::bind(&Service::on_request_once,
                    this, cb, method, util::get_path_for_url(url),body, ext_head,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3);
                
               // return start(param, event_cb, handle);
                auto ret = start(param, event_cb, handle);
                if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                    return ret;
                //设置超时
                return HnwBase_Config(handle, SET_RECV_TIME_OUT_MS, &time_out_ms, sizeof(time_out_ms));
            }
        private:
            //根据输入的协议类型选择基层通道类型
            HNW_BASE_ERR_CODE get_base_channel_type(std::string protocol, PeerType peer_type,
                HNW_CHANNEL_TYPE& chn_type)
            {
                //默认 http
                if ("" == protocol)
                {
                    protocol = "http";
                }

                if ("http" == protocol)
                {
                    if (peer_type == Server)
                        chn_type = HNW_CHANNEL_TYPE::TCP_SERVER;
                    else if (peer_type == Client)
                        chn_type = HNW_CHANNEL_TYPE::TCP_CLIENT;
                    else
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
                else if ("https" == protocol)
                {
                    if (peer_type == Server)
                        chn_type = HNW_CHANNEL_TYPE::SSL_SERVER;
                    else if (peer_type == Client)
                        chn_type = HNW_CHANNEL_TYPE::SSL_CLIENT;
                    else
                        return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
                }
                return HNW_BASE_ERR_CODE::HNW_BASE_OK;
            }

            //选择一个端点
            NetPoint choose_point(const util::UrlParam& url_param)
            {
                //dns
                std::vector<NetPoint> point_ver;
                auto ret = HnwBase_QueryDNS(url_param.host, point_ver, url_param.protocol.empty() ? "http" : url_param.protocol);
                if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                {
                    return NET_INVALID_POINT;
                }
                if (point_ver.size() == 0)
                {
                    return NET_INVALID_POINT;
                }

                NetPoint point = point_ver[0];
                //非空
                if (!url_param.port.empty())
                {
                    try
                    {
                        point.port = std::stoi(url_param.port);
                    }
                    catch (const std::exception&)
                    {
                        return NET_INVALID_POINT;
                    }

                }
                return point;
            }

            HNW_EVENT_CB bind_sp(HNW_EVENT_CB cb)
            {
                return std::bind(&Service::event_cb, this, cb,
                    std::placeholders::_1, std::placeholders::_2
                    , std::placeholders::_3);
            }

            void event_cb(HNW_EVENT_CB cb, std::int64_t handle, int tt, std::shared_ptr<void> event_data)
            {
                HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)tt;

                auto sp = get_session(handle);
                if (nullptr == sp)
                {
                    PRINTFLOG(BL_ERROR, "handle %llu session is no exist", handle);
                    HnwBase_Close(handle);//关闭
                    del_session(handle);//空的也关闭
                    return;
                }
                if (HNW_BASE_EVENT_TYPE::HNW_BASE_RECV_DATA == type)
                {
                    auto data = PTR_CAST(HnwBaseRecvDataEvent, event_data);
                    sp->recv_data((char*)data->buff, data->buff_len);
                }
                else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ACCEPT == type)
                {
                    auto data = PTR_CAST(HnwBaseAcceptEvent, event_data);
                    //创建一个客户端会话
                    auto clinet = add_session(data->client, Server,bind_sp(cb));
                    if(clinet)
                        clinet->on_connect_establish();
                    else
                    {
                        PRINTFLOG(BL_ERROR, "add_session bad!");
                        HnwBase_Close(handle);
                    }
                }
                else if (HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH == type)
                {
                    sp->on_connect_establish();
                }
                else if (HNW_BASE_EVENT_TYPE::HNW_BASE_CLOSED == type)
                {
                    //HnwBase_SetEvntCB(handle,nullptr);
                    sp->on_disconnect();
                    del_session(handle);
                }
                //其他不管
                if (cb)
                    cb(handle, tt, event_data);
            }

            //状态更改
            bool on_download_status_change(std::int64_t handle,
                SharedDownloadTask task)
            {
                if (task)
                {
                    if (task->cb)
                    {
                        std::uint64_t pos = 0;
                        if (task->response && task->response->body)
                        {
                            pos = task->response->body->write_pos();
                        }
                        auto ret = task->cb(task->url, task->output,
                            task->total, pos,
                            task->error);

                        //错误或者下载完毕 可以不用再次回调了
                        if (!ret || HNW_BASE_ERR_CODE::HNW_BASE_OK != task->error || task->total.section_size() == pos)
                        {
                            if (handle != HNW_INVALID_HANDLE)
                            {
                                HnwBase_SetEvntCB(handle, nullptr);
                                HnwBase_Close(handle);
                            }
                            return false;
                        }

                        return true;
                    }
                }
                return false;
            }

            //下载回调 
            void download_cb(SharedDownloadTask task,
                std::int64_t handle, int tt, std::shared_ptr<void> event_data)
            {
                HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)tt;

                if (HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH == type)
                {
                    //请求HEAD信息
                    SPHnwHttpHead head;
                    HnwHttp_GenerateHead(handle, head);
                    head->set_range(task->total);
                    task->error = HnwHttp_RawRequest(handle, util::HTTP_METHOD_GET,
                        util::get_path_for_url(task->url), "", head);
                }
                else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_RESPONSE_HEAD == type)
                {
                    //记录
                    auto response = PTR_CAST(HnwHttpResponse, event_data);
                    if (task->response)
                    {
                        PRINTFLOG(BL_WRAN, "only recv one response");
                        return;
                    }
                    task->response = response;
                    if (task->response->line->status_code() == util::HTTP_STATUS_OK)
                    {
                        task->total.use_end_endpoint = task->total.use_start_endpoint = true;
                        task->total.total = response->head->get_head_int64(util::HTTP_LEN, 0);
                        task->error = HNW_BASE_ERR_CODE::HNW_HTTP_NOT_SUPPORT_RANGE;
                        if (!on_download_status_change(handle, task))
                        {
                            return;
                        }
                        task->error = HNW_BASE_ERR_CODE::HNW_BASE_OK;
                    }
                    else if (task->response->line->status_code() == util::HTTP_STATUS_PARTIAL)
                    {
                        response->head->get_content_range(task->total);
                    }
                    //下载
                    auto ret = HnwHttp_RecvFileResponse(task->response,
                        task->output, task->fflag);
                    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                    {
                        //接收失败
                        task->error = ret;
                        on_download_status_change(handle, task);
                    }
                }
                else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR == type)
                {
                    auto error = PTR_CAST(HnwBaseErrorEvent, event_data);
                    PRINTFLOG(BL_ERROR, "download error %d %s", error->code, error->message.c_str());
                    //接收失败
                    task->error = (HNW_BASE_ERR_CODE)error->code;
                    on_download_status_change(handle, task);
                }
                /* else if (HNW_BASE_EVENT_TYPE::HNW_BASE_SEND_COMPLETE == type)
                 {
                     auto sendevent = PTR_CAST(HnwBaseSendDataEvent, event_data);
                     printf("download send \n%s\n",std::string(sendevent->buff,sendevent->buff_len).c_str());
                 }*/
                on_download_status_change(handle, task);
            }

            //状态回调
            void resource_status_cb(HttpResourceStatus status, HTTP_RES_STATUS_CB cb,
                std::int64_t handle, int tt, std::shared_ptr<void> event_data)
            {
                HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)tt;
                if (HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH == type)
                {
                    //请求HEAD信息
                    HttpRange range;
                    SPHnwHttpHead head;
                    HnwHttp_GenerateHead(handle, head);
                    head->set_range(range);
                    // head->add_head(util::HTTP_HOST, "zhuanlan.zhihu.com");
                    auto ret = HnwHttp_RawRequest(handle, util::HTTP_METHOD_HEAD, status.path, "", head);
                    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                    {
                        if (cb)
                        {
                            cb(status, ret);
                        }

                        HnwBase_Close(handle);
                    }
                }
                else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_RESPONSE_HEAD == type)
                {

                    //记录
                    auto response = PTR_CAST(HnwHttpResponse, event_data);

                    status.size = response->head->get_head_uint64(util::HTTP_LEN, 0);

                    if (response->line->status_code() == util::HTTP_STATUS_PARTIAL)
                    {
                        HttpRange range;
                        response->head->get_content_range(range);
                        status.size = range.total;
                        status.partial_support = true;

                    }
                    else if (response->line->status_code() == util::HTTP_STATUS_MOVE_P
                        || response->line->status_code() == util::HTTP_STATUS_MOVE_T)
                    {
                        status.location = response->head->get_head(util::HTTP_LOC, "");
                    }

                    status.status = response->line->status_code();
                    if (cb)
                    {
                        cb(status, HNW_BASE_ERR_CODE::HNW_BASE_OK);
                    }
                    HnwBase_SetEvntCB(handle, nullptr);
                    HnwBase_Close(handle);
                }
                else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR == type)
                {
                    auto error = PTR_CAST(HnwBaseErrorEvent, event_data);
                    PRINTFLOG(BL_ERROR, "resource_status_cb error %d %s", error->code, error->message.c_str());
                    //接收失败
                    status.status = "500";
                    if (cb)
                    {
                        cb(status, (HNW_BASE_ERR_CODE)error->code);
                    }
                    HnwBase_Close(handle);
                }
            }

            //单次请求
            void on_request_once(HNW_ON_RESPONSE cb,std::string method,std::string path,
                const std::string& body, SPHnwHttpHead ext_head,
                std::int64_t handle, int tt, std::shared_ptr<void> event_data)
            {
                HNW_BASE_EVENT_TYPE type = (HNW_BASE_EVENT_TYPE)tt;
                if (HNW_BASE_EVENT_TYPE::HNW_BASE_CONNECT_ESTABLISH == type)
                {
                    auto ret = HnwHttp_RawRequest(handle, method, path, body, ext_head);
                    if (HNW_BASE_ERR_CODE::HNW_BASE_OK != ret)
                    {
                        if (cb)
                        {
                            cb(nullptr, ret);
                        }

                        HnwBase_Close(handle);
                    }
                }
                else if (HNW_BASE_EVENT_TYPE::HNW_HTTP_RECV_RESPONSE == type)
                {

                    //记录
                    auto response = PTR_CAST(HnwHttpResponse, event_data);
                    if (cb)
                    {
                        cb(response, HNW_BASE_ERR_CODE::HNW_BASE_OK);
                    }
                    HnwBase_SetEvntCB(handle, nullptr);
                    HnwBase_Close(handle);
                }
                else if (HNW_BASE_EVENT_TYPE::HNW_BASE_ERROR == type)
                {
                    auto error = PTR_CAST(HnwBaseErrorEvent, event_data);
                    if (cb)
                    {
                        cb(nullptr, (HNW_BASE_ERR_CODE)error->code);
                    }
                    HnwBase_SetEvntCB(handle, nullptr);
                    HnwBase_Close(handle);
                }
            }

        public:
            //获取会话指针
            SP get_session(HNW_HANDLE handle) 
            {
                std::lock_guard<std::mutex> lk(session_map_lock_);
                auto p = session_map_.find(handle);
                if (session_map_.end() != p)
                {
                    return p->second;
                }
                return nullptr;
            }
        private:
            //新增
            SP add_session(HNW_HANDLE handle, PeerType type, HNW_EVENT_CB cb)
            {
                auto sp = Session::generate(handle, type, cb);
                if (sp)
                {
                    std::lock_guard<std::mutex> lk(session_map_lock_);
                    session_map_[handle] = sp;//已有的覆盖
                }
                return sp;
            }
            //删除
            bool del_session(HNW_HANDLE handle)
            {
                std::lock_guard<std::mutex> lk(session_map_lock_);
                session_map_.erase(handle);//已有的覆盖
                return true;
            }

        private:
            //锁
            std::mutex session_map_lock_;
            //map
            std::map<HNW_HANDLE, SP> session_map_;
        };

	}
}
#endif