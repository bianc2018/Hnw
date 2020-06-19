/*
 ssh 的通用文件头
*/
#ifndef SSH_DEFINE_HPP_
#define SSH_DEFINE_HPP_

//#include "libssh2_config.h"
#ifdef WIN32
# include <winsock2.h>
# include <windows.h>

#pragma comment(lib, "WS2_32")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <libssh2.h>
#include <libssh2_sftp.h>




#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include <string>
#include <iostream>
#endif