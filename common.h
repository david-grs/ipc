#pragma once

#include "libs/shm_types.h"

#include <boost/interprocess/sync/interprocess_condition.hpp>

namespace ipc = boost::interprocess;

struct set
{
	std::array<double, 256> points;
};

struct shared_sets
{
	explicit shared_sets(const shm::void_allocator& sm) :
		_sets(sm)
	{}

	shm::vector<set> _sets;
};


struct current_set
{
    std::size_t set_index;
    ipc::interprocess_condition cond_new_set;
    bool new_set = false;
};
