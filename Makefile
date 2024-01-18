SRC=sutil.c mutil.c printf.c log.c cfg.c vec3.c ray.c tri.c aabb.c mesh.c bvh.c cam.c view.c main.c
OBJ=$(patsubst %.c,obj/%.o,$(SRC))
OUT=main

CFLAGS=-std=c2x -Os -nostdlib -ffast-math -flto -pedantic-errors -Wall -Wextra -Wno-unused-parameter $(shell sdl2-config --cflags)
LDFLAGS=-s --lto
LIBS=$(shell sdl2-config --libs)

all: $(OUT)

.PHONY: all clean

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

obj/%.o: src/%.c
	@mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj $(OUT)
