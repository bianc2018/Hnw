cmake_minimum_required (VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/base)
include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/http)
include_directories(${PROJECT_SOURCE_DIR}/Hnw/Src/ssh)

add_executable(HnwFilesDownloader "FilesDownloader.cpp")

target_link_libraries(HnwFilesDownloader  HnwHttp Boost::filesystem Boost::program_options ${DEFAULT_LIB})
