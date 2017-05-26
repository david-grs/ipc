#include "shm_client.h"

#include <chrono>

int main(int argc, char *argv[])
{
    shm_client client("foob4r");
    client.start();

    auto start = std::chrono::steady_clock::now();
    while (1)
    {
        for (int i = 0; i < 100; ++i)
            client.read();

        auto now = std::chrono::steady_clock::now();
        if (now - start > std::chrono::seconds(1))
        {
            std::cout << double(client.count()) / std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() << "ops/ms" << std::endl;
            start = now;
            client.reset();
        }
    }
    return 0;
};
