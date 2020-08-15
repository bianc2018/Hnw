/*
	http 会话状态
*/
#ifndef HNW_HTTP_SESSION_HPP_
#define HNW_HTTP_SESSION_HPP_

#include <thread>
#include <queue>
#include <mutex>
#include <atomic>

#include "../hnw_http.h"
#include "../parser/http_request_parser.hpp"
#include "../parser/http_response_parser.hpp"
namespace hnw
{
	namespace http
	{
		enum EN_SEND_STATUS
		{
			//line
			EN_SEND_LINE,
			EN_SEND_HEAD,
			EN_SEND_BODY,
		};
		//发送状态
		struct SendStaus
		{
			EN_SEND_STATUS status;
			SPHnwHttpResponse response;
			SPHnwHttpRequest  request;
			std::string cache;
			SendStaus()
				:status(EN_SEND_LINE)
			{

			}
		};

		//为了兼容日志 
		class Session :public std::enable_shared_from_this<Session> /*:public hnw::parser::ParserBase*/
		{
			
		public:
			Session(HNW_HANDLE handle, PeerType type, HNW_EVENT_CB cb)
				:/*hnw::parser::ParserBase(handle),*/
				handle_(handle),
				type_(type), recv_parser_(nullptr),
				send_flag_(false),
				connect_flag_(false),
				close_after_send_flag_(false)
			{
				session_head_ = http::HeadImpl::generate();
				if (type_ == Client)
					add_session_head(util::HTTP_UA, util::HNW_HTTP_UA);
				add_session_head(util::HTTP_ACCEPT, util::ACCEPT_ALL);//
				add_session_head(util::HTTP_CONN, util::HTTP_CONN_CLOSE);
				//根据类型创建解析器
				if (type_ == PeerType::Server)
					recv_parser_ = make_shared_safe<hnw::parser::HttpRequestParser>(handle_);
				else if (type_ == PeerType::Client)
					recv_parser_ = make_shared_safe<hnw::parser::HttpResponseParser>(handle_);
				if (nullptr == recv_parser_)
				{
					PRINTFLOG(BL_ERROR, "create recv_parser_ fail");
					//return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
				}
				else
				{
					recv_parser_->set_event_cb(cb);
				}

				HnwBase_SetEvntCB(handle, cb);
			}
			//init
			//HNW_BASE_ERR_CODE init()
			//{
			//	//根据类型创建解析器
			//	if (type_ == PeerType::Server)
			//		recv_parser_ = make_shared_safe<hnw::parser::HttpRequestParser>(handle_);
			//	else if (type_ == PeerType::Client)
			//		recv_parser_ = make_shared_safe<hnw::parser::HttpResponseParser>(handle_);
			//	if (nullptr == recv_parser_)
			//	{
			//		PRINTFLOG(BL_ERROR, "create recv_parser_ fail");
			//		return HNW_BASE_ERR_CODE::HNW_BASE_ALLOC_FAIL;
			//	}
			//	recv_parser_->set_event_cb(event_cb_);
			//	recv_parser_->set_log_cb(log_cb_);
			//	recv_parser_->set_make_shared_cb(make_shared_);
			//	return HNW_BASE_ERR_CODE::HNW_BASE_OK;
			//}

			//发送
			HNW_BASE_ERR_CODE send(std::shared_ptr<HnwHttpResponse> response)
			{
				if (nullptr == response)
				{
					PRINTFLOG(BL_DEBUG, "response is nullptr");
					//return start_send();
					return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
				}
				//检查类型 服务端才可以发送请求
				if (Server == type_)
				{
					//入列
					{
						std::lock_guard<std::mutex> lk(send_lock_);
						response_queue_.push(response);
					}
				}
				else
				{
					//不支持的
					return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
				}
				return start_send();
			}

			HNW_BASE_ERR_CODE send(std::shared_ptr<HnwHttpRequest> request)
			{
				if (nullptr == request)
				{
					PRINTFLOG(BL_DEBUG, "request is nullptr");
					//return start_send();
					return HNW_BASE_ERR_CODE::HNW_BASE_PARAMS_IS_INVALID;
				}
				//检查类型 服务端才可以发送请求
				if (Client == type_)
				{
					//入列
					{
						std::lock_guard<std::mutex> lk(send_lock_);
						request_queue_.push(request);
					}
				}
				else
				{
					//不支持的
					return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
				}
				return start_send();
			}

