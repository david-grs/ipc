#include "shm_server.h"

#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{
    shm::server server("foob4r");
    server.start();

    auto& data = *server.construct<shm::shared_data>("shared_data");

    data.modify([&server](shm::shared_data& data)
    {
        for (int i = 0; i < 10; ++i)
            data._shm_vector.push_back(i);

        data._shm_map.emplace(shm::string("foo", server.allocator()), shm::mmdata{10.0, 3});
        data._shm_map.emplace(shm::string("bar", server.allocator()), shm::mmdata{4.0, 11});
    });

    auto start = std::chrono::steady_clock::now();
    int64_t ops = 0;

    while (1)
    {
        for (int i = 0; i < 100; ++i)
        {
            data.modify([](shm::shared_data& data)
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

    return 0;
};
