#include <stdlib.h>
#include <stdio.h>
#include "cpucounters.h"

long long num_steps = 90000000;
double step;

using namespace std;

int main(int argc, char **argv) {
  PCM * m = PCM::getInstance();

  if (m->program() != PCM::Success) return 1;

  SystemCounterState before_sstate = getSystemCounterState();

        double x, pi, sum=0.0;
        int i;
	step = 1./(double)num_steps;
        for (int j = 0; j < 3; j++){
        for (i=0; i<num_steps; i++)
        {
                x = (i + .5)*step;
                sum = sum + 4.0/(1.+ x*x);
        }
        pi = sum*step;
        }

  SystemCounterState after_sstate = getSystemCounterState();

  cout << "The value of PI is " << pi << endl << endl;

  cout << "Instructions per clock: " << getIPC(before_sstate,after_sstate) << endl
    << "L3 cache hit ratio: " << getL3CacheHitRatio(before_sstate,after_sstate) << endl
    << "Bytes read: " << getBytesReadFromMC(before_sstate,after_sstate) << endl
    << "Power used: " << getConsumedJoules(before_sstate,after_sstate) <<" joules"<< endl
    << endl;

  PCM::getInstance()->cleanup();

  return 0;
}
