#pragma once

#include "libs/shm_types.h"

struct set
{
	std::array<double, 256> points;
};

struct shared_data
{
	explicit shared_data(const shm::void_allocator& sm) :
		_sets(sm)
	{}

	shm::vector<set> _sets;
};

