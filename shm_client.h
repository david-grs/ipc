#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

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
        managed_shared_memory segment(open_only, _name.c_str());

        shm_vector* v = segment.find<shm_vector>("shm_vector").first;
        _mutex = segment.find_or_construct<interprocess_mutex>("mtx")();

        scoped_lock<interprocess_mutex> lock{*_mutex};
        int last = (*v)[0];
        for (int i = 1; i < 10; ++i)
        {
            const int elem = (*v)[i];

            if (elem != last + 1)
                throw 42;

            std::cout << "elem = " << elem << std::endl;
            last = elem;
        }

        //segment.destroy<shm_vector>("shm_vector");
    }


private:
    const std::string _name;
    interprocess_mutex* _mutex;
};