			HNW_BASE_ERR_CODE recv_data(char* data,
				size_t data_len)
			{
				if (recv_parser_)
					return recv_parser_->input_data((unsigned char*)data, data_len);
				else
					return HNW_BASE_ERR_CODE::HNW_BASE_NO_SUPPORT;
			}

			//可以继承 各种回调
			static std::shared_ptr<Session> generate(HNW_HANDLE handle, PeerType type,HNW_EVENT_CB cb)
			{
				std::shared_ptr<Session> sp = make_shared_safe<Session>(handle, type,cb);
				/*sp->set_log_cb(log_cb_);
				sp->set_event_cb(event_cb_);
				sp->set_make_shared_cb(make_shared_);*/
				return sp;
			}
			
			//原始回调
			/*void even_cb(std::int64_t handle, int tt, std::shared_ptr<void> event_data)
			{
				if (event_cb_)
					event_cb_(handle, tt, event_data);
				else
				{
					PRINTFLOG(BL_WRAN, "event cb is nullptr");
				}
			}*/

			//链接建立
			void on_connect_establish()
			{
				connect_flag_ = true;
				start_send();
			}

			//断开
			void on_disconnect()
			{
				connect_flag_ = false;
				clear();
			}

			//session 会话键值 覆盖
			void add_session_head(const std::string& key, const std::string& value)
			{
				session_head_->del_head(key);
				session_head_->add_head(key, value);
			}
		private:
			void clear()
			{
				////防止自引用
				//if (make_shared_)
				//	make_shared_ = nullptr;

				/*if(event_cb_)
					event_cb_ = nullptr;*/

			/*	if(log_cb_)
					log_cb_ = nullptr;*/
				return;

			}

			//移动buff
			size_t move_cache(char* buff, size_t buff_size, std::string& cache)
			{
				size_t copy_size = 0;
				if (cache.size() <= buff_size)
				{
					copy_size = cache.size();
				}
				else
				{
					//全部拿了
					copy_size = buff_size;
				}
				memcpy(buff, cache.c_str(), copy_size);
				cache = cache.substr(copy_size);
				return copy_size;
			}

			//数据读取回调
			size_t on_read(char* buff, size_t buff_size)
			{
				auto self = shared_from_this();
				if (nullptr == self)
				{
					PRINTFLOG(BL_ERROR, "[%llu] is released,dont read", handle_);;
					//发送过程中 保持活跃
					return 0;
				}
				if (Server == type_)
					return on_read_response(buff, buff_size);
				else if (Client == type_)
					return on_read_request(buff, buff_size);
				return 0;
			}
			
			size_t on_read_request(char* buff, size_t buff_size)
			{
				while (true)
				{
					//取一个数据
					if (nullptr == send_status_.request)
					{
						std::lock_guard<std::mutex> lk(send_lock_);
						if (request_queue_.empty())
						{
							PRINTFLOG(BL_DEBUG, "[%lld] all request is sended", handle_);
							send_flag_ = false;
							if (close_after_send_flag_)
							{
								PRINTFLOG(BL_DEBUG, "[%lld] close_after_send_flag_ is on", handle_);
								//auto self = shared_from_this();//保持
								HnwHttp_Close(handle_);
							}
							//结束
							return 0;
						}
						send_status_.request = request_queue_.front();
						request_queue_.pop();
						send_status_.status = EN_SEND_LINE;
						pre_send(send_status_.request);
						send_status_.cache = send_status_.request->line->string()+util::CRLF;
					}

					if (EN_SEND_LINE == send_status_.status)
					{
						if (send_status_.cache.empty())
						{
							send_status_.status = EN_SEND_HEAD;
							send_status_.cache = send_status_.request->head->string() + util::CRLF;
						}
						else
						{
							return move_cache(buff, buff_size, send_status_.cache);
						}
					}
					if (EN_SEND_HEAD == send_status_.status)
					{
						if (send_status_.cache.empty())
						{
							send_status_.status = EN_SEND_BODY;
						}
						else
						{
							return move_cache(buff, buff_size, send_status_.cache);
						}
					}
					if (EN_SEND_BODY == send_status_.status)
					{
						auto read_size = send_status_.request->body->read_body(buff, buff_size);
						if (0 == read_size)
						{
							//memcpy(buff, util::CRLF.c_str(), util::CRLF.size());
							send_status_.request = nullptr;
							//return util::CRLF.size();
							return 0;
						}
						else
						{
							return read_size;
						}
					}
				}
			}
			
