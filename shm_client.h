#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/upgradable_lock.hpp>
#include <boost/interprocess/sync/named_upgradable_mutex.hpp>

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
        _segment = std::make_unique<managed_shared_memory>(open_only, _name.c_str());
        _mutex = std::make_unique<named_upgradable_mutex>(open_only, "banana");

        _data = _segment->find<shm_vector>("shm_vector").first;
    }

    int count() const { return _reads; }
    void reset() { _reads = 0; }

    void read()
    {
        sharable_lock<named_upgradable_mutex> lock{*_mutex};
        int last = (*_data)[0];
        for (int i = 1; i < 10; ++i)
        {
            const int elem = (*_data)[i];

            if (elem != last + 1)
                throw 42;

            last = elem;
        }

        ++_reads;
        //segment.destroy<shm_vector>("shm_vector");
    }


private:
    const std::string _name;
    std::unique_ptr<managed_shared_memory> _segment;
    std::unique_ptr<named_upgradable_mutex> _mutex;
    shm_vector* _data;
    int _reads = {};
};
