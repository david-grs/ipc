#pragma once

#include "shared.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

#include <iostream>

using namespace boost::interprocess;

struct shm_client
{
    explicit shm_client(const std::string& name) :
      _name(name)
    {}

    void start()
    {
        _segment = std::make_unique<managed_shared_memory>(open_only, _name.c_str());
        _data = _segment->find<SharedData>("blarp").first;
    }

    int count() const { return _reads; }
    void reset() { _reads = 0; }

    void read()
    {
        sharable_lock<interprocess_upgradable_mutex> lock{_data->_mutex};

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
    std::unique_ptr<managed_shared_memory> _segment;
    SharedData* _data;
    int _reads = {};
};
