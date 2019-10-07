default:thread_lib

sample_grader:autograder_main.c thread_lib 
	g++ autograder_main.c threads.o -o sample_grader

thread_lib:pthread.cpp
	g++ -c pthread.cpp -o threads.o

clean:
	rm -f sample_grader
	rm -f threads.o
