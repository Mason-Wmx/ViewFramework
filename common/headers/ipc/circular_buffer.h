
#pragma once

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>


namespace ipc {
namespace imp {

using namespace ::boost::interprocess;

template<std::streamsize SIZE>
class fixed_size_circbuffer
{
public:
    fixed_size_circbuffer()
    {
        _pos_read = _pos_write = 0;
        _glob_written = _glob_read = 0;
        _available = _size = SIZE;
        _eof = false;
    }

    void set_eof(bool eof = true)
    {
        scoped_lock<interprocess_mutex> lock(_mutex);
        _eof = eof;
        if(eof)
            _empty.notify_one();
    }

    std::streamsize read(char* s, std::streamsize n)
    {
        scoped_lock<interprocess_mutex> lock(_mutex);
        std::streamsize read = 0;
        while(true) 
        {
            auto it_read = read_circ(s + read, n - read);
            read += it_read;
            _glob_read += it_read;

            _full.notify_one();

            if (read == n || (_glob_written == _glob_read && read != 0 && _eof))
                break;

            if (_available == _size)
                _empty.wait(lock);
        };        
        return read;
    }
    std::streamsize write(const char* s, std::streamsize n)
    {
        scoped_lock<interprocess_mutex> lock(_mutex);
        std::streamsize written = 0;
        _glob_written += n;
        while(true) 
        {
            written += write_circ(s + written, n - written);

            _empty.notify_one();

            if (written == n)
                break;            

            if (_available == 0)
                _full.wait(lock);
        };        
        return written;
    }

private:
    std::streamsize read_circ(char* s, std::streamsize n)
    {   
        auto to_read = std::min(_size - _available, n);
        if (to_read > 0)
        {
            if( _pos_write <= _pos_read )
            {
                auto read_a = std::min(_size - _pos_read, to_read);
                auto read_b = to_read - read_a;
                memcpy(s, _buf + _pos_read, read_a);
                memcpy(s + read_a, _buf, read_b);
            }
            else
            {
                memcpy(s, _buf + _pos_read, to_read);
            }
            _pos_read = (_pos_read + to_read) % _size;
        }
        _available += to_read;
        return to_read;
    }

    std::streamsize write_circ(const char* s, std::streamsize n)
    {   
        auto to_write = std::min(_available, n);
        if( _pos_write < _pos_read )
        {
            memcpy(_buf + _pos_write, s, to_write);
        }
        else
        {
            auto write_a = std::min(_size - _pos_write, to_write);
            auto write_b = to_write - write_a;
            memcpy(_buf + _pos_write, s, write_a);
            memcpy(_buf, s + write_a, write_b);
        }
        _pos_write = (_pos_write + to_write) % _size;
        _available -= to_write;
        return to_write;
    }

private:
    std::streamsize _pos_read;
    std::streamsize _pos_write;
    std::streamsize _available;
    std::streamsize _size;  // do not remove, this member is here to enable communicating between different size buffer classes in shared memory

    std::streamsize _glob_written;
    std::streamsize _glob_read;

    bool _eof;

    interprocess_mutex _mutex;
    interprocess_condition  _empty;
    interprocess_condition  _full;

    // _buf member has to be declared last to enable communicating between different size buffer classes in shared memory
    // the size of buffer matters on shared memory creation, process that opens it uses buffer with already allocated size (it can be different template instantiation with different SIZE value)
    char _buf[SIZE];
};

} // namespace ipc {
} // namespace imp {
    