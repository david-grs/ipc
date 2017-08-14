#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/containers/stable_vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>

namespace shm {

template <typename T>
using alloc = boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;

using void_allocator = alloc<void>;

using string = boost::interprocess::basic_string<char, std::char_traits<char>, alloc<char>>;

template <typename K, typename V> using map_alloc = alloc<std::pair<const K, V>>;
template <typename K, typename V> using map = boost::interprocess::map<K, V, std::less<K>, map_alloc<K, V>>;

template <typename K> using set = boost::interprocess::set<K, std::less<K>, alloc<K>>;
template <typename V> using list = boost::interprocess::list<V, alloc<V>>;
template <typename V> using deque = boost::interprocess::deque<V, alloc<V>>;
template <typename K> using vector = boost::interprocess::vector<K, alloc<K>>;
template <typename K> using stable_vector = boost::interprocess::stable_vector<K, alloc<K>>;

namespace detail {

namespace ipc = boost::interprocess;

template <typename Object>
struct data
{
    explicit data() :
      _obj{}
    {}

    explicit data(const void_allocator& sm) :
      _obj{sm}
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
    bool try_read(Callable f) const
    {
        ipc::sharable_lock<ipc::interprocess_upgradable_mutex> lock{_mutex, ipc::try_to_lock};
        if (lock.owns())
        {
            f(_obj);
            return true;
        }
        return false;
    }

    template <typename Callable>
    void modify(Callable f)
    {
        ipc::scoped_lock<ipc::interprocess_upgradable_mutex> lock{_mutex};
        f(_obj);
    }

    template <typename Callable>
    bool try_modify(Callable f)
    {
        ipc::scoped_lock<ipc::interprocess_upgradable_mutex> lock{_mutex, ipc::try_to_lock};
        if (lock.owns())
        {
            f(_obj);
            return true;
        }
        return false;
    }

private:
    mutable ipc::interprocess_upgradable_mutex _mutex;
    Object _obj;
};

template <typename Object>
struct data_notifier
{
    explicit data_notifier() :
      _obj{}
    {}

    explicit data_notifier(const void_allocator& sm) :
      _obj{sm}
    {}

    data_notifier(const data_notifier&) =delete;
    data_notifier& operator=(const data_notifier&) =delete;

    data_notifier(data_notifier&& d) :
      _mutex(std::move(d._mutex)),
      _obj(std::move(d._obj))
    {}

    data_notifier& operator=(data_notifier&& d)
    {
        _mutex = std::move(d._mutex);
        _obj = std::move(d._obj);
        return *this;
    }

    template <typename Callable>
    void read(Callable f) const
    {
        ipc::scoped_lock<ipc::interprocess_mutex> lock{_mutex};
        f(_obj);
    }

    template <typename Callable>
    void read_lock(Callable f) const
    {
        ipc::scoped_lock<ipc::interprocess_mutex> lock{_mutex};
        f(_obj, lock);
    }

    template <typename Callable>
    bool try_read(Callable f) const
    {
        ipc::scoped_lock<ipc::interprocess_mutex> lock{_mutex, ipc::try_to_lock};
        if (lock.owns())
        {
            f(_obj);
            return true;
        }
        return false;
    }

    template <typename Callable>
    void modify(Callable f)
    {
        ipc::scoped_lock<ipc::interprocess_mutex> lock{_mutex};
        f(_obj);
    }

    template <typename Callable>
    void modify_lock(Callable f)
    {
        ipc::scoped_lock<ipc::interprocess_mutex> lock{_mutex};
        f(_obj, lock);
    }

    template <typename Callable>
    bool try_modify(Callable f)
    {
        ipc::scoped_lock<ipc::interprocess_mutex> lock{_mutex, ipc::try_to_lock};
        if (lock.owns())
        {
            f(_obj);
            return true;
        }
        return false;
    }

private:
    mutable ipc::interprocess_mutex _mutex;
    Object _obj;
};

}

template <typename Object>
using data = detail::data<Object>;

}
