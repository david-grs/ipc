#include "shm_client.h"

#include <chrono>


void read(shm::data<shm::shared_data>& client)
{
    client.read([](const shm::shared_data& data)
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
    //segment.destroy<shm_vector>("shm_vector");
}

void write(shm::data<shm::shared_data>& client)
{
    client.modify([](shm::shared_data& data)
    {
        for (int i = 0; i < 10; ++i)
            data._shm_vector[i] = data._shm_vector[i] + 1;
    });
}

int main(int argc, char *argv[])
{
    shm::client client("foob4r");
    client.start();

    auto& data = *client.find<shm::shared_data>("shared_data");

    auto start = std::chrono::steady_clock::now();
    int64_t ops = 0;

    while (1)
    {
        for (int i = 0; i < 100; ++i)
            read(data);

        for (int i = 0; i < 100; ++i)
            write(data);

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
