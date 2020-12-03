GIT_HOOKS := .git/hooks/applied
FLAGS=-O2 -Wall -Wextra

all: $(GIT_HOOKS) asr is_power4 num_of_step gcd bitarray bit_comp first_miss
clean: 
	rm -rf asr is_power4 num_of_step gcd bitarray bit_comp first_miss

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

asr: asr.c
	gcc $(FLAGS) -fsanitize=undefined asr.c -o asr

is_power4: is_power4.c
	gcc $(FLAGS) is_power4.c -o is_power4

bit_comp: bit_comp.c
	gcc $(FLAGS) bit_comp.c -o bit_comp

first_miss: first_miss.c
	gcc $(FLAGS) first_miss.c -o first_miss

num_of_step: num_of_step.c
	gcc $(FLAGS) num_of_step.c -o num_of_step

gcd: gcd.c
	gcc $(FLAGS) gcd.c -o gcd

bitarray: bitarray.c
	gcc $(FLAGS) bitarray.c -o bitarray
