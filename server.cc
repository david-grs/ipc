#include "shm_server.h"

#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{
    shm_server server("foob4r");
    server.start();
    while (1)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
};