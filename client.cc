#include "shm_client.h"

#include <chrono>

int main(int argc, char *argv[])
{
    shm::client client("foob4r");
    client.start();

    auto start = std::chrono::steady_clock::now();
    int64_t ops = 0;

    while (1)
    {
        for (int i = 0; i < 100; ++i)
            client.read();
        for (int i = 0; i < 100; ++i)
            client.write();

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
