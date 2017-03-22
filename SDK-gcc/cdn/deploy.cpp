#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

void deploy_server(vector<vi> topo, char * filename)
{
    int n = topo[0][0];
    int m = topo[0][1];
    int c = topo[0][2];
    string ans;
    char a[20];
    sprintf(a, "%d\n\n", c);
    ans = a;

    for (int i = 0; i < c; ++i)
    {
        int idx = topo[m + 2 + i][1];
        int k = topo[m + 2 + i][2];
        sprintf(a, "%d %d %d", idx, i, k);
        ans += a;
        if (i != c - 1)
        {
            ans += "\n";
        }
    }

    char * topo_file = (char *)ans.c_str();
    write_result(topo_file, filename);
}
