CC=gcc
INC=./include
LOGC=./src/make_log.c

src = $(wildcard ./test/*.c)

obj = $(patsubst ./test/%.c, ./bin/%, $(src))

ALL:$(obj)


$(obj):./bin/%:./test/%.c
	$(CC) -I $(INC) $(LOGC) $< -o $@ 

clean:
	rm $(obj)

.PHONY: clean ALL
