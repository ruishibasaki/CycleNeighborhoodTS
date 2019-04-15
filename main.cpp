#include <iostream>
#include <string>

#ifndef WIN32
#include <sys/times.h>
#endif

#include "TabuSearch.hpp"
#include "Solver.hpp"


using namespace std;
#define INF 9999999
#define beta 0



int main(int argc, char * argv[]) {

	if(argc != 2){
		cout << "Nombre de parametre non valide\n Ex: ./prog instance_base\n";
		return 0;
	}

 #ifndef WIN32
   // start time measurement
   double t0;
   struct tms timearr; clock_t tres;
   tres = times(&timearr); 
   t0 = timearr.tms_utime;
#endif
  

	
	string instance(argv[1]);
	
	tabuSearch TS;
	TS.initialize(instance);
	TS.solve();

#ifndef WIN32 
   // end time measurement
   clock_t end;
   tres = times(&timearr);
   double t = (timearr.tms_utime - t0)/100.;
   printf(" Total Time: %f secs\n", t);
#endif
	

	return 0;
}
