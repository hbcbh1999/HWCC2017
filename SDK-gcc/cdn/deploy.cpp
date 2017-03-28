#include "deploy.h"
#include <cstdio>
#include <cstring>
#include <queue>
#include <iostream>
#include <algorithm>
using namespace std;

int startTime;

#define N 1005
#define M 100005
typedef int flow_type;
typedef int cost_type;
const int oo = 1 << 30;
struct Graph
{
    struct Edge
    {
        int x, y;
        flow_type z;
        cost_type c;
    } a[M];
    int n, s, t, tot;
    cost_type ans;
    int b[N], p[N];
    cost_type d[N];
    bool v[N];
    vi path;
    vector<vi> paths;
    void init(int _n, int _s, int _t)
    {
        n = _n;
        s = _s;
        t = _t;
        tot = 1;
        ans = 0;
        memset(b, 0, sizeof(b));
        path.clear();
        paths.clear();
    }
    void add(int i, int j, flow_type k, cost_type c)
    {
        a[++tot].x = b[i];
        b[i] = tot;
        a[tot].y = j;
        a[tot].z = k;
        a[tot].c = c;
        a[++tot].x = b[j];
        b[j] = tot;
        a[tot].y = i;
        a[tot].z = 0;
        a[tot].c = -c;
    }
    bool spfa()
    {
        queue<int> q;
        memset(v, 0, sizeof(v));
        memset(p, 0, sizeof(p));
        for (int i = 0; i < n; ++i)
            d[i] = oo;
        v[s] = 1;
        d[s] = 0;
        q.push(s);
        while (!q.empty())
        {
            int k = q.front();
            q.pop();
            for (int i = b[k]; i; i = a[i].x)
            {
                int j = a[i].y;
                if (a[i].z && d[k] + a[i].c < d[j])
                {
                    d[j] = d[k] + a[i].c;
                    p[j] = i;
                    if (!v[j])
                    {
                        q.push(j);
                        v[j] = 1;
                    }
                }
            }
            v[k] = 0;
        }
        return d[t] != oo;
    }
    void costflow()
    {
        int k, i;
        flow_type x = oo;
        for (k = t; k != s; k = a[i ^ 1].y)
        {
            i = p[k];
            x = min(x, a[i].z);
        }
        path.clear();
        for (k = t; k != s; k = a[i ^ 1].y)
        {
            i = p[k];
            a[i].z -= x;
            a[i ^ 1].z += x;
            ans += a[i].c * x;
            path.push_back(k);
        }
        if (x > 0)
        {
            reverse(path.begin(), path.end());
            path[path.size() - 1] = x;
            paths.push_back(path);
        }
    }
    int solve()
    {
        while (spfa())
            costflow();
        return ans;
    }
    bool is_full()
    {
        for (int i = b[t]; i; i = a[i].x)
            if (a[i ^ 1].z > 0)
                return 0;
        return 1;
    }
} g, gg;
int consumer_id[N];
vi consumers;

vi get_servers(int n)
{
    vi servers = consumers;
    random_shuffle(servers.begin(), servers.end());
    for (int i = servers.size() - n; i; --i)
        servers.pop_back();
    return servers;
}

void deploy_server(vector<vi> topo, char * filename)
{
    startTime = time(NULL);
    srand(startTime);

    int n = topo[0][0];
    int m = topo[0][1];
    int c = topo[0][2];
    int server_cost = topo[1][0];
    string ans;
    char a[20];

    // inputs
    g.init(n + 2, n, n + 1);

    for (int i = 2; i < m + 2; ++i)
    {
        g.add(topo[i][0], topo[i][1], topo[i][2], topo[i][3]);
        g.add(topo[i][1], topo[i][0], topo[i][2], topo[i][3]);
    }

    for (int i = m + 2; i < m + 2 + c; ++i)
    {
        int idx = topo[i][1];
        consumer_id[idx] = topo[i][0];
        consumers.push_back(idx);
        g.add(idx, g.t, topo[i][2], 0);
    }
    gg = g;

    // solve
    int num_servers = c;
    int best_cost = server_cost * num_servers;
    sprintf(a, "%d\n\n", c);
    ans = a;
    for (int i = 0; i < c; ++i)
    {
        int idx = topo[m + 2 + i][1];
        int k = topo[m + 2 + i][2];
        sprintf(a, "%d %d %d\n", idx, i, k);
        ans += a;
    }

    int max_times = min(1 << min(num_servers, 20), 8192);
    int times = 0;
    --num_servers;
    while (1)
    {
        if (time(NULL) - startTime > 60)
            break;

        g = gg;
        vi servers = get_servers(num_servers);
        int total_cost = server_cost * num_servers;
        for (int i = 0; i < num_servers; ++i)
            g.add(g.s, servers[i], oo, 0);

        total_cost += g.solve();

        if (!g.is_full())
        {
            // ++times;
            continue;
        }
        if (total_cost >= best_cost)
        {
            ++times;
            continue;
        }
        if (times >= max_times)
            break;
        best_cost = total_cost;
        // printf("DEBUG %d %d\n", best_cost, best_cost - server_cost * num_servers);
        times = 0;
        --num_servers;
        max_times = min(1 << min(num_servers, 20), 8192);

        // outputs
        int num_route = g.paths.size();
        sprintf(a, "%d\n\n", num_route);
        ans = a;
        for (int i = 0; i < num_route; ++i)
        {
            int k = g.paths[i].size();
            for (int j = 0; j < k - 1; ++j)
            {
                sprintf(a, "%d ", g.paths[i][j]);
                ans += a;
            }
            int last_node = g.paths[i][k - 2];
            sprintf(a, "%d %d\n", consumer_id[last_node], g.paths[i][k - 1]);
            ans += a;
        }
    }

    // try with the same num_server
    ++num_servers;
    while (1)
    {
        if (time(NULL) - startTime > 80)
            break;

        g = gg;
        vi servers = get_servers(num_servers);
        int total_cost = server_cost * num_servers;
        for (int i = 0; i < num_servers; ++i)
            g.add(g.s, servers[i], oo, 0);

        total_cost += g.solve();

        if (!g.is_full())
        {
            // ++times;
            continue;
        }
        if (total_cost >= best_cost)
        {
            ++times;
            continue;
        }
        if (times >= max_times)
            break;
        best_cost = total_cost;
        // printf("DEBUG %d %d\n", best_cost, best_cost - server_cost * num_servers);
        times = 0;

        // outputs
        int num_route = g.paths.size();
        sprintf(a, "%d\n\n", num_route);
        ans = a;
        for (int i = 0; i < num_route; ++i)
        {
            int k = g.paths[i].size();
            for (int j = 0; j < k - 1; ++j)
            {
                sprintf(a, "%d ", g.paths[i][j]);
                ans += a;
            }
            int last_node = g.paths[i][k - 2];
            sprintf(a, "%d %d\n", consumer_id[last_node], g.paths[i][k - 1]);
            ans += a;
        }
    }

    char * topo_file = (char *)ans.c_str();
    write_result(topo_file, filename);
    // printf("DEBUG time: %d\n", time(NULL) - startTime);
}