			size_t on_read_response(char* buff, size_t buff_size)
			{
				while (true)
				{
					//取一个数据
					if (nullptr == send_status_.response)
					{
						//bool is_end = false;

						/*{*/
						std::lock_guard<std::mutex> lk(send_lock_);
						if (response_queue_.empty())
						{
							PRINTFLOG(BL_DEBUG, "[%lld] all request is sended", handle_);
							send_flag_ = false;
							if (close_after_send_flag_)
							{
								PRINTFLOG(BL_DEBUG, "[%lld] close_after_send_flag_ is on", handle_);
								//auto self = shared_from_this();//保持
								HnwHttp_Close(handle_);
							}
							return 0;
						}
						else
						{
							//is_end = false;
							send_status_.response = response_queue_.front();
							response_queue_.pop();
							send_status_.status = EN_SEND_LINE;
							pre_send(send_status_.response);
							send_status_.cache = send_status_.response->line->string() + util::CRLF;
						}
						//}

						//if (is_end)
						//{
						//	//结束
						//	if (close_after_send_flag_)
						//	{
						//		PRINTFLOG(BL_DEBUG, "[%lld] close_after_send_flag_ is on", handle_);
						//	//	HnwHttp_Close(handle_);
						//	}
						//	return 0;
						//}
					}

					if (EN_SEND_LINE == send_status_.status)
					{
						if (send_status_.cache.empty())
						{
							send_status_.status = EN_SEND_HEAD;
							send_status_.cache = send_status_.response->head->string() + util::CRLF;
						}
						else
						{
							return move_cache(buff, buff_size, send_status_.cache);
						}
					}
					if (EN_SEND_HEAD == send_status_.status)
					{
						if (send_status_.cache.empty())
						{
							send_status_.status = EN_SEND_BODY;
						}
						else
						{
							return move_cache(buff, buff_size, send_status_.cache);
						}
					}
					if (EN_SEND_BODY == send_status_.status)
					{
						auto read_size = send_status_.response->body->read_body(buff, buff_size);
						if (0 == read_size)
						{
							send_status_.response = nullptr;
						}
						else
						{
							return read_size;
						}
					}
				}
			}
			
			HNW_BASE_ERR_CODE start_send()
			{
				//发送中 直接入列
				if (send_flag_)
				{
					return HNW_BASE_ERR_CODE::HNW_BASE_OK;
				}
				//没有发送启动发送
				else
				{
					if (false == connect_flag_)
					{
						return HNW_BASE_ERR_CODE::HNW_HTTP_SEND_PENDING;
					}

					send_flag_ = true;
					return HnwBase_Send_Cb(handle_, std::bind(&Session::on_read, this,
						std::placeholders::_1, std::placeholders::_2));
				}
			}

			//发送之前
			void pre_send(SPHnwHttpRequest request)
			{
				if (request->head->get_head_count(util::HTTP_LEN) == 0)
				{
					request->head->add_head_uint64(util::HTTP_LEN, request->body->body_size());
				}

				//合并
				request->head->merge(session_head_, false);
			}
			
			void pre_send(SPHnwHttpResponse response)
			{
				if (response->head->get_head_count(util::HTTP_LEN) == 0)
				{
					response->head->add_head_uint64(util::HTTP_LEN, response->body->body_size());
				}
				if (response->head->get_head_count(util::HTTP_CT) == 0)
				{
					response->head->add_head(util::HTTP_CT, util::HTML_MIME);
				}
			
				//合并
				response->head->merge(session_head_, false);

				if (util::HTTP_CONN_CLOSE == response->head->get_head(util::HTTP_CONN,\
					util::HTTP_CONN_CLOSE))
				{
					close_after_send_flag_ = true;
				}
				
			}
			//std::string get_mime_by_url(const std::string &url)
		private:
			//启动参数
			PeerType type_;
			//通信句柄
			HNW_HANDLE handle_;
			//接收解析器
			std::shared_ptr<hnw::parser::ParserBase>  recv_parser_;

			//发送队列
			//type=server
			std::queue<SPHnwHttpResponse> response_queue_;
			//client
			std::queue<SPHnwHttpRequest > request_queue_;

			//发送状态
			SendStaus send_status_;
			//锁
			std::mutex send_lock_;

			//是否在发送中 false 重新启动
			std::atomic_bool send_flag_;

			std::atomic_bool connect_flag_;

			SPHnwHttpHead session_head_;

			//发送之后是否关闭
			std::atomic_bool close_after_send_flag_;
		}; 
	
		//共享指针
		typedef std::shared_ptr<Session> SP;
	}
}
#endif