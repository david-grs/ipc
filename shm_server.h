#pragma once

#include "shared.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <memory>
#include <iostream>
#include <string>

namespace shm { namespace detail {

namespace ipc = boost::interprocess;

struct server
{
    server(const std::string& name) :
      _name(name)
    {
         ipc::shared_memory_object::remove(_name.c_str());
    }

    ~server()
    {
        ipc::shared_memory_object::remove(_name.c_str());
    }

    void start()
    {
        std::cout << "starting..." << std::endl;

        _segment = std::make_unique<ipc::managed_shared_memory>(ipc::create_only, _name.c_str(), 65536);
        _alloc = std::make_unique<const void_allocator>(_segment->get_segment_manager());

        _data = _segment->construct<shared_data>("blarp")(*_alloc);
        for (int i = 0; i < 10; ++i)
            _data->_shm_vector.push_back(i);

        _data->_shm_map.emplace(shared_data::shm_string("foo", *_alloc), data{10.0, 3});
        _data->_shm_map.emplace(shared_data::shm_string("bar", *_alloc), data{4.0, 11});
    }

    int count() const { return _updates; }
    void reset() { _updates = 0; }

    void update()
    {
        ipc::scoped_lock<ipc::interprocess_upgradable_mutex> lock{_data->_mutex};

        for (int i = 0; i < 10; ++i)
            _data->_shm_vector[i] = _data->_shm_vector[i] + 1;

        ++_updates;
    }


private:
    const std::string _name;
    std::unique_ptr<ipc::managed_shared_memory> _segment;
    std::unique_ptr<const void_allocator> _alloc;
    shared_data* _data;
    int _updates = {};
};

}

using server = detail::server;

}

