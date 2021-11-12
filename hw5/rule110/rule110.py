#!/trinity/shared/opt/python/3.7.1/bin/python3.7
from mpi4py import MPI
from argparse import ArgumentParser
from matplotlib import pyplot as plt
import numpy as np

RANDOM_SEED = 1


comm = MPI.COMM_WORLD
psize = comm.Get_size()
prank = comm.Get_rank()

def update_cells(buffer, cyclic):
	result = buffer.copy()
	if psize == 1:
		return result
	if prank == 0:
		comm.send(result[-2], dest=prank+1)
		result[-1] = comm.recv(source=prank+1)
		if cyclic:
			comm.send(result[1], dest=psize-1)
			result[0] = comm.recv(source=psize-1)
	elif prank == psize-1:
		comm.send(result[1], dest=prank-1)
		result[0] = comm.recv(source=prank-1)
		if cyclic:
	                comm.send(result[-2], dest=0)
        	        result[-1] = comm.recv(source=0)
	else:
                comm.send(result[1], dest=prank-1)
                result[0] = comm.recv(source=prank-1)
                comm.send(result[-2], dest=prank+1)
                result[-1] = comm.recv(source=prank+1)
	return result


def make_step(buffer, rule):
	idx = 4*np.roll(buffer, 1) + 2*buffer + np.roll(buffer, -1)
	idx = np.roll(idx, -1)
	result = rule[idx]
	result = np.roll(result, 1)
	if not cyclic:
		if prank == 0:
			result[0] = 0
		if prank == psize-1:
			result[-1] = 0
	return result

def save_result(buffer, path):
	plt.figure(figsize=(8, 8))
	plt.axis("off")
	plt.imshow(buffer, cmap=plt.cm.binary)
	plt.savefig(f"{path}.png")

parser = ArgumentParser()
parser.add_argument("rule", type=int)
parser.add_argument("length", type=int)
parser.add_argument("epochs", type=int)
parser.add_argument("--cyclic", action="store_true")
args = parser.parse_args()

# parse args
rule_bits = np.binary_repr(args.rule).zfill(8)
rule = np.array([int(s) for s in reversed(rule_bits)], dtype=np.int)
length = args.length
epochs = args.epochs
cyclic = True if args.cyclic else False
path = f"{args.rule}_{length}_{epochs}_{psize}"
path += "_cyclic" if cyclic else "_bounded"
window = np.ceil(length/psize).astype(np.int)

np.random.seed(RANDOM_SEED)
buffer = np.random.randint(2, size=window+2)
if not cyclic:
	if prank == 0:
		buffer[0] = 0
	if prank == psize-1:
		buffer[-1] = 0

timestamp = MPI.Wtime()

result = np.zeros((epochs,window), dtype=np.int)
result[0,:] = buffer[1:-1]
for i in range(1, epochs):
	buffer = update_cells(buffer, cyclic)
	buffer = make_step(buffer, rule)
	result[i,:] = buffer[1:-1]

cumm_result = comm.gather(result, root=0)

if prank == 0:
	timestamp = MPI.Wtime() - timestamp
	print(f"{timestamp}")
	output = np.hstack(cumm_result)
	save_result(output, path)


MPI.Finalize()

