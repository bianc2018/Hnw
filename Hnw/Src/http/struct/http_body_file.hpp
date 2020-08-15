
/*
    文件为基础的body
*/
#ifndef HNW_HTTP_BODY_FILE_HPP_
#define HNW_HTTP_BODY_FILE_HPP_

#include  <boost/interprocess/file_mapping.hpp>
#include  <boost/interprocess/mapped_region.hpp>
#include  <boost/filesystem.hpp>
#include "http_body_impl.hpp"

namespace hnw
{
    namespace http
    {
        class FileBodyImpl :public BaseBodyImpl
        {

        public:
            FileBodyImpl() :BaseBodyImpl(BODY_FILE), flag_(0)
            {
                
            }
            ~FileBodyImpl()
            {
                //删除掉
                if (flag_ & HTTP_FILE_FLAG_DELETE)
                {
                    PRINTFLOG(BL_DEBUG, "file_mapping %s is delete", file_name_.c_str());
                    boost::interprocess::file_mapping::remove(file_name_.c_str());
                }
            }

            //初始化
            bool init(const std::string& name,
                const HttpRange& range,int flag)
            {
                file_name_ = name;
                range_ = range;
                flag_ = flag;

                //是否存在？
                if (!util::file_is_exist(file_name_))
                {
                    //不存在，是否新建
                    if (flag_ & HTTP_FILE_FLAG_CREATE|| flag_ & HTTP_FILE_FLAG_CREATE_NEW)
                    {
                        
                        if (!util::create_null_file(file_name_, range_.total))
                        {
                            PRINTFLOG(BL_ERROR, "create_null_file fail name=%s",
                                file_name_.c_str());
                            return false;
                        }
                    }
                    else
                    {
                        PRINTFLOG(BL_ERROR, "file %s is not found,set flag HTTP_FILE_FLAG_CREATE create file",
                            file_name_.c_str());
                        return false;
                    }
                    size_ = range_.total;
                }
                else
                {
                    //存在，是否新建
                    if (flag_ & HTTP_FILE_FLAG_CREATE_NEW)
                    {
                        util::remove_file(file_name_);

                        if (!util::create_null_file(file_name_, range_.total))
                        {
                            PRINTFLOG(BL_ERROR, "create_null_file fail name=%s",
                                file_name_.c_str());
                            return false;
                        }
                        size_ = range_.total;
                    }
                    else
                    {
                        size_ = util::file_size(file_name_);
                        range_.total = size_;
                    }
                }

                if (flag_ & HTTP_FILE_FLAG_CREATE || flag_ & HTTP_FILE_FLAG_CREATE_NEW)
                {//新建默认
                    flag_ |= HTTP_FILE_FLAG_WRITE;
                }

                try
                {
                    //数据范围
                    if (range_.use_start_endpoint == true)
                        range_.start = 0;
                    if (range_.use_end_endpoint == true)
                        range_.end = size_-1;
                    if (range_.start >= size_)
                    {
                        PRINTFLOG(BL_ERROR, "range_.start[%llu] >= size_[%llu]", range_.start, size_);
                        return false;
                   
                    }
                    if (range_.end >= size_)
                    {
                        PRINTFLOG(BL_ERROR, "range_.end[%llu] >= size_[%llu]", range_.end, size_);
                        return false;
                    }
                    if (range_.start > range_.end)
                    {
                        PRINTFLOG(BL_ERROR,
                            "range_.start[%llu] >= range_.end_[%llu]", range_.start, range_.end);
                        return false;
                    }

                    if (flag_ & HTTP_FILE_FLAG_WRITE)
                    {
                        //文件映射
                        boost::interprocess::file_mapping fmap(file_name_.c_str(),
                            boost::interprocess::mode_t::read_write);
                        fmap_.swap(fmap);
                        return true;
                    }
                    else
                    {
                        //文件映射
                        boost::interprocess::file_mapping fmap(file_name_.c_str(),
                            boost::interprocess::mode_t::read_only);
                        fmap_.swap(fmap);
                        return true;
                    }
                   
                }
                catch (const std::exception&e)
                {
                    PRINTFLOG(BL_ERROR, "FileBodyImpl::init error,what=%s", e.what());
                    return false;
                }
            }

            virtual std::uint64_t body_size()
            {
                return range_.end - range_.start + 1;
            }
            //是否是完整的报文
            virtual bool is_complete()
            {
                return (write_pos() == body_size());
            }
            virtual size_t read_body_cb(char* buff, size_t buff_size, std::uint64_t start_pos)override
            {
                size_t offset = 0;
                
                start_pos += range_.start;

                if (start_pos >= range_.end+1)
                    return offset;

                auto end = start_pos + buff_size;
                if (end >= range_.end + 1)
                {
                    offset = buff_size - (end - range_.end - 1);
                }
                else
                {
                    offset = buff_size;
                }

                //建立映射
                boost::interprocess::mapped_region map_region(fmap_,
                    boost::interprocess::mode_t::read_only, start_pos, offset);

                memcpy(buff,map_region.get_address(), offset);

                
                return offset;
            }
            
            virtual size_t write_body_cb(const char* buff, size_t buff_size, std::uint64_t start_pos)override
            {
                //不允许写
                if (!(flag_ & HTTP_FILE_FLAG_WRITE))
                {
                    PRINTFLOG(BL_ERROR, "write error ,HTTP_FILE_FLAG_WRITE not set");
                    return 0;
                }
                size_t offset = 0;
                start_pos += range_.start;

                if (start_pos >= range_.end + 1)
                    return offset;

                auto end = start_pos + buff_size;
                if (end >= size_)
                {
                    offset = buff_size - (end - range_.end - 1);
                }
                else
                {
                    offset = buff_size;
                }

                //建立映射
                boost::interprocess::mapped_region map_region(fmap_,
                    boost::interprocess::mode_t::read_write, start_pos, offset);

                memcpy(map_region.get_address(), buff, offset);

                return offset;
            }

            virtual std::uint64_t file_size()
            {
                return size_;
            }
        public:
            //已有的文件
            static SPHnwHttpBody generate(const std::string& name,
                const HttpRange& range= HttpRange(),int flag= HTTP_FILE_FLAG_OPEN)
            {
                auto p = make_shared_safe<FileBodyImpl>();
                if (true == p->init(name, range,flag))
                    return p;
                return nullptr;
            }
            

        private:
            std::string file_name_;
            std::uint64_t size_;
            int flag_;

            boost::interprocess::file_mapping fmap_;
            HttpRange range_;
        };
    }
}
#endif