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
            costflow();
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
int consumer_id[N];
vi consumers;
vi best_servers, servers;
int best_cost, total_cost, server_cost;
vector<vi> best_paths;

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
    while (times--)
    {
        if (time(NULL) - init_time > 8)
            return 0;
        if (time(NULL) - startTime > 80)
            return 0;
        servers = get_servers(best_servers, n_servers);
        total_cost = server_cost * n_servers;
        g = gg;
        for (int i = 0; i < n_servers; ++i)
            g.add(g.s, servers[i], oo, 0);
        total_cost += g.solve();
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
    best_cost = server_cost * c;

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
        best_servers.push_back(idx);
        best_paths.push_back(path);
    }
    gg = g;

    // solve
    int l = 0, r = c;
    while (l < r)
    {
        if (time(NULL) - startTime > 80)
            break;

        int mid = (l + r) >> 1;
        // printf("DEBUG %d %d %d\n", l, r, mid);
        if (work(mid))
        {
            r = mid;
            best_cost = total_cost;
            best_servers = servers;
            // printf("DEBUG %d %d\n", best_cost, best_cost - server_cost * mid);
            g.get_paths();
            best_paths = g.paths;
        }
        else
            l = mid + 1;
    }

    // solve, stage 2
    int n_servers = best_servers.size();
    int times = min(1 << min(n_servers + 1, 20), 65536);
    while (times--)
    {
        if (time(NULL) - startTime > 80)
            break;
        servers = get_servers(consumers, n_servers);
        total_cost = server_cost * n_servers;
        g = gg;
        for (int i = 0; i < n_servers; ++i)
            g.add(g.s, servers[i], oo, 0);
        total_cost += g.solve();
        if (!g.is_full())
            continue;
        if (total_cost >= best_cost)
            continue;
        best_cost = total_cost;
        best_servers = servers;
        // printf("DEBUG2 %d %d\n", best_cost, best_cost - server_cost * n_servers);
        g.get_paths();
        best_paths = g.paths;
    }

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
