CFLAGS = -Wall -Wextra -Werror -Wfatal-errors -std=c99 -pedantic
LFLAGS = -lpthread
BIN    = bin/bot


.SILENT:

debug: CFLAGS += -DDEBUG=1 -g
debug: build

release: CFLAGS += -DDEBUG=0 -DNDEBUG -O3
release: build

clean:
	rm -rf bin/ *.core

run:
	./$(BIN)


build:
	mkdir -p $(dir $(BIN))
	$(CC) -o $(BIN) $(CFLAGS) $(LFLAGS) $(shell find src/ -name '*.c')

