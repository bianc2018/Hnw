/*
	c++ ·â×° libssh2
*/
#ifndef HNW_SSH_HPP_
#define HNW_SSH_HPP_
#include "ssh_session.hpp"

namespace ssh
{
	class Service
	{
	public:
		Service()
		{
#ifdef WIN32
			//³õÊ¼»¯WSA  
			WORD sockVersion = MAKEWORD(2, 2);
			WSADATA wsaData;
			if (WSAStartup(sockVersion, &wsaData) != 0)
			{
				return ;
			}
#endif
			libssh2_init(0);
		}
		~Service()
		{
			libssh2_exit();
#ifdef WIN32
			WSACleanup();
#endif
		}
		static Service& init()
		{
			static Service s;
			return s;
		}
	private:

	};

}
#endif
