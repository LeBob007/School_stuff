//benchmark.c

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>


clock_t clock1, clock2, clock3;
clock_t end1, end2, end3;
double time_spent1, time_spent2, time_spent3;
pid_t parent, child_1, child_2, child_3;
double smallest_time, longest_time, middle_time;
int nice1, nice2, nice3;
struct rusage time_buff1;
struct rusage time_buff2;
struct rusage time_buff3;



int main(){

	printf("****Executing benchmark****\n");

	//get child_1 process
	int child1 = fork();

	//if child process
	if(child1 == 0){

		clock1 = clock();
		///10 million lines of nothing
		for(int i = 0; i < 100000000; i++){
		}

		//end clock cycle
		end1 = clock();

		// Return resource usage statistics for the calling process, which is the sum of resources used by all threads in the process.
	  getrusage(RUSAGE_SELF, &time_buff1);

		printf("\n---------------------------Child Process 1:---------------------------------\n");

		//run_utime: This is the total amount of time spent executing in user mode, expressed in a timeval structure (seconds plus microseconds).
		//tv_usec: This is the rest of the elapsed time (a fraction of a second), represented as the number of microseconds. It is always less than one million.
		printf("user microseconds for child process of nice value %i: %ld\n", 0, time_buff1.ru_utime.tv_usec);
		printf("total user seconds for child process of nice value %i: %e\n", 0, (double) time_buff1.ru_utime.tv_sec + (double) time_buff1.ru_utime.tv_usec / (double) 1000000);

		//calculate the time in took to processes the 10 million lines,
		//divide by CLOCKS_PER_SEC: to get the number of seconds used by the CPU
		time_spent1 = (double)(end1 - clock1)/CLOCKS_PER_SEC;
		printf("\nTime for child process of nice value %i using clock(): %f \n", 0, time_spent1);

		time_spent1 = time_buff1.ru_utime.tv_usec;

		//--------------------------------------------------------------------------

		//change the priority of process, higher the number = bad for scheduling
		nice(6);

		//fork child_1 to child_2 process
		int child2 = fork();

		if(child2 == 0){

			printf("\n---------------------------Child Process 2:---------------------------------\n");
			printf("Sleep for 10 seconds.\n");
			sleep(10);


			clock2 = clock();
			///10 million lines of nothing
			for(int i = 0; i < 100000000; i++){
			}

			//end clock cycle
			end2 = clock();

			// Return resource usage statistics for the calling process, which is the sum of resources used by all threads in the process.
			getrusage(RUSAGE_SELF, &time_buff2);


			printf("user microseconds for child process of nice value %i: %ld\n", 6, time_buff2.ru_utime.tv_usec);
			printf("total user seconds for child process of nice value %i: %e\n", 6, (double) time_buff2.ru_utime.tv_sec + (double) time_buff2.ru_utime.tv_usec / (double) 1000000);

			//calculate the time in took to processes the 10 million lines
			time_spent2 = (double)(end2 - clock2)/CLOCKS_PER_SEC;
			printf("\nTime for child process of nice value %i using clock(): %f \n", 6, time_spent2);

			time_spent2 = time_buff2.ru_utime.tv_usec;


			//change the priority of process, higher the number = bad for scheduling
			nice(8);

			//fork child_1 to child_2 process
			int child3 = fork();


			if(child3 == 0){
				printf("\n---------------------------Child Process 3:---------------------------------\n");

				clock3 = clock();

				///10 million lines of nothing
				for(int i = 0; i < 100000000; i++){
				}

				//end clock cycle
				end3 = clock();


				// Return resource usage statistics for the calling process, which is the sum of resources used by all threads in the process.
				getrusage(RUSAGE_SELF, &time_buff3);

				printf("user microseconds for child process of nice value %i: %ld\n", 8, time_buff3.ru_utime.tv_usec);
				printf("total user seconds for child process of nice value %i: %e\n", 8, (double) time_buff3.ru_utime.tv_sec + (double) time_buff3.ru_utime.tv_usec / (double) 1000000);

				//calculate the time in took to processes the 10 million lines
				time_spent3 = (double)(end3 - clock3)/CLOCKS_PER_SEC;
				printf("\nTime for child process of nice value %i using clock(): %f \n", 8, time_spent3);

				time_spent3 = time_buff3.ru_utime.tv_usec;

				if(time_spent1 > time_spent2 && time_spent1 > time_spent3){
					longest_time = time_spent1;
					nice1 = 0;
				}
				else if(time_spent1 < time_spent2 && time_spent1 < time_spent3){
					smallest_time = time_spent1;
					nice2 = 0;
				}
				else{
					middle_time = time_spent1;
					nice3 = 0;
				}

				if(time_spent2 > time_spent1 && time_spent2 > time_spent3){
					longest_time = time_spent2;
					nice1 = 6;
				}
				else if(time_spent2 < time_spent1 && time_spent2 < time_spent3){
					smallest_time = time_spent2;
					nice2 = 6;
				}
				else{
					middle_time = time_spent2;
					nice3 = 6;
				}

				if(time_spent3 > time_spent1 && time_spent3 > time_spent2){
					longest_time = time_spent3;
					nice1 = 8;
				}
				else if(time_spent3 < time_spent1 && time_spent3 < time_spent2){
					smallest_time = time_spent3;
					nice2 = 8;
				}
				else{
					middle_time = time_spent3;
					nice3 = 8;
				}


				//total stats
				printf("\n---------------------------Total Stats---------------------------------\n");
				printf("Average time to run 10 mil lines in microseconds is %f \n", (time_spent1 + time_spent2 + time_spent3)/3);
				printf("Slowest child process had nice value: %d with time in microseconds: %d \n", nice1, (int)longest_time);
				printf("Middle time child process had nice value: %d with time in microseconds: %d \n", nice3,  (int)middle_time);
				printf("Quickest child process had nice value: %d with time in microseconds: %d \n", nice2,  (int)smallest_time);
			}
		}
	}

	//exit the program
	exit(0);
}
