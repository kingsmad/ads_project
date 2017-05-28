OBJS = ksheap.o
ENCODER_OBJS = $(OBJS) encoder.o
DECODER_OBJS = $(OBJS) decoder.o
BM_OBJS = $(OBJS) heap_benchmark.o
CC = g++ 
DEBUG = -g
CFLAGS = -w -c $(DEBUG) -std=c++11 -O3
LFLAGS = -w $(DEBUG)

run: encoder decoder benchmark

encoder : $(ENCODER_OBJS)
	$(CC) $(LFLAGS) $(ENCODER_OBJS) -o encoder

decoder : $(DECODER_OBJS)
	$(CC) $(LFLAGS) $(DECODER_OBJS) -o decoder

benchmark : $(BM_OBJS)
	$(CC) $(LFLAGS) $(BM_OBJS) -o benchmark

ksheap.o: ksheap.h ksheap.cpp
	$(CC) $(CFLAGS) -c ksheap.cpp

encoder.o: ksheap.h encoder.cpp
	$(CC) $(CFLAGS) -c encoder.cpp

decoder.o: ksheap.h decoder.cpp
	$(CC) $(CFLAGS) -c decoder.cpp

heap_benchmark.o: ksheap.h heap_benchmark.cpp
	$(CC) $(CFLAGS) -c heap_benchmark.cpp

clean:
	-@rm *.o decoder encoder benchmark 2>/dev/null || true
tar:
	tar cfv ads_project.tar *.h *.cpp makefile report.pdf
