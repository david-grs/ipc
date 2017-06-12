#include "common.h"
#include "libs/shm_client.h"

#include <chrono>
#include <iostream>

extern "C"
{
#include <signal.h>
}

// boo
volatile bool run = true;

int main(int argc, char *argv[])
{
    shm::client client("foob4r");
    client.start();

    auto& data = client.find<shared_data>("shared_data");

    auto start = std::chrono::steady_clock::now();
    int64_t ops = 0;

    ::signal(15, [](int) { run = false; });
    ::signal(2, [](int)  { run = false; });

    while (run)
    {
        for (int i = 0; i < 100; ++i)
        {
            data.read([](const shared_data& data)
            {
                int last = data._shm_vector[0];
                for (int i = 1; i < 10; ++i)
                {
                    const int elem = data._shm_vector[i];

                    if (elem != last + 1)
                        throw 42;

                    last = elem;
                }

                if (data._shm_map.size() != 2)
                    throw 42;
            });
        }

        for (int i = 0; i < 100; ++i)
        {
            data.modify([](shared_data& data)
            {
                for (int i = 0; i < 10; ++i)
                    data._shm_vector[i] = data._shm_vector[i] + 1;
            });
        }

        ops += 200;

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
