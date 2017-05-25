#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <vector>
#include <iostream>

using namespace boost::interprocess;

using shm_allocator = allocator<int, managed_shared_memory::segment_manager>;
using shm_vector = std::vector<int, shm_allocator>;

struct shm_client
{
    explicit shm_client(const std::string& name) :
      _name(name)
    {}

    void start()
    {
        managed_shared_memory segment(open_only, _name);

        shm_vector* v = segment.find<shm_vector>("shm_vector").first;

        std::sort(v->rbegin(), v->rend());
        for (const auto& x : *v)
            std::cout << "child = " << x << std::endl;

        segment.destroy<shm_vector>("shm_vector");
    }


private:
    const std::string _name;
};
