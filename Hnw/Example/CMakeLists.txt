cmake_minimum_required (VERSION 3.8)

#project ("HnwExample")

include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/Boost.cmake)

include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/base)
include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/http)

add_executable(HnwTcpClient "tcpclient/main.cpp")
target_link_libraries(HnwTcpClient  HnwBase )

add_executable(HnwTcpServer "tcpserver/main.cpp")
target_link_libraries(HnwTcpServer  HnwBase)

add_executable(HnwUdp "udp/main.cpp")
target_link_libraries(HnwUdp  HnwBase)

add_executable(HnwDns "dns/main.cpp")
target_link_libraries(HnwDns  HnwBase)

add_executable(HnwHttpClient "httpclient/main.cpp")
target_link_libraries(HnwHttpClient  HnwHttp)

add_executable(HnwHttpServer "httpserver/main.cpp")
target_link_libraries(HnwHttpServer  HnwHttp)

include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/Boost.cmake)
include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/OpenSSL.cmake)

add_executable(SSL_Test "SSLTest/main.cpp")
target_link_libraries(SSL_Test  OpenSSL::SSL OpenSSL::Crypto)

install(TARGETS HnwTcpClient HnwTcpServer HnwUdp HnwDns
	RUNTIME DESTINATION Hnw/Example
)
#target_include_directories(HnwTcpClient ${PROJECT_SOURCE_DIR}/../Base)