#include "shm_server.h"

int main(int argc, char *argv[])
{
    shm_server server("foob4r");
    server.start();
    return 0;
};
