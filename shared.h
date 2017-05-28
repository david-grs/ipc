#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <string>
#include <vector>
#include <map>

using namespace boost::interprocess;

struct data
{
    double d;
    int i;
};

struct SharedData
{
    explicit SharedData(managed_shared_memory::segment_manager* sm) :
      _shm_map(std::less<std::string>(), sm),
      _shm_vector(sm)
    {}

    using shm_map_alloc = allocator<std::pair<std::string, data>, managed_shared_memory::segment_manager>;
    using shm_map = std::map<std::string, data, std::less<std::string>, shm_map_alloc>;

    using shm_vector_alloc = allocator<int, managed_shared_memory::segment_manager>;
    using shm_vector = std::vector<int, shm_vector_alloc>;

    shm_map _shm_map;
    shm_vector _shm_vector;
};

