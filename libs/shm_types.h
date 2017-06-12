#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>

namespace shm {

template <typename T>
using alloc = boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;

using void_allocator = alloc<void>;

using char_alloc = alloc<char>;
using string = boost::interprocess::basic_string<char, std::char_traits<char>, char_alloc>;

template <typename K, typename V>
using mapv_alloc = alloc<std::pair<const K, V>>;

template <typename K, typename V>
using map = boost::interprocess::map<K, V, std::less<K>, mapv_alloc<K, V>>;

template <typename K>
using vector = boost::interprocess::vector<K, alloc<K>>;

namespace detail {

namespace ipc = boost::interprocess;

template <typename Object>
struct data
{
    explicit data(const void_allocator& sm) :
      _obj(sm)
    {}

    data(const data&) =delete;
    data& operator=(const data&) =delete;

    data(data&& d) :
      _mutex(std::move(d._mutex)),
      _obj(std::move(d._obj))
    {}

    data& operator=(data&& d)
    {
        _mutex = std::move(d._mutex);
        _obj = std::move(d._obj);
        return *this;
    }

    template <typename Callable>
    void read(Callable f) const
    {
        ipc::sharable_lock<ipc::interprocess_upgradable_mutex> lock{_mutex};
        f(_obj);
    }

    template <typename Callable>
    void modify(Callable f)
    {
        ipc::scoped_lock<ipc::interprocess_upgradable_mutex> lock{_mutex};
        f(_obj);
    }

private:
    mutable ipc::interprocess_upgradable_mutex _mutex;
    Object _obj;
};

}

template <typename Object>
using data = detail::data<Object>;

}
