cmake_minimum_required (VERSION 3.8)

#project ("HnwExample")

include(${PROJECT_SOURCE_DIR}/Hnw/Cmake/Boost.cmake)

include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src)

add_executable(HnwTcpClient "tcpclient/main.cpp")
target_link_libraries(HnwTcpClient  Hnw)

add_executable(HnwTcpServer "tcpserver/main.cpp")
target_link_libraries(HnwTcpServer  Hnw)

add_executable(HnwUdp "udp/main.cpp")
target_link_libraries(HnwUdp  Hnw)

add_executable(HnwDns "dns/main.cpp")
target_link_libraries(HnwDns  Hnw)

install(TARGETS HnwTcpClient HnwTcpServer HnwUdp HnwDns
	RUNTIME DESTINATION Hnw/Example
)
#target_include_directories(HnwTcpClient ${PROJECT_SOURCE_DIR}/../Base)