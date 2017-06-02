#pragma once

#include "shared.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/any.hpp>

#include <memory>
#include <iostream>
#include <string>
#include <unordered_map>

namespace shm { namespace detail {

namespace ipc = boost::interprocess;

struct base_data
{
};

template <typename Object>
struct data : public base_data
{
    explicit data(Object* obj) :
      _obj(obj)
    {}

    data(const data&) =delete;
    data& operator=(const data&) =delete;

    template <typename Callable>
    void read(Callable f)
    {
        ipc::sharable_lock<ipc::interprocess_upgradable_mutex> lock{_mutex};
        f(*_obj);
    }

    template <typename Callable>
    void modify(Callable f)
    {
        ipc::scoped_lock<ipc::interprocess_upgradable_mutex> lock{_mutex};
        f(*_obj);
    }

private:
    ipc::interprocess_upgradable_mutex _mutex;
    Object* _obj;
};

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

    template <typename Object>
    data<Object> construct(const std::string& name)
    {
        Object* obj = _segment->construct<Object>(name.c_str())(*_alloc);
        const bool inserted = _deleters.emplace(name, [=]() { _segment->destroy<Object>(name.c_str()); });

        if (!inserted)
            throw std::runtime_error("shm: object " + name + " already inserted");

        return data<Object>(obj);
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

#if 0
        _data = _segment->construct<shared_data>("blarp")(*_alloc);
        for (int i = 0; i < 10; ++i)
            _data->_shm_vector.push_back(i);

        _data->_shm_map.emplace(shared_data::shm_string("foo", *_alloc), data{10.0, 3});
        _data->_shm_map.emplace(shared_data::shm_string("bar", *_alloc), data{4.0, 11});
#endif

    int count() const { return _updates; }
    void reset() { _updates = 0; }

    void update()
    {
#if 0
        ipc::scoped_lock<ipc::interprocess_upgradable_mutex> lock{_data->_mutex};

        for (int i = 0; i < 10; ++i)
            _data->_shm_vector[i] = _data->_shm_vector[i] + 1;

        ++_updates;
#endif
    }


private:
    const std::string _name;
    std::unique_ptr<ipc::managed_shared_memory> _segment;
    std::unique_ptr<const void_allocator> _alloc;
    std::unordered_map<std::string, std::function<void()>> _deleters;
    int _updates = {};
};

}

using server = detail::server;

}

