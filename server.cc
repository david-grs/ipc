#include "shm_server.h"

#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{
    shm_server server("foob4r");
    server.start();

    auto start = std::chrono::steady_clock::now();
    while (1)
    {
        for (int i = 0; i < 100; ++i)
        {
            // std::this_thread::sleep_for(std::chrono::microseconds(1));
            server.update();
        }

        auto now = std::chrono::steady_clock::now();
        if (now - start > std::chrono::seconds(1))
        {
            std::cout << double(server.count()) / std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() << "ops/ms" << std::endl;
            start = now;
            server.reset();
        }
    }

    return 0;
};
