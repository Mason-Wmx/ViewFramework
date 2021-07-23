
#pragma once

#pragma warning(push)
#pragma warning(disable: 4250) // disable warnings
#pragma warning(disable: 4018) // caused by boost library

#include "shared_memory_counter.h"
#include "shared_memory_state_machine.h"
#include "shared_memory_buffer_devices.h"
#include "circular_buffer.h"
#include "pipeline_link.h"
#include "pipeline.h"


#if defined(_WIN32) || defined(_WIN64)
#include <boost/interprocess/windows_shared_memory.hpp>
namespace ipc {
    typedef ::boost::interprocess::windows_shared_memory shared_memory_imp;
}

#elif
#include <boost/interprocess/shared_memory_object.hpp>
namespace ipc {
typedef ::boost::interprocess::shared_memory_object shared_memory_imp;
}
#endif

#pragma warning(pop)

namespace ipc {

template<class T>
struct shm_pod_container {
    typedef imp::shm_pod_container<T, shared_memory_imp> type;
};

template<class T>
struct shm_sync_pod_container {
  typedef imp::shm_sync_pod_container<T, shared_memory_imp> type;
};

template<class INT_TYPE>
struct shm_counter {
  typedef imp::shm_counter<INT_TYPE, shared_memory_imp> type;
};

template<class INT_TYPE>
struct shm_duplex_state_machine {
    typedef imp::shm_duplex_state_machine<INT_TYPE, shared_memory_imp> type;
};

template<size_t SIZE>
struct shm_buffer_sink {
  typedef imp::shm_buffer_sink<imp::fixed_size_circbuffer<SIZE>, shared_memory_imp> type;
};

template<size_t SIZE>
struct shm_buffer_source {
  typedef imp::shm_buffer_source<imp::fixed_size_circbuffer<SIZE>, shared_memory_imp> type;
};

template<size_t SIZE, class ARCH_SETUP = imp::arch_setup_default>
struct pipeline_uplink {
  typedef imp::pipeline_uplink<typename shm_buffer_sink<SIZE>::type, ARCH_SETUP> type;
};

template<size_t SIZE, class ARCH_SETUP = imp::arch_setup_default>
struct pipeline_downlink {
  typedef imp::pipeline_downlink<typename shm_buffer_source<SIZE>::type, ARCH_SETUP> type;
};

template<size_t SIZE, class ARCH_SETUP = imp::arch_setup_default>
struct pipeline_duplex {
  typedef imp::pipeline_duplex<typename shm_buffer_sink<SIZE>::type, typename shm_buffer_source<SIZE>::type, ARCH_SETUP> type;
};

template<size_t SIZE, class ARCH_SETUP = imp::arch_setup_default>
struct pipeline_2_spec_size {
  typedef imp::pipeline_2<typename pipeline_duplex<SIZE, ARCH_SETUP>::type, typename shm_duplex_state_machine<int>::type> type;
};

template<class ARCH_SETUP>
struct pipeline_2_with_setup {
  typedef typename pipeline_2_spec_size<1024*1024, ARCH_SETUP>::type type;
};

typedef pipeline_2_spec_size<1024*1024>::type pipeline_2;

} // namespace ipc {

