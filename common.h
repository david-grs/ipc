#pragma once

#include "libs/shm_types.h"

struct mmdata
{
    double d;
    int i;
};

struct shared_data
{
    explicit shared_data(const shm::void_allocator& sm) :
      _shm_map(std::less<shm::string>(), sm),
      _shm_vector(sm)
    {}

    shm::map<shm::string, mmdata> _shm_map;
    shm::vector<int> _shm_vector;
};

