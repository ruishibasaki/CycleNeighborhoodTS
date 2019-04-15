//
//  tabusearch.cpp
//  CycleNeighbor
//
//  Created by Rui Shibasaki on 09/12/16.
//  Copyright (c) 2016 Rui Shibasaki. All rights reserved.
//

#include "TabuSearch.hpp"

void tabuSearch::readInstance(std::string instance){

	std::ifstream file;
	file.open(instance.c_str());
	if (!file.is_open()) {
		std::cout<<"Failure to open ufl datafile: %s\n "<<instance;
		abort();
	}
 
	std::string s;
	std::istringstream ss;
   
	getline(file,s); //first line, skip!
	getline(file,s);
	ss.str(s);
	

	// read number of nodes, arcs and commodities
	double cost = 0;
	ss>>sol_t.nNodes;
	ss>>sol_t.nArcs;
	ss>>sol_t.nDemands;
	
	ss.clear();

	sol_t.d_k.resize(sol_t.nDemands);
	sol_t.arcs.resize(sol_t.nArcs);
	sol_t.costs.resize(sol_t.nArcs);
	sol_t.capacities.resize(sol_t.nArcs);
	sol_t.fixed_cost.resize(sol_t.nArcs);
	
	for(int i=0; i<sol_t.nArcs; ++i){
		getline(file,s);
		ss.str(s);
		ss>>sol_t.arcs[i].i;
		ss>>sol_t.arcs[i].j;
		ss>>sol_t.costs[i];
		ss>>sol_t.capacities[i];
		ss>>sol_t.fixed_cost[i];
		ss.clear();
	}
	
	for(int i=0; i<sol_t.nDemands; ++i){
		getline(file,s);
		ss.str(s);
		ss>> sol_t.d_k[i].O >> sol_t.d_k[i].D >> sol_t.d_k[i].quantity;
		ss.clear();
	}
	
	file.close();
	
}

void tabuSearch::evaluate(){
	if(solver.solve())
		UB = solver.getSolution(sol_t.x);
	
	//trim
	for(int e=0;e<sol_t.nArcs;++e){
		sol_t.y[e]=0;
		for(int k=0;k<sol_t.nDemands;++k){
			if(sol_t.x[k][e] > 0){
				sol_t.y[e]=1;
				UB+=sol_t.fixed_cost[e];
				break;
			}
		}
	}

}

void tabuSearch::initialize(std::string instance){

	readInstance(instance);
	maxiter = 100;
	ternure = 5;
	intensegap = 0.15;
	//Add auxiliary arcs for feasibility
	for(int k=0;k<sol_t.nDemands;++k){
		sol_t.arcs.push_back(arc(sol_t.d_k[k].O,sol_t.d_k[k].D));
		sol_t.capacities.push_back(sol_t.d_k[k].quantity);
		sol_t.costs.push_back(1e15);
		sol_t.fixed_cost.push_back(1e15);
		++sol_t.nArcs;
	}
	
	//set trivial design solution
	sol_t.y.resize(sol_t.nArcs,1);
	sol_t.x.resize(sol_t.nDemands);
	for(int k=0;k<sol_t.nDemands;++k){
		sol_t.x[k].resize(sol_t.nArcs,0.0);
	}
	
	//initialize solver 
	solver.initialize(sol_t);
	
	//initialize neighborhood generator
	N.initialize(sol_t);
	tern_mark.resize(sol_t.nArcs,0);
	
	//fisrt feasible solution
	solver.modify(sol_t);
	evaluate();
	bestUB = UB;
	std::cout<<bestUB<<std::endl;
	/*
	std::cout<<bestUB<<std::endl;
		for(int a=0;a<sol_t.nArcs;++a){
		if(sol_t.y[a])
			std::cout<<std::endl<<"open arc "<<a+1<<" ";
		for(int k=0;k<sol_t.nDemands;++k)
			if(sol_t.x[k][a]>0.0)
				std::cout<<" flow "<<k+1<<" = "<<sol_t.x[k][a];
		}
	*/
	int cont=0;
	if(intensification()){
		solver.modify(sol_t);
		evaluate();
		if(bestUB>UB)
			bestUB = UB;
		
		
	}
	
}


