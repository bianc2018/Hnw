/*
	���ַ���Ϊ������body
*/
#ifndef HNW_HTTP_BODY_STR_HPP_
#define HNW_HTTP_BODY_STR_HPP_
#include "http_body_impl.hpp"
namespace hnw
{
	namespace http
	{
		class RawBodyImpl:public BaseBodyImpl
		{
		public:
            RawBodyImpl(std::uint64_t maximum_capacity, bool auto_increment)
                :BaseBodyImpl(BODY_RAW)
                , maximum_capacity_(maximum_capacity)
                ,increment_(auto_increment)
            {

            }
            virtual std::uint64_t body_size()
            {
                return body_.size();
            }
            //�Ƿ��������ı���
            virtual bool is_complete()
            {
                return increment_||(maximum_capacity_ == body_size());
            }
            virtual size_t read_body_cb(char* buff, size_t buff_size, std::uint64_t start_pos)override
            {
                //ʣ�µ�
                std::int64_t dl = body_.size() - start_pos;
                //������
                if (dl <= 0)
                {
                    return 0;
                }
                //����
                else if (dl >= buff_size)
                {
                    memcpy(buff, body_.c_str()+start_pos, buff_size);
                    return buff_size;
                }
                //ȱ�ٵ�
                else if (dl < buff_size)
                {
                    memcpy(buff, body_.c_str() + start_pos, (size_t)dl);
                    return (size_t)dl;
                }
                return 0;
            }
            virtual size_t write_body_cb(const char* buff, size_t buff_size, std::uint64_t start_pos)override
            {
                //��������
                auto end_pos = start_pos + buff_size;
                if ((!increment_) &&(end_pos >= maximum_capacity_))
                {
                    buff_size = maximum_capacity_ - start_pos;
                }

                if (start_pos >= body_.size())
                {
                    body_ += std::string(buff, buff_size);
                    
                }
                else
                {
                    //ֱ�Ӹ���
                    body_ = body_.substr(0,(size_t)start_pos)+ std::string(buff, buff_size);
                }
                return buff_size;
            }

        public:
            static SPHnwHttpBody generate(std::uint64_t maximum_capacity)
            {
                return make_shared_safe<RawBodyImpl>(maximum_capacity,false);
            }
            static SPHnwHttpBody generate()
            {
                return make_shared_safe<RawBodyImpl>((std::uint64_t)0,true);
            }
		private:
            std::string body_;
            //�������
            std::uint64_t maximum_capacity_;
            //�Զ����� ������������
            bool  increment_;
		};
	}
}
#endif