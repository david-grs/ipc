#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include <vector>
#include <memory>
#include <iostream>
#include <string>

using namespace boost::interprocess;

using shm_allocator = allocator<int, managed_shared_memory::segment_manager>;
using shm_vector = std::vector<int, shm_allocator>;

struct shm_server
{
    shm_server(const std::string& name) :
      _name(name)
    {
         shared_memory_object::remove(_name.c_str());
    }

    ~shm_server()
    {
        shared_memory_object::remove(_name.c_str());
    }

    void start()
    {
        std::cout << "starting..." << std::endl;

        _segment = std::make_unique<managed_shared_memory>(create_only, _name.c_str(), 65536);
        _alloc = std::make_unique<const shm_allocator>(_segment->get_segment_manager());

        _data = _segment->construct<shm_vector>("shm_vector")(*_alloc);
        _mutex = _segment->find_or_construct<interprocess_mutex>("mtx")();

        for (int i = 0; i < 10; ++i)
            _data->push_back(i);
    }

    void update()
    {
        scoped_lock<interprocess_mutex> lock{*_mutex};
        
        for (int i = 0; i < 10; ++i)
            (*_data)[i] = (*_data)[i] + 1;
    }


private:
    const std::string _name;
    std::unique_ptr<managed_shared_memory> _segment;
    std::unique_ptr<const shm_allocator> _alloc;
    interprocess_mutex* _mutex;
    shm_vector* _data;
};
