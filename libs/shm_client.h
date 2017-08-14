#pragma once

#include "shm_types.h"

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
	}

	template <typename Object>
	data<Object>& find(const std::string& name)
	{
		std::pair<data<Object>*,std::size_t> p = _segment->find<data<Object>>(name.c_str());
		assert(p.second == 1);

		return *p.first;
	}

	template <typename Object>
	std::vector<data<Object>*> find_array(const std::string& name)
	{
		std::vector<data<Object>*> v;

		std::pair<data<Object>*,std::size_t> p = _segment->find<data<Object>>(name.c_str());
		for (std::size_t i = 0; i < p.second; ++i)
			v.push_back(p.first++);

		return v;
	}

private:
	const std::string _name;
	std::unique_ptr<ipc::managed_shared_memory> _segment;
};

}

using client = detail::client;

}


