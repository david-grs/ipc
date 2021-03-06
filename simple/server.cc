#include "common.h"
#include "libs/shm_server.h"

#include <thread>
#include <chrono>

extern "C"
{
#include <signal.h>
}

// boo
volatile bool run = true;

int main(int argc, char *argv[])
{
    auto server = std::make_unique<shm::server>("foob4r");
    server->start();

    auto data = server->construct<shared_data>("shared_data");

    data->modify([&server](shared_data& data)
    {
        for (int i = 0; i < 10; ++i)
            data._shm_vector.push_back(i);

        data._shm_map.emplace(shm::string("foo", server->allocator()), mmdata{10.0, 3});
        data._shm_map.emplace(shm::string("bar", server->allocator()), mmdata{4.0, 11});
    });

    auto start = std::chrono::steady_clock::now();
    int64_t ops = 0;

    ::signal(15, [](int) { run = false; });
    ::signal(2, [](int)  { run = false; });

    while (run)
    {
        for (int i = 0; i < 100; ++i)
        {
            data->modify([](shared_data& data)
            {
                for (int i = 0; i < 10; ++i)
                    data._shm_vector[i] = data._shm_vector[i] + 1;
            });

            ++ops;
        }

        auto now = std::chrono::steady_clock::now();
        if (now - start > std::chrono::seconds(1))
        {
            std::cout << double(ops) / std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() << "ops/ms" << std::endl;
            start = now;
            ops = 0;
        }
    }

    server.reset();

    return 0;
};
