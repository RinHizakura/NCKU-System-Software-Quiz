GIT_HOOKS := .git/hooks/applied
FLAGS=-O2 -Wall -Wextra

all: $(GIT_HOOKS) str_check hexspeak fastdiv single_num
clean: 
	rm -rf str_check hexspeak fastdiv single_num


$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

str_check: str_check.c
	gcc $(FLAGS) -msse4 str_check.c -o str_check

hexspeak: hexspeak.c
	gcc $(FLAGS) hexspeak.c -o hexspeak

fastdiv: fastdiv.c
	gcc -O0 -Wall -Wextra fastdiv.c -o fastdiv

single_num: single_num.c
	gcc $(FLAGS) single_num.c -o single_num

