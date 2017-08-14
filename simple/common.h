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
      _shm_set(std::less<shm::string>(), sm),
      _shm_list(sm),
      _shm_vector(sm),
      _shm_svector(sm)
    {}

    shm::map<shm::string, mmdata> _shm_map;
    shm::set<shm::string> _shm_set;
    shm::list<shm::string> _shm_list;
    shm::vector<int> _shm_vector;
    shm::stable_vector<int> _shm_svector;
};

