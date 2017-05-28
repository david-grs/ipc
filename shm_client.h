#pragma once

#include "shared.h"

#include <boost/interprocess/managed_shared_memory.hpp>

#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/named_upgradable_mutex.hpp>

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
        _mutex = std::make_unique<named_upgradable_mutex>(open_only, "banana");

        _data = _segment->find<SharedData>("blarp").first;
    }

    int count() const { return _reads; }
    void reset() { _reads = 0; }

    void read()
    {
        sharable_lock<named_upgradable_mutex> lock{*_mutex};
        int last = _data->_shm_vector[0];
        for (int i = 1; i < 10; ++i)
        {
            const int elem = _data->_shm_vector[i];

            if (elem != last + 1)
                throw 42;

            last = elem;
        }

        if (_data->_shm_map.size() != 1)
            throw 42;

        ++_reads;
        //segment.destroy<shm_vector>("shm_vector");
    }


private:
    const std::string _name;
    std::unique_ptr<managed_shared_memory> _segment;
    std::unique_ptr<named_upgradable_mutex> _mutex;
    SharedData* _data;
    int _reads = {};
};
