//
//  cycleNeighborGen.h
//  CycleNeighbor
//
//  Created by Rui Shibasaki on 09/12/16.
//  Copyright (c) 2016 Rui Shibasaki. All rights reserved.
//

#ifndef __CycleNeighbor__cycleNeighborGen__
#define __CycleNeighbor__cycleNeighborGen__

#include <stdio.h>
#include "estruturas.hpp"
#include <cmath>
#include <iostream>
#include <deque>
#include <limits>



class neighborGen{
public:
    
    neighborGen(){};
    ~neighborGen();
    residual res_graph;
    
    int idslctd;
	int snseslctd;
    double bestgamma;
    double bestgain;
    std::deque<pair> bestcycle;
    std::vector<bool> tabuList;
	bool infeas;
    
    void initialize(const graph & sol_t);

	//residual graph operations
    double find_cycle(int id,double gamma, const graph & sol_t, int sense, std::deque<pair>& path);
    void create_residual(double gamma, const graph & sol_t, int xk);
    void clear_residual();
    void compute_residual_capa(const graph & sol_t);
    double compute_residual_cost(int a,double gamma, const graph & sol_t, int sense);
    
    //auxiliary functions
    bool checkIfTabu(arc a,int id, double gamma,const graph & sol_t);
    void find_overall_cycle(double gamma,const graph & sol_t, int xk,  std::deque<pair>&path, int ca);
    int checkIfCandidate(const graph & sol_t,int arcid, double gamma, int xk);
    bool modify_xy(graph & sol_t, int xk);
};



#endif /* defined(__CycleNeighbor__cycleNeighborGen__) */
