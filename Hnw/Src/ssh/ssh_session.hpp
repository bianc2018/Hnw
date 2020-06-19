/*
ssh 会话
*/
#ifndef SSH_SESSION_HPP_
#define SSH_SESSION_HPP_
#include "ssh_channel.hpp"
namespace ssh
{
    class Session
    {
    public:
        Session(const std::string& ip, int port = 22)
            :ip_(ip),port_(port),sock_(-1),session_(nullptr)
        {
            
        }
        ~Session(void)
        {
            Disconnect();
        }

        bool Connect(const std::string& username, const std::string& userpwd)
        {
            sock_ = socket(AF_INET, SOCK_STREAM, 0);

            sockaddr_in sin;
            sin.sin_family = AF_INET;
            sin.sin_port = htons(port_);
            sin.sin_addr.s_addr = inet_addr(ip_.c_str());
            if (connect(sock_, (sockaddr*)(&sin), sizeof(sockaddr_in)) != 0)
            {
                Disconnect();
                return false;
            }

            session_ = libssh2_session_init();
            if (libssh2_session_handshake(session_, sock_))
            {
                Disconnect();
                return false;
            }

            int auth_pw = 0;
            std::string fingerprint = libssh2_hostkey_hash(session_, LIBSSH2_HOSTKEY_HASH_SHA1);
            std::string userauthlist = libssh2_userauth_list(session_, username.c_str(), (int)username.size());
            if (strstr(userauthlist.c_str(), "password") != NULL)
            {
                auth_pw |= 1;
            }
            if (strstr(userauthlist.c_str(), "keyboard-interactive") != NULL)
            {
                auth_pw |= 2;
            }
            if (strstr(userauthlist.c_str(), "publickey") != NULL)
            {
                auth_pw |= 4;
            }

            if (auth_pw & 1)
            {
                /* We could authenticate via password */
                if (libssh2_userauth_password(session_, username.c_str(), userpwd.c_str()))
                {
                    Disconnect();
                    return false;
                }
            }
            else if (auth_pw & 2)
            {
                /* Or via keyboard-interactive */
              /*  s_password = userPwd;
                if (libssh2_userauth_keyboard_interactive(m_session, userName.c_str(), &S_KbdCallback))
                {
                    Disconnect();
                    return false;
                }*/
                exit(-100);
            }
            else
            {
                Disconnect();
                return false;
            }


            return true;

        }
        bool Disconnect(void)
        {
            if(session_)
            {
                libssh2_session_disconnect(session_, "Bye bye, Thank you");
                libssh2_session_free(session_);
                session_ = NULL;
            }
            if (sock_ != -1)
            {
#ifdef WIN32
                closesocket(sock_);
#else
                close(sock_);
#endif
                sock_ = -1;
            }
            return true;
        }

        std::shared_ptr<Channel> CreateChannel(const std::string& ptyType = "vanilla")
        {
            if (NULL == session_)
            {
                return NULL;
            }

            LIBSSH2_CHANNEL* channel = NULL;
            /* Request a shell */
            if (!(channel = libssh2_channel_open_session(session_)))
            {
                return NULL;
            }

            /* Request a terminal with 'vanilla' terminal emulation
             * See /etc/termcap for more options
             */
            if (libssh2_channel_request_pty(channel, ptyType.c_str()))
            {
                libssh2_channel_free(channel);
                return NULL;
            }

            /* Open a SHELL on that pty */
            if (libssh2_channel_shell(channel))
            {

                libssh2_channel_free(channel);
                return NULL;
            }

            auto ret = std::make_shared<Channel>(channel);
           // std::cout << ret->Read() << std::endl;
            return ret;
        }

    public:
       /* static void S_KbdCallback(const char*, int, const char*, int, int, const LIBSSH2_USERAUTH_KBDINT_PROMPT*, LIBSSH2_USERAUTH_KBDINT_RESPONSE*, void** a);
        static string s_password;*/

        //等待事件
        int waitsocket()
        {
            if (nullptr == session_)
                return - 1;

            struct timeval timeout;
            int rc;
            fd_set fd;
            fd_set* writefd = NULL;
            fd_set* readfd = NULL;
            int dir;

            timeout.tv_sec = 10;
            timeout.tv_usec = 0;

            FD_ZERO(&fd);

            FD_SET(sock_, &fd);

            /* now make sure we wait in the correct direction */
            dir = libssh2_session_block_directions(session_);

            if (dir & LIBSSH2_SESSION_BLOCK_INBOUND)
                readfd = &fd;

            if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
                writefd = &fd;

            rc = select(sock_ + 1, readfd, writefd, NULL, &timeout);

            return rc;
        }

    private:
        std::string ip_;
        int    port_;
        /*
        std::string username_;
        std::string password_;*/
        int    sock_;
        LIBSSH2_SESSION* session_;
    };
}
#endif