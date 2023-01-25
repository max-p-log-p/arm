CC=cc
CFLAGS += -Wall -Werror -g -fno-stack-protector -no-pie -fno-pic -I ./include
OBJDUMP = objdump
OBJS = arm_disas.o

.PHONY = all objs clean
all: lab3_fact lab3_fact2 lab3_fact3 lab3_fib

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

lab3_fib: lab3.c arm_callout.S fib.o
	$(CC) $(CFLAGS) -D__FIB__  $^ -static -Wl,-N -o $@
	$(OBJDUMP) -d $@ > $@.dis

lab3_fact: lab3.c arm_callout.S fact.o
	$(CC) $(CFLAGS) -D__FACT__  $^ -static -Wl,-N -o $@
	$(OBJDUMP) -d $@ > $@.dis

lab3_fact2: lab3.c arm_callout.S fact2.o
	$(CC) $(CFLAGS)  -D__FACT2__ $^ -static -Wl,-N -o $@
	$(OBJDUMP) -d $@ > $@.dis

lab3_fact3: lab3.c arm_callout.S fact3.o
	$(CC) $(CFLAGS)  -D__FACT3__ $^ -static -Wl,-N -o $@
	$(OBJDUMP) -d $@ > $@.dis
objs: $(OBJS)

clean:
	rm -f lab3_fib lab3_fact lab3_fact? *.o *.dis 
