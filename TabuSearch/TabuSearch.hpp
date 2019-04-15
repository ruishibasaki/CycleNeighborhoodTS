//
//  tabusearch.h
//  CycleNeighbor
//
//  Created by Rui Shibasaki on 09/12/16.
//  Copyright (c) 2016 Rui Shibasaki. All rights reserved.
//

#ifndef __CycleNeighbor__tabusearch__
#define __CycleNeighbor__tabusearch__

#include <stdio.h>
#include <stdio.h>
#include <vector>
#include <deque>
#include <iostream>
#include "estruturas.hpp"
#include "Solver.hpp"
#include "NeighborGen.hpp"

struct tabuSearch{
    
    graph sol_t;
    int ternure, nIter;
    double bestUB;
	double UB;
	CG solver;
	int maxiter;
	double intensegap;
	std::vector<int>tern_mark;
	
	neighborGen N;
	
    int solve();
    tabuSearch(){}
    void evaluate();
    void initialize(std::string instance);
    void readInstance(std::string instance);
	bool intensification();
    int RestoreIfInfeas();
};

#endif /* defined(__CycleNeighbor__tabusearch__) */
