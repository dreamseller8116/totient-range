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

TARGETS	:= totient v1/totient v2/totient v3/totient v4/totient v5/totient v6/totient v7/totient

all: $(TARGETS)

totient: totient.c utils/timer.o
	$(CC) $^ -o $@

v1/totient: v1/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

v2/totient: v2/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

v3/totient: v3/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

v4/totient: v4/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

v5/totient: v5/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

v6/totient: v6/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

v7/totient: v7/totient.c $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(UTILS)/%.o: $(UTILS)/%.c
	$(CC) -I$(SRC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) $(TARGETS) $(OBJECTS)
