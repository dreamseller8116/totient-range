CC=gcc

totient: TotientRange.c
	$(CC) -o totient TotientRange.c -O3

totient-prof: TotientRange.c
	@$(CC) -o totient TotientRange.c -g -pg 

test: totient
	./totient 0 100
	./totient 0 1000
	./totient 5 3000
	./totient 100 10000

bench: clean totient
	srun --partition=amd-shortq ./totient 1 15000
	srun --partition=amd-shortq ./totient 1 15000
	srun --partition=amd-shortq ./totient 1 15000
	srun --partition=amd-shortq ./totient 1 30000
	srun --partition=amd-shortq ./totient 1 30000
	srun --partition=amd-shortq ./totient 1 30000
	srun --partition=amd-shortq ./totient 1 100000	
	srun --partition=amd-shortq ./totient 1 100000	
	srun --partition=amd-shortq ./totient 1 100000

profile: clean totient-prof
	@srun --partition=amd-shortq ./totient 1 15000
	gprof totient gmon.out

.PHONY: clean

clean:
	rm -f totient
