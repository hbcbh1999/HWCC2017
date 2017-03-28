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
server_cost = inputs[1][0]
topo = dict()
for i in range(2, m + 2):
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

consumers = dict()
for i in range(m + 2, m + 2 + c):
    c_id, idx, need = inputs[i]
    if c_id in consumers:
        print('consumer id {} duplicate'.format(c_id))
    consumers[c_id] = [idx, need]

# check output
flow_cost = 0
num_edge = outputs[0][0]
servers = set()
for i in range(1, num_edge + 1):
    flow = outputs[i][-1]
    for j in range(len(outputs[i]) - 3):
        x = outputs[i][j]
        y = outputs[i][j + 1]
        topo[x][y][0] -= flow
        if topo[x][y][0] < 0:
            print('path {}: {}->{} wrong'.format(i, x, y))
        flow_cost += flow * topo[x][y][1]
    c_id = outputs[i][-2]
    idx = outputs[i][-3]
    if consumers[c_id][0] != idx:
        print('path {}: node id {} and consumer id {} mismatch'.format(i, idx, c_id))
    consumers[c_id][1] -= flow
    servers.add(outputs[i][0])

for c_id in consumers.keys():
    if consumers[c_id][1] != 0:
        print('consumer {}: {} needed'.format(c_id, consumers[c_id][1]))

total_cost = flow_cost + len(servers) * server_cost
print('total cost: {}, servers: {}, flow cost: {}'.format(total_cost, len(servers), flow_cost))
