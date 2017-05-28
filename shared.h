#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

using namespace boost::interprocess;

using void_allocator = allocator<void, managed_shared_memory::segment_manager>;

struct data
{
    double d;
    int i;
};

struct SharedData
{
    using shm_char_alloc = allocator<char, managed_shared_memory::segment_manager>;
    using shm_string = basic_string<char, std::char_traits<char>, shm_char_alloc>;

    using shm_map_alloc = allocator<std::pair<const shm_string, data>, managed_shared_memory::segment_manager>;
    using shm_map = map<shm_string, data, std::less<shm_string>, shm_map_alloc>;

    using shm_vector_alloc = allocator<int, managed_shared_memory::segment_manager>;
    using shm_vector = vector<int, shm_vector_alloc>;

    explicit SharedData(const void_allocator& sm) :
      _shm_map(std::less<shm_string>(), sm),
      _shm_vector(sm)
    {}

    shm_map _shm_map;
    shm_vector _shm_vector;
};

