cmake_minimum_required (VERSION 3.8)

#project ("HnwExample")

#include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/Boost.cmake)

include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/base)
include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/http)
include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/ssh)

add_executable(HnwTcpClient "tcpclient/main.cpp")
target_link_libraries(HnwTcpClient  HnwBase ${DEFAULT_LIB})

add_executable(HnwTcpSendFile "tcpsendfile/main.cpp")
target_link_libraries(HnwTcpSendFile  HnwBase ${DEFAULT_LIB})

add_executable(HnwTcpServer "tcpserver/main.cpp")
target_link_libraries(HnwTcpServer  HnwBase ${DEFAULT_LIB})

add_executable(HnwUdp "udp/main.cpp")
target_link_libraries(HnwUdp  HnwBase ${DEFAULT_LIB})

add_executable(HnwUdpBroadCast "UdpBroadCast/main.cpp")
target_link_libraries(HnwUdpBroadCast  HnwBase ${DEFAULT_LIB})

add_executable(HnwDns "dns/main.cpp")
target_link_libraries(HnwDns  HnwBase ${DEFAULT_LIB})

add_executable(HnwHttpClient "httpclient/main.cpp")
target_link_libraries(HnwHttpClient  HnwHttp ${DEFAULT_LIB})

add_executable(HnwHttpServer "httpserver/main.cpp")
target_link_libraries(HnwHttpServer  HnwHttp ${DEFAULT_LIB})

add_executable(HnwHttpGetResourceStatus "HttpResourceStatus/HttpResourceStatus.cpp")
target_link_libraries(HnwHttpGetResourceStatus  HnwHttp ${DEFAULT_LIB})

add_executable(HnwHttpDownload "HttpDownload/HttpDownload.cpp")
target_link_libraries(HnwHttpDownload  HnwHttp ${DEFAULT_LIB})

add_executable(HnwHttpCookie "HttpCookie/HttpCookie.cpp")
target_link_libraries(HnwHttpCookie  HnwHttp ${DEFAULT_LIB})

add_executable(HnwFileMap "filemap/filemap.cpp")
target_link_libraries(HnwFileMap  Boost::filesystem)

add_executable(HnwSTest "test/test.cpp")
target_link_libraries(HnwSTest  )

add_executable(HnwFileShared "FileService/main.cpp" "FileService/service.h" "FileService/service.cpp")
target_link_libraries(HnwFileShared  HnwHttp Boost::filesystem Boost::program_options ${DEFAULT_LIB})
#include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/Boost.cmake)
#include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/OpenSSL.cmake)
#√ª”–‘Ú ß∞‹
if(OpenSSL_FOUND)
add_executable(SSL_Test "SSLTest/main.cpp")
target_link_libraries(SSL_Test OpenSSL::SSL OpenSSL::Crypto ${DEFAULT_LIB})
endif()


#“¿¿µ LibSsh2
if(OpenSSL_FOUND)
	find_package(Libssh2)
if(Libssh2_FOUND)
	ADD_DEFINITIONS(-D_HNW_USE_LIBSSH2=1)
	include_directories(${Libssh2_INCLUDE_DIR})
	link_directories(${Libssh2_LIBRARY_DIR})
elseif()
	message(STATUS "Libssh2 not fonud")
endif()
endif()

#include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/LibSsh2.cmake)
if(Libssh2_FOUND)
add_executable(SSHClient "sshclient/main.cpp")
target_link_libraries(SSHClient Libssh2::libssh2 ${DEFAULT_LIB})
endif()

install(TARGETS HnwTcpClient HnwTcpServer HnwUdp HnwDns HnwHttpClient HnwHttpServer SSL_Test
	RUNTIME DESTINATION bin
)
#target_include_directories(HnwTcpClient ${PROJECT_SOURCE_DIR}/../Base)