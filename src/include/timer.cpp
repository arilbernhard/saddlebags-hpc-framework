#include <ctime>
#include <iostream>


class Benchmark_Timer {
	
	private: 
		struct timespec time_start, time_end;
		long long start, end, elapsed;
	public:
		
		Benchmark_Timer() {
			clock_gettime(CLOCK_MONOTONIC, &time_start);   
			start = time_start.tv_sec*1000000000LL + time_start.tv_nsec;			
		}
	
		long long stop() {
			clock_gettime(CLOCK_MONOTONIC, &time_end);    
			end = time_end.tv_sec*1000000000LL + time_end.tv_nsec;
			elapsed = end - start;
			return elapsed;
		}
	
		void print() {
			std::cout << "Elapsed time " << elapsed << " ns = " << elapsed / 1000000 << " ms" << std::endl;
		}
		
		long long elapsed_time() {
			return elapsed / 1000000;
		}
};