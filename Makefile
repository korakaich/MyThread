#Group info:
#rkulkar2 Rishkul Kulkarni
#kaich Korak Aich
#ssharma8 Siddharth Sharma

a3:
	gcc -pthread -lrt -lm log_stats.c -o log_stats
a4:
	nvcc p4.cu -arch=sm_13 -o log_stats
a5:
	gcc futex.c mythread.c p5.c -o p5
clean:
	rm -rf p5 log_stats *.o 
