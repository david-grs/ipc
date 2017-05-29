#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>

namespace ipc = boost::interprocess;

template <typename T>
using shm_alloc = ipc::allocator<T, ipc::managed_shared_memory::segment_manager>;

using void_allocator = shm_alloc<void>;

struct data
{
    double d;
    int i;
};

struct SharedData
{
    using shm_char_alloc = shm_alloc<char>;
    using shm_string = ipc::basic_string<char, std::char_traits<char>, shm_char_alloc>;

    using shm_mapv_alloc = shm_alloc<std::pair<const shm_string, data>>;
    using shm_map = ipc::map<shm_string, data, std::less<shm_string>, shm_mapv_alloc>;

    using shm_int_alloc = shm_alloc<int>;
    using shm_vector = ipc::vector<int, shm_int_alloc>;

    explicit SharedData(const void_allocator& sm) :
      _shm_map(std::less<shm_string>(), sm),
      _shm_vector(sm)
    {}

    shm_map _shm_map;
    shm_vector _shm_vector;
    ipc::interprocess_upgradable_mutex _mutex;
};

