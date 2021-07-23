#pragma once

#include <mutex>
#include <condition_variable>


namespace SIM {
namespace ObjectStore {

typedef std::mutex Mutex;
typedef std::unique_lock<Mutex> LockGuard;

}
}