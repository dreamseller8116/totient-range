CC		:= gcc

UTILS	:= utils

ifdef OSX

CFLAGS	+= -O3 -Wall -Wextra -std=c99 -D_GNU_SOURCE -DOSX -g
LDFLAGS	+= -framework OpenCL

else

OPENCL	:= /opt/AMDAPP
CFLAGS	+= -O3 -Wall -Wextra -I$(OPENCL)/include -std=c99 -D_GNU_SOURCE -g -lm
LDFLAGS	+= -L$(OPENCL)/lib/x86_64 -lrt -lOpenCL

endif

SOURCES := $(wildcard $(UTILS)/*.c)
OBJECTS := $(patsubst $(UTILS)/%.c, $(UTILS)/%.o, $(SOURCES))

TARGETS_1D	:= 1d/v1/totient 1d/v2/totient 1d/v3/totient 1d/v4/totient 1d/v5/totient 1d/v6/totient 1d/v7/totient 1d/v8/totient
TARGETS_2D	:= 2d/v1/totient 2d/v2/totient 2d/v3/totient 2d/v4/totient 2d/v5/totient 2d/v6/totient 2d/v7/totient 2d/v8/totient
TARGETS		:= totient $(TARGETS_1D) $(TARGETS_2D)

all: $(TARGETS)

totient: totient.c utils/timer.o
	$(CC) -O3 $^ -o $@

1d/v1/totient: 1d/v1/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

1d/v2/totient: 1d/v2/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

1d/v3/totient: 1d/v3/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

1d/v4/totient: 1d/v4/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

1d/v5/totient: 1d/v5/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

1d/v6/totient: 1d/v6/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

1d/v7/totient: 1d/v7/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

1d/v8/totient: 1d/v8/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

2d/v1/totient: 2d/v1/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

2d/v2/totient: 2d/v2/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

2d/v3/totient: 2d/v3/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

2d/v4/totient: 2d/v4/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

2d/v5/totient: 2d/v5/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

2d/v6/totient: 2d/v6/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

2d/v7/totient: 2d/v7/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

2d/v8/totient: 2d/v8/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(UTILS)/%.o: $(UTILS)/%.c
	$(CC) -I$(SRC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) $(TARGETS) $(OBJECTS)
