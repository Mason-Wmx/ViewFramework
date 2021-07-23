
#pragma once

#include "shared_memory_container.h"

namespace ipc {
namespace imp {

using namespace ::boost::interprocess;

template<class INT_TYPE, class SHMEM>
class shm_counter : public shm_sync_pod_container<INT_TYPE, SHMEM>
{
public:
    shm_counter(const std::string & id)
        : shm_sync_pod_container<INT_TYPE, SHMEM>(id)
    {
        scoped_lock<interprocess_mutex> lock(get_mutex());
        _ordinal = get_data()++;
    }
    
    ~shm_counter()
    {
        scoped_lock<interprocess_mutex> lock(get_mutex());
        get_data()--;
    }

    INT_TYPE get_ordinal()
    {
        return _ordinal;
    }
private:
    shm_counter(const shm_counter & other) {}
private:
    INT_TYPE _ordinal;
};

} // namespace ipc {
} // namespace imp {
