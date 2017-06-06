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

namespace shm { namespace detail {

namespace ipc = boost::interprocess;

template <typename T>
using shm_alloc = ipc::allocator<T, ipc::managed_shared_memory::segment_manager>;

using void_allocator = shm_alloc<void>;

struct mmdata
{
    double d;
    int i;
};

struct shared_data
{
    using shm_char_alloc = shm_alloc<char>;
    using shm_string = ipc::basic_string<char, std::char_traits<char>, shm_char_alloc>;

    using shm_mapv_alloc = shm_alloc<std::pair<const shm_string, mmdata>>;
    using shm_map = ipc::map<shm_string, mmdata, std::less<shm_string>, shm_mapv_alloc>;

    using shm_int_alloc = shm_alloc<int>;
    using shm_vector = ipc::vector<int, shm_int_alloc>;

    explicit shared_data(const void_allocator& sm) :
      _shm_map(std::less<shm_string>(), sm),
      _shm_vector(sm)
    {}

    shm_map _shm_map;
    shm_vector _shm_vector;
    ipc::interprocess_upgradable_mutex _mutex;
};


struct base_data
{
};

template <typename Object>
struct data : public base_data
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

using shared_data = detail::shared_data;

template <typename Object>
using data = detail::data<Object>;

}
