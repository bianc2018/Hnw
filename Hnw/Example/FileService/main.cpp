/*
	�ļ�������
*/
#include "service.h"
#include <signal.h>
#include <thread>
#include <atomic>
static std::atomic_bool exit_flag = true;;
int main(int argc, char* argv[])
{
	//�����˳��źŴ�����
	signal(SIGINT, [](int) { exit_flag = false; });// �����˳��ź�

	file::FileServce service;
	auto ret =  service.start(argc, argv);
	if (0 == ret)
	{
		while (exit_flag)
		{
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}
	return 0;
}
