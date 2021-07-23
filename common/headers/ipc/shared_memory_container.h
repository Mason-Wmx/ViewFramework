
#pragma once

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/interprocess/mapped_region.hpp>


namespace ipc {
namespace imp {

using namespace ::boost::interprocess;

template< class SHMEM >
struct shm_creator
{
    static SHMEM * create(const std::string & name, std::size_t size)
    {
        auto shm = new SHMEM(create_only, name.c_str(), read_write);
        shm->truncate(size);

        return shm;
    }

    static void remove(const std::string & name)
    {
        SHMEM::Remove(name.c_str());
    }
};

} //namespace ipc {
} //namespace imp {


#if defined(_WIN32) || defined(_WIN64)

#include <boost/interprocess/windows_shared_memory.hpp>

namespace ipc {
namespace imp {

template<>
struct shm_creator<windows_shared_memory>
{
    static windows_shared_memory * create(const std::string & name, std::size_t size)
    {
        return new windows_shared_memory(create_only, name.c_str(), read_write, size);
    }

    static void remove(const std::string & name)
    {
    }
};

} //namespace ipc {
} //namespace imp {

#endif


namespace ipc {
namespace imp {

template<class T, class SHMEM>
class shm_pod_container
{
public:
    shm_pod_container(const std::string & id)
    {
        try
        {
            _shm.reset(shm_creator<SHMEM>::create(id, sizeof(T)));
            _region.reset(new mapped_region(*_shm, read_write));
            new (_region->get_address()) T;
        }
        catch (const interprocess_exception &)
        {
            _shm.reset(new SHMEM(open_only, id.c_str(), read_write));
            _region.reset(new mapped_region(*_shm, read_write));
        }
        _data = static_cast<T*>(_region->get_address());
    }

    ~shm_pod_container()
    {
        shm_creator<SHMEM>::remove(_shm->get_name());
    }

    T & get_data()
    {
        return *_data;
    }

private:
    std::shared_ptr<SHMEM> _shm;
    std::shared_ptr<mapped_region> _region;
    T * _data;
};


template<class T, class SHMEM>
class shm_sync_pod_container
{
private:
    struct shm_content
    {
        T data;
        interprocess_mutex mutex;
    };
    typedef shm_pod_container<shm_content, SHMEM> shm_content_type;
public:
    shm_sync_pod_container(const std::string & id)
        : _data(id)
    {
    }

    T & get_data()
    {
        return _data.get_data().data;
    }

    interprocess_mutex & get_mutex()
    {
        return _data.get_data().mutex;
    }

    void interlocked(std::function<void(T &)> fun)
    {
         scoped_lock<interprocess_mutex> lock(get_mutex());
         fun(get_data());
    } 
private:
    shm_content_type _data;
};

} // namespace ipc {
} // namespace imp {
