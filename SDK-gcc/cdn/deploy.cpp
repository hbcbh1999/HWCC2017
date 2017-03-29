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
const int oo = 1 << 24;
int consumer_id[N];
vi consumers;
vi best_servers, servers;
int best_cost, total_cost, total_servers_cost, server_cost;
vector<vi> best_paths;

struct Graph
{
    struct Edge
    {
        int x, y;
        flow_type z;
        cost_type c;
    } a[M];
    int n, s, t, tot;
    flow_type flow;
    cost_type ans;
    int b[N], p[N];
    cost_type d[N];
    bool v[N];
    vi path, path_edge;
    vector<vi> paths;
    bool flag;
    void init(int _n, int _s, int _t)
    {
        n = _n;
        s = _s;
        t = _t;
        tot = 1;
        flow = 0;
        ans = 0;
        memset(b, 0, sizeof(b));
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
        if (x <= 0 || x == oo)
            return;
        flow += x;
        for (k = t; k != s; k = a[i ^ 1].y)
        {
            i = p[k];
            a[i].z -= x;
            a[i ^ 1].z += x;
            ans += a[i].c * x;
        }
    }
    int solve()
    {
        while (spfa())
        {
            costflow();
            if (ans + total_servers_cost >= best_cost)
                return ans;
        }
        return ans;
    }
    bool is_full()
    {
        for (int i = b[t]; i; i = a[i].x)
            if (a[i ^ 1].z != 0)
                return 0;
        return 1;
    }
    void get_one_path(int k, int x)
    {
        if (k == t)
        {
            path[int(path.size()) - 1] = x;
            flag = 1;
            return;
        }
        for (int i = b[k]; i; i = a[i].x)
            if (i % 2 == 0 && a[i ^ 1].z > 0)
            {
                int j = a[i].y;
                path_edge.push_back(i ^ 1);
                path.push_back(j);
                get_one_path(j, min(x, a[i ^ 1].z));
                if (flag)
                    return;
                path.pop_back();
                path_edge.pop_back();
            }
    }
    void get_paths()
    {
        while (flow > 0)
        {
            flag = 0;
            path.clear();
            path_edge.clear();
            get_one_path(s, oo);
            if (!flag)
                break;
            paths.push_back(path);
            int x = path[path.size() - 1];
            for (int i = 0; i < int(path_edge.size()); ++i)
                a[path_edge[i]].z -= x;
            flow -= x;
        }
    }
} g, gg;

vi get_servers(vi servers, int n_servers)
{
    random_shuffle(servers.begin(), servers.end());
    for (int i = servers.size() - n_servers; i; --i)
        servers.pop_back();
    return servers;
}

bool work(int n_servers)
{
    int init_time = time(NULL);
    int times = min(1 << min(n_servers + 1, 20), 8192);
    int success_times = 0;
    int best = best_cost;
    total_servers_cost = server_cost * n_servers;
    while (times--)
    {
        if (time(NULL) - init_time > 8)
            return 0;
        if (time(NULL) - startTime > 50)
            return 0;
        servers = get_servers(best_servers, n_servers);
        g = gg;
        for (int i = 0; i < n_servers; ++i)
            g.add(g.s, servers[i], oo, 0);
        total_cost = total_servers_cost + g.solve();
        if (!g.is_full())
            continue;
        if (total_cost <= best)
        {
            best = total_cost;
            ++success_times;
        }
        if (success_times > 2)
            return 1;
    }
    return 0;
}

void deploy_server(vector<vi> topo, char * filename)
{
    startTime = time(NULL);
    srand(startTime);

    int n = topo[0][0];
    int m = topo[0][1];
    int c = topo[0][2];
    server_cost = topo[1][0];

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
        // init best_paths
        vi path;
        path.push_back(idx);
        path.push_back(topo[i][2]);
        best_paths.push_back(path);
    }
    gg = g;
    best_cost = server_cost * c;
    best_servers = consumers;

    // solve, random
    if (c > 200)
    {
        int l = 0, r = c;
        while (l < r)
        {
            if (time(NULL) - startTime > 50)
                break;
            int mid = (l + r) >> 1;
            // printf("DEBUG %d %d %d\n", l, r, mid);
            if (work(mid))
            {
                r = mid;
                best_cost = total_cost;
                best_servers = servers;
                // printf("DEBUG %d %d\n", best_cost, best_cost - total_servers_cost);
                g.get_paths();
                best_paths = g.paths;
            }
            else
                l = mid + 1;
        }
    }

    // int best_cost1 = best_cost;
    // vector<vi> best_paths1 = best_paths;
    // best_cost = server_cost * c;
    // best_servers = consumers;

    // solve, greedy
    while (1)
    {
        vi old_servers = best_servers;
        bool flag = 1;
        int tot = int(old_servers.size()) - 1;
        int init_time = time(NULL);
        total_servers_cost = server_cost * tot;
        for (int k = 0; k <= tot; ++k)
        {
            if (time(NULL) - init_time > 1)
                break;
            if (time(NULL) - startTime > 86)
                break;
            servers = old_servers;
            servers[k] = servers[tot];
            servers.pop_back();
            g = gg;
            for (int i = 0; i < tot; ++i)
                g.add(g.s, servers[i], oo, 0);
            int total_cost = total_servers_cost + g.solve();
            if (total_cost >= best_cost || !g.is_full())
                continue;
            flag = 0;
            best_cost = total_cost;
            best_servers = servers;
            g.get_paths();
            best_paths = g.paths;
        }
        if (flag)
            break;
        // printf("DEBUG2 %d  servers: %d  time: %d\n", best_cost, tot, int(time(NULL) - startTime));
    }

    // if (best_cost1 < best_cost)
    // {
    //     best_cost = best_cost1;
    //     best_paths = best_paths1;
    // }

    // outputs
    string ans;
    char a[20];
    int n_route = best_paths.size();
    sprintf(a, "%d\n\n", n_route);
    ans = a;
    for (int i = 0; i < n_route; ++i)
    {
        int k = best_paths[i].size();
        for (int j = 0; j < k - 1; ++j)
        {
            sprintf(a, "%d ", best_paths[i][j]);
            ans += a;
        }
        int last_node = best_paths[i][k - 2];
        sprintf(a, "%d %d\n", consumer_id[last_node], best_paths[i][k - 1]);
        ans += a;
    }
    char * topo_file = (char *)ans.c_str();
    write_result(topo_file, filename);
    // printf("DEBUG time: %d\n", int(time(NULL) - startTime));
}
