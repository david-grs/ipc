#include "shm_client.h"

int main(int argc, char *argv[])
{
    shm_client client("foob4r");
    client.start();
    return 0;
};
