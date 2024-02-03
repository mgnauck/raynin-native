SRC=sutil.c mutil.c printf.c log.c cfg.c vec3.c mat4.c ray.c tri.c aabb.c mesh.c bvh.c bvhinst.c tlas.c pool.c cam.c view.c intersect.c main.c
OBJ=$(patsubst %.c,obj/%.o,$(SRC))
OUT=main

CC=clang
CFLAGS=-std=c2x -O2 -ffast-math -flto -pedantic-errors -Wall -Wextra -Wno-unused-parameter $(shell sdl2-config --cflags)
LDFLAGS=-s -fuse-ld=lld
#LDFLAGS=-s
LIBS=-lm $(shell sdl2-config --libs)

all: $(OUT)

.PHONY: all clean

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

obj/%.o: src/%.c
	@mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj $(OUT)
