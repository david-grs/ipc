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
        return *_segment->find<data<Object>>(name.c_str()).first;
    }

private:
    const std::string _name;
    std::unique_ptr<ipc::managed_shared_memory> _segment;
};

}

using client = detail::client;

}


