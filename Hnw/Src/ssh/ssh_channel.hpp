/*
 ssh ´«ÊäÍ¨µÀ
*/

#ifndef SSH_CHANNEL_HPP_
#define SSH_CHANNEL_HPP_
#include "ssh_define.hpp"
#include <thread>
namespace ssh
{
    const int CHANNEL_READ_TIMEOUT = 3000;

    class Channel
    {
    public:
        Channel(LIBSSH2_CHANNEL* channel):channel_(channel)
        {

        }
        ~Channel(void)
        {
            if (channel_)
            {
                libssh2_channel_free(channel_);
                channel_ = NULL;
            }
        }

        std::string Read(int timeout = CHANNEL_READ_TIMEOUT)
        {
            std::string data;
            if (NULL == channel_)
            {
                return data;
            }

            LIBSSH2_POLLFD* fds = new LIBSSH2_POLLFD;
            fds->type = LIBSSH2_POLLFD_CHANNEL;
            fds->fd.channel = channel_;
            fds->events = LIBSSH2_POLLFD_POLLIN | LIBSSH2_POLLFD_POLLOUT;

           /* if (timeout % 50)
            {
                timeout += timeout % 50;
            }*/

            while (timeout > 0)
            {
                int rc = (libssh2_poll(fds, 1, 10));
                if (rc < 1)
                {
                    timeout -= 50;
                    std::this_thread::sleep_for(std::chrono::seconds(50));
                    //usleep(50 * 1000);
                    continue;
                }

                if (fds->revents & LIBSSH2_POLLFD_POLLIN)
                {
                    char buffer[64 * 1024] = {0};
                    size_t n = libssh2_channel_read(channel_, buffer, sizeof(buffer));
                    if (n == LIBSSH2_ERROR_EAGAIN)
                    {
                        continue;
                    }
                    else if (n <= 0)
                    {
                        break;
                        /*return data;*/
                    }
                    else
                    {
                        data += std::string(buffer);
                        /*if ("" == strend)
                        {
                            return data;
                        }
                        size_t pos = data.rfind(strend);
                        if (pos != string::npos && data.substr(pos, strend.size()) == strend)
                        {
                            return data;
                        }*/
                    }
                }
                timeout -= 50;
                std::this_thread::sleep_for(std::chrono::seconds(50));
                //usleep(50 * 1000);
            }
            delete fds;
            fds = nullptr;
          //  cout << "read data timeout" << endl;
            return data;
        }
        bool   Write(const std::string& data)
        {
            if (NULL == channel_)
            {
                return false;
            }

            std::string send_data = data + "\n";
            return libssh2_channel_write_ex(channel_, 0, send_data.c_str(), send_data.size()) == data.size();
        }
    private:
        Channel(const Channel&)
        {}
        Channel operator=(const Channel&)
        {}
    private:
        LIBSSH2_CHANNEL* channel_;
    };
}
#endif
