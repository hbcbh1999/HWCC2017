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
int best_cost, best_flow_cost, server_cost;
vector<vi> best_paths;

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
        paths.clear();
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
                return oo;
        }
        if (flow != flow_need)
            return oo;
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
} g, gg;

int get_flow_cost(vi & servers)
{
    g = gg;
    int n_servers = int(servers.size());
    for (int i = 0; i < n_servers; ++i)
        g.add(g.s, servers[i], oo, 0);
    return g.solve();
}

struct Particle
{
    vector<double> v, v_best, vp;
    int cost, cost_best, n;
    Particle() {}
    Particle(int _n, vi &servers, int _cost) : v(_n, 0), v_best(_n, 0), vp(_n, 0)
    {
        n = _n;
        cost_best = cost = _cost;
        int size = servers.size();
        for (int i = 0; i < size; ++i)
            v_best[servers[i]] = v[servers[i]] = 1;
    }
    bool operator<(const Particle & p) const
    {
        return cost_best < p.cost_best;
    }
    vi get_servers()
    {
        vi servers;
        for (int i = 0; i < n; ++i)
            if (v[i] > 0.5)
                servers.push_back(i);
        return servers;
    }
    vi get_servers_best()
    {
        vi servers;
        for (int i = 0; i < n; ++i)
            if (v_best[i] > 0.5)
                servers.push_back(i);
        return servers;
    }
    inline void run()
    {
        int x = rand() % n;
        int y = rand() % n;
        while (v[x] <= 0.5)
            x = rand() % n;
        while (v[y] >= 0.5)
            y = rand() % n;
        swap(v[x], v[y]);
    }
    inline void update()
    {
        servers = get_servers();
        int flow_cost = get_flow_cost(servers);
        cost = flow_cost + server_cost * servers.size();
        if (cost < cost_best)
        {
            v_best = v;
            cost_best = cost;
        }
    }
};

struct PSO
{
    vector<Particle> particles;
    Particle p_best;
    double c1, c2, w;
    int n;
    void init(int _n, double _c1, double _c2, double _w)
    {
        c1 = _c1;
        c2 = _c2;
        w = _w;
        n = min(_n, int(particles.size()));
        sort(particles.begin(), particles.end());
        // printf("DEBUG %d %d\n", n, int(particles.size()));
        // for (int i = 0; i < n; ++i)
        //     printf("DEBUG particles %d %d\n", i, particles[i].cost);
        p_best = particles[0];
    }
    void add(Particle & p)
    {
        particles.push_back(p);
    }
    void solve()
    {
        for (int i = 0; i < n; ++i)
        {
            particles[i].run();
            particles[i].update();
            if (particles[i] < p_best)
            {
                p_best.v_best = particles[i].v_best;
                p_best.cost_best = particles[i].cost_best;
            }
            update(particles[i]);
        }
    }
    inline void update(Particle & p)
    {
        for (int i = 0; i < p.n; ++i)
        {
            p.vp[i] = w * p.vp[i] + c1 * rand() / RAND_MAX * (p.v_best[i] - p.v[i]) + c2 * rand() / RAND_MAX * (p_best.v_best[i] - p.v[i]);
            p.v[i] = 1 / (1 + exp(100 * (0.5 - p.v[i] - p.vp[i])));
        }
    }
} pso;

vi get_servers(vi & servers, int n_servers)
{
    vi ans(n_servers, 0);
    random_shuffle(servers.begin(), servers.end());
    for (int i = 0; i < n_servers; ++i)
        ans[i] = servers[i];
    return ans;
}

bool work(int n_servers)
{
    int init_time = time(NULL);
    while (time(NULL) - init_time < 2)
    {
        servers = get_servers(best_servers, n_servers);
        int flow_cost = get_flow_cost(servers);
        if (g.flow == flow_need)
        {
            flow_cost += server_cost * n_servers;
            if (flow_cost < best_cost)
            {
                best_cost = flow_cost;
                best_servers = servers;
            }
            return 1;
        }
    }
    return 0;
}

void solve_greedy()
{
    while (1)
    {
        if (time(NULL) - startTime > 85)
            break;
        vi old_servers = best_servers;
        bool flag = 1;
        int n_servers = int(old_servers.size()) - 1;
        int init_time = time(NULL);
        best_flow_cost = best_cost - server_cost * n_servers;
        for (int k = 0; k <= n_servers; ++k)
        {
            if (time(NULL) - init_time > 1)
                break;
            servers = old_servers;
            servers[k] = servers[n_servers];
            servers.pop_back();
            int flow_cost = get_flow_cost(servers);
            if (flow_cost >= best_flow_cost)
                continue;
            flag = 0;
            best_flow_cost = flow_cost;
            best_servers = servers;
            g.get_paths();
            best_paths = g.paths;
        }
        if (flag)
            break;
        best_cost = best_flow_cost + server_cost * n_servers;
    }
    printf("DEBUG_greedy %d  servers: %d  time: %d\n", best_cost, int(best_servers.size()), int(time(NULL) - startTime));
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
    gg = g;
    best_cost = server_cost * c;
    best_servers = consumers;

    // solve, greedy
    if (c <= 150)
    {
        solve_greedy();
    }

    // solve
    best_flow_cost = oo;
    int l = 0, r = c;
    while (l < r)
    {
        if (time(NULL) - startTime > 20)
            break;
        int mid = (l + r) >> 1;
        if (work(mid))
            r = mid;
        else
            l = mid + 1;
    }
    // printf("DEBUG %d  servers: %d r=%d time: %d\n", best_cost, int(best_servers.size()), r, int(time(NULL) - startTime));
    Particle p(n, best_servers, best_cost);
    pso.add(p);
    for (int i = min(r + 9, c - 1); i >= r - 10; --i)
    {
        servers = get_servers(consumers, i);
        int flow_cost = get_flow_cost(servers);
        Particle p(n, servers, flow_cost + server_cost * i);
        pso.add(p);
    }
    pso.init(8, 1.0, 1.6, 0.9);
    while (time(NULL) - startTime <= 75)
        pso.solve();
    best_servers = pso.p_best.get_servers_best();
    int flow_cost = get_flow_cost(best_servers);
    best_cost = flow_cost + server_cost * best_servers.size();
    // printf("DEBUG pso %d  servers: %d  time: %d\n", best_cost, int(best_servers.size()), int(time(NULL) - startTime));
    g.get_paths();
    best_paths = g.paths;

    solve_greedy();

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
