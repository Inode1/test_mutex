CC=g++
CFLAGS=-I.
OBJ = test_mutex2.o
%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test_mutex2: $(OBJ)
	g++ -lrt -lpthread -o $@ $^ $(CFLAGS)
