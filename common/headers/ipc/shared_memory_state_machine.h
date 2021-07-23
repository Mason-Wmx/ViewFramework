
#pragma once

#include "shared_memory_container.h"

namespace ipc {
namespace imp {

using namespace ::boost::interprocess;

template<class INT_TYPE, class SHMEM>
class shm_duplex_state_machine : public shm_sync_pod_container<INT_TYPE, SHMEM>
{
public:
    shm_duplex_state_machine(const std::string & id)
        : shm_sync_pod_container<INT_TYPE, SHMEM>(id)
    {
        scoped_lock<interprocess_mutex> lock(get_mutex());
        auto& cnt = get_data();
        _ordinal = cnt & INT_TYPE(1);
        cnt += (_ordinal + 1);
    }

    ~shm_duplex_state_machine()
    {
        scoped_lock<interprocess_mutex> lock(get_mutex());
        auto& cnt = get_data();
        cnt -= (_ordinal + 1);
    }

    INT_TYPE get_ordinal()
    {
        return _ordinal;
    }
private:
    shm_duplex_state_machine(const shm_duplex_state_machine & other) {}
private:
    INT_TYPE _ordinal;
};

} // namespace ipc {
} // namespace imp {