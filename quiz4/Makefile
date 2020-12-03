GIT_HOOKS := .git/hooks/applied
FLAGS=-O2 -Wall -Wextra

all: $(GIT_HOOKS) fizzbuzz pageq kans ham nqueen
clean:
	rm -rf fizzbuzz pageq kans ham nqueen

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

fizzbuzz: fizzbuzz.c
	gcc $(FLAGS) -o fizzbuzz fizzbuzz.c

pageq: pageq.c
	gcc $(FLAGS) -o pageq pageq.c

kans: kans.c
	gcc $(FLAGS) -o kans kans.c

ham: ham.c
	gcc $(FLAGS) -o ham ham.c

nqueen: NQueen.c
	gcc $(FLAGS) -o nqueen -g -fsanitize=address NQueen.c

perf:
	sudo perf stat --repeat 100 -e cache-misses:u ./kans
