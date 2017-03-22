#include "deploy.h"
#include "lib_time.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
    print_time("Begin");

    char *topo_file = argv[1];

    vector<vi> topo = read_file(topo_file);

    char *result_file = argv[2];

    deploy_server(topo, result_file);

    print_time("End");

    return 0;
}

