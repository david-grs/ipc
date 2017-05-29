#pragma once

#include "shared.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

#include <iostream>

namespace shm { namespace detail {

namespace ipc = boost::interprocess;

struct client
{

    explicit client(const std::string& name) :
      _name(name)
    {}

    void start()
    {
        _segment = std::make_unique<ipc::managed_shared_memory>(ipc::open_only, _name.c_str());
        _data = _segment->find<shared_data>("blarp").first;
    }

    int count() const { return _reads; }
    void reset() { _reads = 0; }

    void read()
    {
        ipc::sharable_lock<ipc::interprocess_upgradable_mutex> lock{_data->_mutex};

        int last = _data->_shm_vector[0];
        for (int i = 1; i < 10; ++i)
        {
            const int elem = _data->_shm_vector[i];

            if (elem != last + 1)
                throw 42;

            last = elem;
        }

        if (_data->_shm_map.size() != 2)
            throw 42;

        ++_reads;
        //segment.destroy<shm_vector>("shm_vector");
    }


private:
    const std::string _name;
    std::unique_ptr<ipc::managed_shared_memory> _segment;
    shared_data* _data;
    int _reads = {};
};

}

using client = detail::client;

}


