GIT_HOOKS := .git/hooks/applied
FLAGS=-O2 -Wall -Wextra 

SRCS=$(wildcard *.c)
OUT=$(SRCS:.c=.out)

all: $(GIT_HOOKS) $(OUT)
clean: 
	rm -rf $(OUT)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

%.out: %.c
	gcc -o $@ $^ $(FLAGS)
