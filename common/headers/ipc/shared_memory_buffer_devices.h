#pragma once

#include <boost/iostreams/concepts.hpp>

#include "shared_memory_container.h"


namespace ipc {
namespace imp {

template<class BUFIMP, class SHMEM>
class shm_buffer_source : public boost::iostreams::source, public shm_pod_container<BUFIMP, SHMEM>
{
public:
    typedef BUFIMP BufferType;

public:
    shm_buffer_source(const std::string & id)
        : shm_pod_container(id)
    {
    }

    void reset()
    {
        get_data().reset();
    }

    std::streamsize read(char* s, std::streamsize n)
    {
        return get_data().read(s, n);
    }
};

template<class BUFIMP, class SHMEM>
class shm_buffer_sink : public boost::iostreams::sink, public shm_pod_container<BUFIMP, SHMEM>
{
public:
    typedef BUFIMP BufferType;

public:
    shm_buffer_sink(const std::string & id)
        : shm_pod_container(id)
    {
    }

    void reset()
    {
        get_data().reset();
    }

    void set_eof()
    {
        get_data().set_eof();
    }

    std::streamsize write(const char* s, std::streamsize n)
    {
        return get_data().write(s, n);
    }
};

} // namespace ipc {
} // namespace imp {
