#pragma once

#include "shm_types.h"

#include <boost/interprocess/managed_shared_memory.hpp>

#include <memory>
#include <iostream>
#include <string>

namespace shm { namespace detail {

namespace ipc = boost::interprocess;

struct server
{
	server(const std::string& name) :
	  _name(name)
	{
		 ipc::shared_memory_object::remove(_name.c_str());
	}

	~server()
	{
		ipc::shared_memory_object::remove(_name.c_str());
	}

	void start()
	{
		std::cout << "starting..." << std::endl;

		_segment = std::make_shared<ipc::managed_shared_memory>(ipc::create_only, _name.c_str(), 65536);
		_alloc = std::make_unique<const void_allocator>(_segment->get_segment_manager());
	}

	const void_allocator& allocator() const { return *_alloc; }

	template<typename T>
	using ShmUniquePtr = std::unique_ptr<T, std::function<void(T*)>>;

private:
	struct shm_allocator_tag {};
	struct no_shm_allocator_tag {};

	template <typename Object>
	ShmUniquePtr<data<Object>>
	construct(const std::string& name, shm_allocator_tag)
	{
		data<Object>* obj = _segment->construct<data<Object>>(name.c_str())(*_alloc);
		return ShmUniquePtr<data<Object>>{obj, [segment = _segment, name](data<Object>*)
		{
			segment->destroy<data<Object>>(name.c_str());
		}};
	}

	template <typename Object>
	ShmUniquePtr<data<Object>>
	construct(const std::string& name, no_shm_allocator_tag)
	{
		data<Object>* obj = _segment->construct<data<Object>>(name.c_str())();
		return ShmUniquePtr<data<Object>>{obj, [segment = _segment, name](data<Object>*)
		{
			segment->destroy<data<Object>>(name.c_str());
		}};
	}

public:
	template <typename Object>
	ShmUniquePtr<data<Object>>
	construct(const std::string& name)
	{
		using tag_t = std::conditional_t<std::is_constructible<Object, const void_allocator&>::value,
											shm_allocator_tag,
											no_shm_allocator_tag>;
		return construct<Object>(name, tag_t{});
	}

	template <typename Object>
	ShmUniquePtr<std::vector<data<Object>*>>
	construct(const std::string& name, std::size_t count)
	{
		data<Object>* obj = _segment->construct<data<Object>>(name.c_str())[count](*_alloc);
		ShmUniquePtr<std::vector<data<Object>*>> v{new std::vector<data<Object>*>{}, [segment = _segment, name](std::vector<data<Object>*>*)
		{
			segment->destroy<data<Object>>(name.c_str());
		}};

		for (std::size_t i = 0; i < count; ++i)
			v->push_back(obj++);
		return v;
	}

private:
	const std::string _name;
	std::shared_ptr<ipc::managed_shared_memory> _segment;
	std::unique_ptr<const void_allocator> _alloc;
};

}

using server = detail::server;

}

