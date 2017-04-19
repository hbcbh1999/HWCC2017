import sys

if len(sys.argv) < 3:
    print('Run: python {} $input_path $output_path'.format(sys.argv[0]))
    exit()

input_path = sys.argv[1]
output_path = sys.argv[2]

fd = open(input_path)
inputs = [map(int, line.split()) for line in fd if line.strip() != '']
fd.close()

fd = open(output_path)
outputs = [map(int, line.split()) for line in fd if line.strip() != '']
fd.close()

# read input
n, m, c = inputs[0]
server_cost = dict()
i = 1
while i < len(inputs):
    if len(inputs[i]) != 3:
        break
    s_id, flow, cost = inputs[i]
    if s_id in server_cost:
        print('servers id {} duplicate'.format(s_id))
    server_cost[s_id] = [flow, cost]
    i += 1

node_cost = dict()
while i < len(inputs):
    if len(inputs[i]) != 2:
        break
    idx, cost = inputs[i]
    if idx in node_cost:
        print('node id {} duplicate'.format(idx))
    node_cost[idx] = cost
    i += 1

topo = dict()
tot = i + m
while i < tot:
    line = inputs[i]
    if line[0] not in topo:
        topo[line[0]] = dict()
    if line[1] not in topo:
        topo[line[1]] = dict()
    if line[1] in topo[line[0]]:
        print('{}->{} duplicate'.format(line[0], line[1]))
    if line[0] in topo[line[1]]:
        print('{}->{} duplicate'.format(line[1], line[0]))
    topo[line[0]][line[1]] = [line[2], line[3]]
    topo[line[1]][line[0]] = [line[2], line[3]]
    i += 1

consumers = dict()
tot = i + c
while i < tot:
    c_id, idx, need = inputs[i]
    if c_id in consumers:
        print('consumer id {} duplicate'.format(c_id))
    consumers[c_id] = [idx, need]
    i += 1

# check output
cost_flow = 0
cost_server = 0
cost_node = 0
servers = dict()
num_edge = outputs[0][0]
for i in range(1, num_edge + 1):
    line = outputs[i]
    flow = line[-2]
    for j in range(len(line) - 4):
        x = line[j]
        y = line[j + 1]
        topo[x][y][0] -= flow
        if topo[x][y][0] < 0:
            print('path {}: {}->{} wrong'.format(i, x, y))
        cost_flow += flow * topo[x][y][1]
    s_id = line[-1]
    c_id = line[-3]
    idx = line[-4]
    if consumers[c_id][0] != idx:
        print('path {}: node id {} and consumer id {} mismatch'.format(i, idx, c_id))
    consumers[c_id][1] -= flow
    idx = line[0]
    if idx not in servers:
        servers[idx] = server_cost[s_id][0]
        cost_server += server_cost[s_id][1]
        cost_node += node_cost[idx]
    servers[idx] -= flow

for c_id in consumers.keys():
    if consumers[c_id][1] != 0:
        print('consumer {}: {} needed'.format(c_id, consumers[c_id][1]))

for idx in servers.keys():
    if servers[idx] < 0:
        print('server {}: {} needed'.format(idx, -servers[idx]))

cost_total = cost_flow + cost_server + cost_node
print('{} = {} + {} + {}  servers: {}'.format(
    cost_total, cost_flow, cost_server, cost_node, len(servers.keys())))