bool tabuSearch::intensification(){
	
	bool done = false;
	
	std::deque<pair>path;
	N.compute_residual_capa(sol_t);
	for(int k=0;k<sol_t.nDemands;++k){ //main loop for each commodity
		N.bestgain = 0;
		N.idslctd = 0;
		N.snseslctd = 0;
		N.bestgamma = 0;

		for(int a=0;a<sol_t.nArcs;++a){ //first in-loop for gammas
			if(sol_t.x[k][a]>0.000){		
				//get ggamma = sol_t.x[k][a]
				N.find_overall_cycle(sol_t.x[k][a],sol_t,k+1,path,0);
			}
		}
		if(N.bestgain < 0.000){
				//std::cout<<"gain: "<<bestgain<<" gamma: "<<bestgama<<" arc: "<<sol_t.arcs[indexselectd].i<<"-"<<sol_t.arcs[indexselectd].j<<std::endl;
			done = N.modify_xy(sol_t, k+1);
			UB += N.bestgain;
			/*for(int a=0;a<sol_t.nArcs;++a){
				if(sol_t.y[a])
					std::cout<<std::endl<<"open arc "<<a+1<<" ";
				for(int k=0;k<sol_t.nDemands;++k)
					if(sol_t.x[k][a]>0.0)
						std::cout<<" flow "<<k+1<<" = "<<sol_t.x[k][a];
			}
			double sum=0;
			for(int e=0;e<sol_t.nArcs;++e){
				for(int k=0;k<sol_t.nDemands;++k){
					sum += sol_t.costs[e]*sol_t.x[k][e];
				}
				
				sum += sol_t.fixed_cost[e]*sol_t.y[e];
			}
			std::cout<<sum<<" UB "<<UB<<std::endl;*/
		}
	}
	
	
			
	return done;
}


int tabuSearch::solve(){
	
	std::deque<pair>path;
	int iter =0;
	while(iter<100){
		
		//compute and update tabulist and ternure
		for(int a=0;a<sol_t.nArcs;++a){
			if(N.tabuList[a])
				++tern_mark[a];
			if(tern_mark[a]>=ternure){
				tern_mark[a]=0;
				N.tabuList[a]=false;
			}
		}		
		
		N.bestgain = __DBL_MAX__;
		N.idslctd = 0;
		N.snseslctd = 0;
		N.bestgamma = 0;
		N.compute_residual_capa(sol_t);
		for(int a=0;a<sol_t.nArcs;++a){ //first in-loop for gammas
			if(N.res_graph.tt_flow[a]>0){
				//get gamma = N.res_graph.tt_flow[a]	
				N.find_overall_cycle(N.res_graph.tt_flow[a],sol_t,0,path,0);
			}
		}
		
		if(N.bestgain<__DBL_MAX__){
			std::cout<<"iter "<<iter<<" gain: "<<N.bestgain<<" gamma: "<<N.bestgamma<<" arc: "<<sol_t.arcs[N.idslctd].i<<"-"<<sol_t.arcs[N.idslctd].j<<std::endl;
			bool done = false;
			done = N.modify_xy(sol_t, 0);
			
			if(done){
				solver.modify(sol_t);
				evaluate();
				std::cout<<"best= "<<bestUB<<" current= "<<UB<<std::endl;
				if(RestoreIfInfeas()<0){
					std::cout<<"ERROR : cannot restore infeas"<<std::endl;
					return 0;
				}
				else{
					solver.modify(sol_t);
					evaluate();
					std::cout<<"best= "<<bestUB<<" current= "<<UB<<std::endl;
	
				}
				
				if(bestUB>UB)
					bestUB = UB;
			}
			
		}
		if((bestUB-UB)/bestUB <= intensegap){
			if(intensification()){
				solver.modify(sol_t);
				evaluate();
				if(bestUB>UB)
					bestUB = UB;
			}
		}
		++iter;
	}
	if(intensification()){
		solver.modify(sol_t);
		evaluate();
		if(bestUB>UB)
			bestUB = UB;
		
	}
	std::cout<<"FINAL = "<<bestUB<<std::endl;
}

int tabuSearch::RestoreIfInfeas(){
	
	std::deque<pair>path;
	int done = 0;
	
	int ca = sol_t.nArcs-sol_t.nDemands;
	
	N.compute_residual_capa(sol_t);

	for(int a=ca;a<sol_t.nArcs;++a){
		N.bestgain = __DBL_MAX__;
		N.idslctd = 0;
		N.snseslctd = 0;
		N.bestgamma = 0;
				
		if(N.res_graph.tt_flow[a]>0){
			//get gamma = N.res_graph.tt_flow[a]
			std::cout<<"restore "<<a+1<<" "<<N.res_graph.tt_flow[a]<<std::endl;
			N.infeas = true;	
			N.find_overall_cycle(N.res_graph.tt_flow[a],sol_t,(a-ca+1),path,ca);
			
			if(N.bestgain<__DBL_MAX__){
				//std::cout<<"gain: "<<N.bestgain<<" gamma: "<<N.bestgamma<<" arc: "<<sol_t.arcs[N.idslctd].i<<"-"<<sol_t.arcs[N.idslctd].j<<std::endl;
				if(N.modify_xy(sol_t, (a-ca+1))){
					 N.infeas = false;
					 done = 1;
				}
				else
					 return -1;
			}
			else
				 return -1;		 
		}
	}
	return done;
}











