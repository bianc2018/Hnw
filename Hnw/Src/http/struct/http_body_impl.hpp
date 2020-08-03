/*
    基础body实例
*/
#ifndef HNW_HTTP_BODY_BASE_HPP_
#define HNW_HTTP_BODY_BASE_HPP_
#include "../hnw_http.h"
#include "../util/http_util.hpp"
namespace hnw
{

    namespace http
    {
        class BaseBodyImpl :public HnwHttpBody
        {
        public:
            BaseBodyImpl(
                HTTP_BODY_TYPE type = BODY_EMPTY,
                std::uint64_t read_pos = 0,
                std::uint64_t write_pos = 0,
                HNW_READ_CB read_cb = nullptr,
                HNW_WRITE_CB write_cb = nullptr)
                :type_(type),
                read_pos_(read_pos), write_pos_(write_pos),
                read_cb_(read_cb), write_cb_(write_cb)
            {

            }

            //是否是完整的报文
            virtual bool is_complete()
            {
                return true;
            }
            //存放数据的形式
          /*  virtual void body_type(HTTP_BODY_TYPE type)
            {
                type_ = type;
            }*/
            virtual HTTP_BODY_TYPE body_type()
            {
                return type_;
            }
            // 通过 HnwHttpBody 继承
            virtual std::uint64_t body_size()
            {
                return 0;
            }
            virtual void read_cb(HNW_READ_CB cb)
            {
                read_cb_ = cb;
            }
            virtual HNW_READ_CB read_cb()
            {
                return read_cb_;
            }
            virtual void write_cb(HNW_WRITE_CB cb)
            {
                write_cb_ = cb;
            }
            virtual HNW_WRITE_CB write_cb()
            {
                return write_cb_;
            }

            virtual void read_pos(std::uint64_t pos)
            {
                read_pos_ = pos;
            }
            virtual std::uint64_t read_pos()
            {
                return read_pos_;
            }
            virtual void write_pos(std::uint64_t pos)
            {
                write_pos_ = pos;
            }
            virtual std::uint64_t write_pos()
            {
                return write_pos_;
            }
            virtual size_t read_body(char* buff, size_t buff_size)
            {
                size_t read_size = 0;
                if (read_cb_)
                {
                    read_size = read_cb_(buff, buff_size, read_pos_);
                }
                else
                {
                    read_size = read_body_cb(buff, buff_size, read_pos_);
                }
                read_pos_ += read_size;
                return read_size;
            }
            virtual size_t write_body(const char* buff, size_t buff_size)
            {
                size_t write_size = 0;
                if (write_cb_)
                {
                    write_size = write_cb_(buff, buff_size, write_pos_);
                }
                else
                {
                    write_size = write_body_cb(buff, buff_size, write_pos_);
                }
                write_pos_ += write_size;
                return write_size;
            }
            virtual size_t read_body_cb(char* buff, size_t buff_size, std::uint64_t start_pos)
            {
                return 0;
            }
            virtual size_t write_body_cb(const char* buff, size_t buff_size, std::uint64_t start_pos)
            {
                return 0;
            }
        public:
            static SPHnwHttpBody generate()
            {
                return make_shared_safe<BaseBodyImpl>();
            }
        private:
            HTTP_BODY_TYPE type_;
            uint64_t read_pos_, write_pos_;
            HNW_READ_CB read_cb_;
            HNW_WRITE_CB write_cb_;
        };
    }
}
#endif