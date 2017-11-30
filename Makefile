OBJS = main.o api.o
LDFLAGS = -Wall -Wextra
main: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lm