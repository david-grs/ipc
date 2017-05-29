#pragma once

#include "shared.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <boost/interprocess/sync/scoped_lock.hpp>

#include <memory>
#include <iostream>
#include <string>

using namespace boost::interprocess;

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
        _alloc = std::make_unique<const void_allocator>(_segment->get_segment_manager());

        _data = _segment->construct<SharedData>("blarp")(*_alloc);
        for (int i = 0; i < 10; ++i)
            _data->_shm_vector.push_back(i);

        _data->_shm_map.emplace(SharedData::shm_string("foo", *_alloc), data{10.0, 3});
        _data->_shm_map.emplace(SharedData::shm_string("bar", *_alloc), data{4.0, 11});
    }

    int count() const { return _updates; }
    void reset() { _updates = 0; }

    void update()
    {
        scoped_lock<interprocess_upgradable_mutex> lock{_data->_mutex};

        for (int i = 0; i < 10; ++i)
            _data->_shm_vector[i] = _data->_shm_vector[i] + 1;

        ++_updates;
    }


private:
    const std::string _name;
    std::unique_ptr<managed_shared_memory> _segment;
    std::unique_ptr<const void_allocator> _alloc;
    SharedData* _data;
    int _updates = {};
};
