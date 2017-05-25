#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <vector>
#include <iostream>
#include <string>

using namespace boost::interprocess;

using shm_allocator = allocator<int, managed_shared_memory::segment_manager>;
using shm_vector = std::vector<int, shm_allocator>;

struct shm_server
{
    shm_server(const std::string& name) :
      _name(name)
     {}

    ~shm_server()
    {
        shared_memory_object::remove(_name.c_str());
    }

    void start()
    {
        std::cout << "starting..." << std::endl;

        //Create a new segment with given name and size
        managed_shared_memory segment(create_only, _name.c_str(), 65536);

        //Initialize shared memory STL-compatible allocator
        const shm_allocator alloc_inst (segment.get_segment_manager());

        //Construct a vector named "shm_vector" in shared memory with argument alloc_inst
        shm_vector *myvector = segment.construct<shm_vector>("shm_vector")(alloc_inst);

        for(int i = 0; i < 100; ++i)
            myvector->push_back(i);

        //Check child has destroyed the vector
        //if(segment.find<MyVector>("MyVector").first)
        //    return 1;
    }


private:
    const std::string _name;
};
