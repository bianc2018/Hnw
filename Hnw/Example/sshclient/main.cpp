#include "ssh_service.hpp"

int main(int argc, char* argv[])
{
	ssh::Service::init();

	ssh::Session s("192.168.1.105");
	s.Connect("root", "root");
	auto ch = s.CreateChannel();
	ch->Write("ls -l");
	std::cout << ch->Read();
	return 0;
}