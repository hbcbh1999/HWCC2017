#include "deploy.h"
#include <cstdio>
#include <cstring>
#include <queue>
#include <iostream>
#include <algorithm>
using namespace std;

int startTime;

#define N 1003
#define M 100003
const int oo = 1 << 24;
int consumer_id[N];
vi consumers;
vi best_servers, servers;
int flow_need;
int best_cost, total_cost, best_flow_cost, server_cost;
vector<vi> best_paths;

int a_bak[M][4], b_bak[N];
int tot_bak;

struct Graph
{
    int a[M][4];
    int b[N], p[N];
    int d[N];
    bool v[N];
    vi path, path_edge;
    vector<vi> paths;
    int n, s, t, tot;
    int flow, ans;
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
    }
    void add(int i, int j, int k, int c)
    {
        a[++tot][0] = b[i];
        b[i] = tot;
        a[tot][1] = j;
        a[tot][2] = k;
        a[tot][3] = c;
        a[++tot][0] = b[j];
        b[j] = tot;
        a[tot][1] = i;
        a[tot][2] = 0;
        a[tot][3] = -c;
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
            for (int i = b[k]; i; i = a[i][0])
            {
                int j = a[i][1];
                if (a[i][2] && d[k] + a[i][3] < d[j])
                {
                    d[j] = d[k] + a[i][3];
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
        int x = oo;
        for (k = t; k != s; k = a[i ^ 1][1])
        {
            i = p[k];
            x = min(x, a[i][2]);
        }
        if (x <= 0 || x == oo)
            return;
        flow += x;
        for (k = t; k != s; k = a[i ^ 1][1])
        {
            i = p[k];
            a[i][2] -= x;
            a[i ^ 1][2] += x;
            ans += a[i][3] * x;
        }
    }
    int solve()
    {
        while (spfa())
        {
            costflow();
            if (ans >= best_flow_cost)
                return ans;
        }
        return ans;
    }
    void get_one_path(int k, int x)
    {
        if (k == t)
        {
            path[int(path.size()) - 1] = x;
            flag = 1;
            return;
        }
        for (int i = b[k]; i; i = a[i][0])
            if (i % 2 == 0 && a[i ^ 1][2] > 0)
            {
                int j = a[i][1];
                path_edge.push_back(i ^ 1);
                path.push_back(j);
                get_one_path(j, min(x, a[i ^ 1][2]));
                if (flag)
                    return;
                path.pop_back();
                path_edge.pop_back();
            }
    }
    void get_paths()
    {
        paths.clear();
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
                a[path_edge[i]][2] -= x;
            flow -= x;
        }
    }
} g;

void g_save()
{
    tot_bak = g.tot;
    memcpy(a_bak, g.a, sizeof(a_bak));
    memcpy(b_bak, g.b, sizeof(b_bak));
}

void g_load()
{
    g.flow = 0;
    g.ans = 0;
    g.tot = tot_bak;
    memcpy(g.a, a_bak, sizeof(a_bak));
    memcpy(g.b, b_bak, sizeof(b_bak));
}

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
    int times = 1 << min(n_servers + 1, 13);
    int success_times = 0;
    best_flow_cost = best_cost - server_cost * n_servers;
    while (times--)
    {
        if (time(NULL) - init_time > 8)
            return 0;
        servers = get_servers(best_servers, n_servers);
        g_load();
        for (int i = 0; i < n_servers; ++i)
            g.add(g.s, servers[i], oo, 0);
        int flow_cost = g.solve();
        if (flow_cost > best_flow_cost || g.flow != flow_need)
            continue;
        best_flow_cost = flow_cost;
        ++success_times;
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

    flow_need = 0;
    for (int i = m + 2; i < m + 2 + c; ++i)
    {
        int idx = topo[i][1];
        consumer_id[idx] = topo[i][0];
        consumers.push_back(idx);
        g.add(idx, g.t, topo[i][2], 0);
        flow_need += topo[i][2];
        // init best_paths
        vi path;
        path.push_back(idx);
        path.push_back(topo[i][2]);
        best_paths.push_back(path);
    }
    g_save();
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
                best_cost = best_flow_cost + server_cost * mid;
                best_servers = servers;
                // printf("DEBUG %d\n", best_cost);
                g.get_paths();
                best_paths = g.paths;
            }
            else
                l = mid + 1;
        }
    }

    // solve, greedy
    while (1)
    {
        if (time(NULL) - startTime > 85)
            break;
        vi old_servers = best_servers;
        bool flag = 1;
        int tot = int(old_servers.size()) - 1;
        int init_time = time(NULL);
        best_flow_cost = best_cost - server_cost * tot;
        for (int k = 0; k <= tot; ++k)
        {
            if (time(NULL) - init_time > 2)
                break;
            servers = old_servers;
            servers[k] = servers[tot];
            servers.pop_back();
            g_load();
            for (int i = 0; i < tot; ++i)
                g.add(g.s, servers[i], oo, 0);
            int flow_cost = g.solve();
            if (flow_cost >= best_flow_cost || g.flow != flow_need)
                continue;
            flag = 0;
            best_flow_cost = flow_cost;
            best_servers = servers;
            g.get_paths();
            best_paths = g.paths;
        }
        if (flag)
            break;
        best_cost = best_flow_cost + server_cost * tot;
        // printf("DEBUG2 %d  servers: %d  time: %d\n", best_cost, tot, int(time(NULL) - startTime));
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
