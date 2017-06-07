#pragma once

#include "shared.h"

#include <boost/interprocess/managed_shared_memory.hpp>

#include <memory>
#include <iostream>
#include <string>
#include <unordered_map>

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
        for (const auto& p : _deleters)
            p.second();

        ipc::shared_memory_object::remove(_name.c_str());
    }

    void start()
    {
        std::cout << "starting..." << std::endl;

        _segment = std::make_unique<ipc::managed_shared_memory>(ipc::create_only, _name.c_str(), 65536);
        _alloc = std::make_unique<const void_allocator>(_segment->get_segment_manager());
    }

    const void_allocator& allocator() const { return *_alloc; }

    template <typename Object>
    data<Object>* construct(const std::string& name)
    {
        data<Object>* obj = _segment->construct<data<Object>>(name.c_str())(*_alloc);
        const bool inserted = _deleters.emplace(name, [=]() { _segment->destroy<data<Object>>(name.c_str()); }).second;

        if (!inserted)
            throw std::runtime_error("shm: object " + name + " already inserted");

        return obj;
    }

    template <typename Object>
    using ShmDeleter = std::function<void(data<Object>*)>;

    template <typename Object>
    std::unique_ptr<data<Object>, ShmDeleter<Object>> construct2(const std::string& name)
    {
        data<Object>* obj = _segment->construct<data<Object>>(name.c_str())(*_alloc);
        std::unique_ptr<data<Object>, ShmDeleter<Object>> ptr{obj, [&](data<Object>*) { _segment->destroy<data<Object>>(name.c_str()); }};
        return std::move(ptr);
    }

    template <typename Object>
    void destroy(const std::string& name)
    {
        auto it = _deleters.find(name);
        if (it == _deleters.cend())
        {
            assert(false);
            return;
        }

        it->second();
        _deleters.erase(it);
    }

private:
    const std::string _name;
    std::unique_ptr<ipc::managed_shared_memory> _segment;
    std::unique_ptr<const void_allocator> _alloc;
    std::unordered_map<std::string, std::function<void()>> _deleters;
};

}

using server = detail::server;

}

