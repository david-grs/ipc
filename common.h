#pragma once

#include "libs/shm_types.h"

struct mmdata
{
    double d;
    int i;
};

struct shared_data
{
    explicit shared_data(const void_allocator& sm) :
      _shm_map(std::less<string>(), sm),
      _shm_vector(sm)
    {}

    map<string, mmdata> _shm_map;
    vector<int> _shm_vector;
    ipc::interprocess_upgradable_mutex _mutex;
};

