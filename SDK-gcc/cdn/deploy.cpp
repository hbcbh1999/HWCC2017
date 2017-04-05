#include "deploy.h"
#include <cstdio>
#include <cstring>
#include <deque>
#include <iostream>
#include <algorithm>
using namespace std;

int startTime;

#define N 1003
#define M 100003
const int oo = 16843009;
int consumer_id[N];
vi consumers;
vi best_servers, servers;
int flow_need;
int best_cost, best_flow_cost, flow_cost, server_cost;
vector<vi> best_paths;

struct Graph
{
    int a[M][6];
    int b[N], d[N];
    bool v[N];
    int s, t, tot, tot_tmp;
    int cost, flow, dist;
    void init(int _s, int _t)
    {
        s = _s;
        t = _t;
        tot = 1;
        dist = 0;
        flow = 0;
        cost = 0;
        memset(b, 0, sizeof(b));
    }
    void add(int i, int j, int k, int c)
    {
        a[++tot][0] = b[i];
        b[i] = tot;
        a[tot][1] = j;
        a[tot][2] = k;
        a[tot][3] = c;
        a[tot][4] = k;
        a[tot][5] = c;
        a[++tot][0] = b[j];
        b[j] = tot;
        a[tot][1] = i;
        a[tot][2] = 0;
        a[tot][3] = -c;
        a[tot][4] = 0;
        a[tot][5] = -c;
    }
    int sap(int k, int now)
    {
        if (k == t)
        {
            cost += dist * now;
            flow += now;
            return now;
        }
        int x = now;
        v[k] = 1;
        for (int i = b[k]; i; i = a[i][0])
        {
            int j = a[i][1];
            if (a[i][2] && !a[i][3] && !v[j])
            {
                int f = sap(j, min(x, a[i][2]));
                a[i][2] -= f;
                a[i ^ 1][2] += f;
                x -= f;
                if (!x)
                    return now;
            }
        }
        return now - x;
    }
    bool flag()
    {
        deque<int> q;
        memset(v, 0, sizeof(v));
        memset(d, 1, sizeof(d));
        q.push_back(s);
        d[s] = 0;
        v[s] = 1;
        while (!q.empty())
        {
            int k = q.front();
            q.pop_front();
            for (int i = b[k]; i; i = a[i][0])
            {
                int j = a[i][1];
                if (!a[i][2])
                    continue;
                int dis = d[k] + a[i][3];
                if (dis < d[j])
                {
                    d[j] = dis;
                    if (!v[j])
                    {
                        v[j] = 1;
                        if (q.size() && d[j] < d[q[0]])
                            q.push_front(j);
                        else
                            q.push_back(j);
                    }
                }
            }
            v[k] = 0;
        }
        for (int i = 1; i <= tot; ++i)
            a[i][3] -= d[a[i][1]] - d[a[i ^ 1][1]];
        dist += d[t];
        return d[t] < oo;
    }
    int costflow()
    {
        cost = flow = dist = 0;
        do
        {
            while (sap(s, oo) > 0)
                memset(v, 0, sizeof(v));
        } while (flag());
        if (flow != flow_need)
            cost = oo;
        return cost;
    }
    void add_server(vi & servers)
    {
        if (tot != tot_tmp)
        {
            tot = tot_tmp;
            for (int i = b[s]; i; i = a[i][0])
            {
                int j = a[i][1];
                b[j] = a[b[j]][0];
            }
            b[s] = 0;
            for (int i = 1; i <= tot; ++i)
            {
                a[i][2] = a[i][4];
                a[i][3] = a[i][5];
            }
        }
        for (int i = 0; i < int(servers.size()); ++i)
            add(s, servers[i], oo, 0);
    }
    vector<vi> get_paths()
    {
        vector<vi> paths;
        while (1)
        {
            vi path;
            int x = oo;
            int k = s;
            while (k != t)
            {
                bool flag = 1;
                for (int i = b[k]; i; i = a[i][0])
                {
                    int j = a[i][1];
                    if (a[i][4] > a[i][2])
                    {
                        x = min(x, a[i][4] - a[i][2]);
                        k = j;
                        flag = 0;
                        break;
                    }
                }
                if (flag)
                    return paths;
            }
            k = s;
            while (k != t)
            {
                for (int i = b[k]; i; i = a[i][0])
                {
                    int j = a[i][1];
                    if (a[i][4] > a[i][2])
                    {
                        a[i][2] += x;
                        k = j;
                        break;
                    }
                }
                path.push_back(k);
            }
            path[int(path.size()) - 1] = x;
            paths.push_back(path);
        }
        return paths;
    }
} g;

inline int get_flow_cost(vi & servers)
{
    g.add_server(servers);
    return g.costflow();
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
        int cnt = 0;
        while (v[x] <= 0.5 && ++cnt < n)
            x = rand() % n;
        cnt = 0;
        while (v[y] >= 0.5 && ++cnt < n)
            y = rand() % n;
        swap(v[x], v[y]);
    }
    inline void update()
    {
        servers = get_servers();
        flow_cost = get_flow_cost(servers);
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
    while (time(NULL) - init_time < 1)
    {
        servers = get_servers(best_servers, n_servers);
        flow_cost = get_flow_cost(servers);
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
    while (time(NULL) - startTime <= 85)
    {
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
            flow_cost = get_flow_cost(servers);
            if (flow_cost >= best_flow_cost)
                continue;
            flag = 0;
            best_flow_cost = flow_cost;
            best_servers = servers;
            best_paths = g.get_paths();
        }
        if (flag)
            break;
        best_cost = best_flow_cost + server_cost * n_servers;
    }
    // printf("DEBUG_greedy %d  servers: %d  time: %d\n", best_cost, int(best_servers.size()), int(time(NULL) - startTime));
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
    g.init(n, n + 1);

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
    g.tot_tmp = g.tot;
    best_cost = server_cost * c;
    best_servers = consumers;

    // solve, greedy
    if (c <= 150)
    {
        solve_greedy();
    }

    // solve
    if (c > 1)
    {
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
        int mi = max(0, r - 5);
        int ma = min(c - 1, r + 5);
        for (int i = mi; i < ma; ++i)
        {
            work(i);
            Particle p(n, servers, flow_cost + server_cost * i);
            pso.add(p);
        }
        pso.init(10, 1.0, 1.6, 0.9);
        while (time(NULL) - startTime <= 75)
            pso.solve();
        best_servers = pso.p_best.get_servers_best();
        int flow_cost = get_flow_cost(best_servers);
        best_cost = flow_cost + server_cost * best_servers.size();
        // printf("DEBUG pso %d  servers: %d  time: %d\n", best_cost, int(best_servers.size()), int(time(NULL) - startTime));
        best_paths = g.get_paths();

        solve_greedy();
    }

    // outputs
    string ans;
    char a[20];
    int n_route = best_paths.size();
    sprintf(a, "%d\n\n", n_route);
    ans = a;
    for (int i = 0; i < n_route; ++i)
    {
        int k = best_paths[i].size() - 1;
        for (int j = 0; j < k; ++j)
        {
            sprintf(a, "%d ", best_paths[i][j]);
            ans += a;
        }
        int last_node = best_paths[i][k - 1];
        sprintf(a, "%d %d\n", consumer_id[last_node], best_paths[i][k]);
        ans += a;
    }
    char * topo_file = (char *)ans.c_str();
    write_result(topo_file, filename);
    // printf("DEBUG time: %d\n", int(time(NULL) - startTime));
}
