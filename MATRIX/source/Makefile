# Create for g++ in 2023 (initial awas 1996 - so it is 26 years later)
CCX = g++

TARGET = matrixfull

CFLAGS  = -g -Wall

All: $(TARGET)
 
$(TARGET): matrixfull.o matrix_c.o reel_c.o sets_c.o
	$(CCX) $(CFLAGS) -o $(TARGET) matrixfull.o matrix_c.o reel_c.o sets_c.o

%.o: %.c
	$(CCX) $(CFLAGS) -c $< -o $@
 
clean:
	$(RM) $(TARGET)
