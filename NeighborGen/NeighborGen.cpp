//
//  cycleNeighborGen.cpp
//  CycleNeighbor
//
//  Created by Rui Shibasaki on 09/12/16.
//  Copyright (c) 2016 Rui Shibasaki. All rights reserved.
//

#include "NeighborGen.hpp"

void neighborGen::initialize(const graph & sol_t){
	res_graph.nNodes = sol_t.nNodes;
	res_graph.nArcs = sol_t.nArcs;
	
	res_graph.links.resize(sol_t.nNodes);
	res_graph.capa.resize(sol_t.nArcs);
	res_graph.tt_flow.resize(sol_t.nArcs);
	tabuList.resize(sol_t.nArcs);
	infeas=false;
}


void neighborGen::compute_residual_capa(const graph &sol_t){
    
    for (int a=0; a<sol_t.nArcs; ++a){
	    double sum=0;
	    for (int k=0; k<sol_t.nDemands; ++k) {
	        sum+= sol_t.x[k][a];
	    }
	    res_graph.tt_flow[a] = sum;
	    res_graph.capa[a] = sol_t.capacities[a] - sum;
	}
}

double neighborGen::compute_residual_cost(int a,double gamma, const graph & sol_t , int sense){
    double cost=0;
    int min=0;
	
	//1 positive; 2 negative; 0 not candidate
    if (sense == 1) {
 
        cost = (double) sol_t.costs[a]*gamma;
        if (res_graph.tt_flow[a]> 0) {
            min = 1;
        }else min = 0;
        
        cost += sol_t.fixed_cost[a]*(1-min);
        return cost;
    }
    else{
        
        cost = (double) -gamma*(sol_t.costs[a]);
        
        if (res_graph.tt_flow[a] == gamma) {
            cost -= sol_t.fixed_cost[a];
        }
        return cost;
    }
}

void neighborGen::clear_residual(){
	for(int p=0;p<res_graph.links.size();++p)
		res_graph.links[p].clear();
	
}

neighborGen::~neighborGen(){
	 clear_residual();
	 res_graph.links.clear();
	 res_graph.tt_flow.clear();
	 res_graph.capa.clear();
	 bestcycle.clear();
	 tabuList.clear();
}



void neighborGen::create_residual(double gamma, const graph & sol_t, int xk){
    //std::cout<<"create residual"<<" gamma "<<gamma<<std::endl;
    clear_residual();
	int contarc=0;
    for (int a=0; a<sol_t.nArcs; ++a) {
        if (res_graph.capa[a] >= gamma ) {
            double cp = compute_residual_cost(a, gamma, sol_t, 1);
            ++contarc;
            res_graph.links[sol_t.arcs[a].i-1].push_back(connexion(sol_t.arcs[a].j,a,contarc,cp));
            //std::cout<<sol_t.arcs[a].i<<" "<<sol_t.arcs[a].j<<" "<<cp<<"  index "<<a<<std::endl;
        }
        
        double total_flow;
        if(xk == 0) total_flow = res_graph.tt_flow[a];
        else total_flow = sol_t.x[xk-1][a];
        
        if (total_flow >= gamma) {
            double cn = compute_residual_cost(a, gamma, sol_t, 2);
            ++contarc;
            res_graph.links[sol_t.arcs[a].j-1].push_back(connexion(sol_t.arcs[a].i,a,contarc,cn));
            //std::cout<<sol_t.arcs[a].j<<" "<<sol_t.arcs[a].i<<" "<<cn<<"  index "<<a<<std::endl;
        }
    }
}


bool inPath(const std::vector<pair> & pred, int j ,int s, int i){
    bool in=false;
    int jj = j;
    int ii =i;
    //std::cout<<"check if " <<j<<" in path of "<<s<<" to "<<i<<std::endl;
    while(i != 0) {
        //std::cout<<"pred "<<i<<" is "<<pred[i-1].node<<std::endl;
        if(pred[i-1].node == jj){
			in = true;
			//std::cout<<jj<<" in path "<<s<<" to "<<ii<<std::endl;
            break;
		}
		i = pred[i-1].node;
    }
    
    return in;
}



double neighborGen::find_cycle(int id, double gamma, const graph & sol_t, int sense, std::deque<pair>&path){
    
    
    double gain=0;
    std::vector<double> label(res_graph.nNodes,1e20);
    std::vector<pair> pred(res_graph.nNodes);
    std::vector<int> list;
    std::vector<bool> inlist(res_graph.nNodes, false);
	path.clear();
    
    //stablish O-D like the sense
    //1 positive; 2 negative; 0 not candidate
    int origin, dest;
    if(sense==1){
		 origin = sol_t.arcs[id].j;
		 dest = sol_t.arcs[id].i;
    }else{
		 origin = sol_t.arcs[id].i;
		 dest = sol_t.arcs[id].j;
	 }
   
	//initialize
    label[origin-1]=0;
    pred[origin-1].node=0;
    pred[origin-1].a=-1;
    
    list.push_back(origin);
    inlist[origin-1] = true;
	
	//std::cout<<"find cycle candidate: "<<id+1<<" O: "<<origin<<" D:"<<dest<<std::endl;
	
	//main loop
    while (!list.empty()) {
        int i = list.back();
        list.pop_back();
        inlist[i-1]=false;
        for(int p=0;p<res_graph.links[i-1].size();++p){
            int j= res_graph.links[i-1][p].nodj;
            double cost = res_graph.links[i-1][p].cost;
            //std::cout<<i<<" "<<j<<" "<<cost<<std::endl;
            if(!(i==origin && j==dest))
				if (label[j-1] > label[i-1] + cost)
					if (!inPath(pred, j, origin, i)) {
						label[j-1] =label[i-1] +cost;
						pred[j-1].node = i;
						pred[j-1].a = res_graph.links[i-1][p].arcid0;
						//std::cout<<"pred "<<j<<" "<<i<<" "<<std::endl;
						if(!inlist[j-1]){
							list.push_back(j);
							inlist[j-1] = true;
						}
					}
        }
    }
	
	//path retrival
    gain = label[dest-1] + compute_residual_cost(id, gamma, sol_t, sense);
    int p=dest;
    while (pred[p-1].node != 0) {
        path.push_front(pair(p, pred[p-1].a));
        //check if tabu move -- if infeasible restoration let tabu move
        if(!infeas)
			if(checkIfTabu(arc(pred[p-1].node,p),pred[p-1].a,gamma,sol_t)){return __DBL_MAX__;} 
        p = pred[p-1].node;
    }
    path.push_front(pair(origin, pred[origin-1].a));
    return gain;
    
}

bool neighborGen::checkIfTabu(arc a, int id, double gamma, const graph & sol_t){
	//std::cout<<a.i<<"-"<<a.j<<" asis: "<<sol_t.arcs[id].i<<"-"<<sol_t.arcs[id].j<<std::endl;
	if(id>= sol_t.nArcs-sol_t.nDemands)
		return false;
	if(a.i == sol_t.arcs[id].i)//positive
		if(sol_t.y[id]==0)
			if(tabuList[id])return true;
			else return false;
		else return false;
	else{ //negative
		if(sol_t.y[id]==1 && res_graph.tt_flow[id]==gamma)
			if(tabuList[id])return true;
			else return false;
		else return false;
	}
}


void neighborGen::find_overall_cycle(double gamma,const graph & sol_t, int xk, std::deque<pair>&path, int ca){
	double gain=0;
	create_residual(gamma, sol_t, xk); //build the gamma-residual network
				
	for(;ca<sol_t.nArcs;++ca){ //second in-loop for candidates arcs with that gamma
		int sense = checkIfCandidate(sol_t, ca,gamma, xk); //1 positive; 2 negative; 0 not candidate
		if(sense>0 && !tabuList[ca]){
			gain = find_cycle(ca,gamma, sol_t,sense, path);
			if(gain<bestgain){
				bestcycle=path;
				bestgain = gain;
				idslctd = ca;
				snseslctd = sense;
				bestgamma = gamma;
			}
		}
	}
}


int neighborGen::checkIfCandidate(const graph & sol_t, int a, double gamma, int xk){
	
	if(sol_t.y[a] == 0){
		if(sol_t.capacities[a]>= gamma)
			return 1;
		else return 0;
	}else{
		if(xk > 0){
			if(sol_t.x[xk-1][a]>=gamma)
				return 2;
			else if(res_graph.capa[a]>= gamma)
				return 1;
			else return 0;
		}
		else{ 
			if(res_graph.tt_flow[a]>=gamma)
				return 2;
			else if(res_graph.capa[a]>= gamma)
				return 1;
			else return 0;
		}
	}	
}



bool neighborGen::modify_xy(graph & sol_t, int xk){
	//std::cout<<"push gamma "<<gamma<<" candidate selected: "<<id+1<<std::endl;
    bool ymodified=false;
    //modify candidate arc
    if(xk>0){
		if(snseslctd==1){ //positive
			if(res_graph.tt_flow[idslctd]==0){ sol_t.y[idslctd]=1; ymodified=true;}
			tabuList[idslctd]=true;
			sol_t.x[xk-1][idslctd]+=bestgamma;
			res_graph.tt_flow[idslctd]+=bestgamma;
			res_graph.capa[idslctd]-=bestgamma;
			//std::cout<<idslctd+1<<" "<<sol_t.arcs[idslctd].i<<"-"<<sol_t.arcs[idslctd].j<<" is tabu? "<<tabuList[idslctd]<<" flow = "<<res_graph.tt_flow[idslctd]<<std::endl;
			//std::cout<<id+1<<" "<<sol_t.arcs[id].i<<"-"<<sol_t.arcs[id].j<<" flow= "<<sol_t.x[xk-1][id]<<"/"<<res_graph.tt_flow[id]<<" "<<sol_t.y[id]<<std::endl;
		}
		else{ //if(sense==2) negative
			sol_t.x[xk-1][idslctd]-=bestgamma;
			res_graph.tt_flow[idslctd]-=bestgamma;
			res_graph.capa[idslctd]+=bestgamma;
			if(res_graph.tt_flow[idslctd]==0){ sol_t.y[idslctd]=0;ymodified=true;}
			tabuList[idslctd]=true;
			//std::cout<<idslctd+1<<" "<<sol_t.arcs[idslctd].i<<"-"<<sol_t.arcs[idslctd].j<<" is tabu? "<<tabuList[idslctd]<<" flow = "<<res_graph.tt_flow[idslctd]<<std::endl;
			//std::cout<<id+1<<" "<<sol_t.arcs[id].i<<"-"<<sol_t.arcs[id].j<<" flow= "<<sol_t.x[xk-1][id]<<"/"<<res_graph.tt_flow[id]<<" "<<sol_t.y[id]<<std::endl;
		}
	}
	else{
		if(snseslctd==1){//positive
			 if(res_graph.tt_flow[idslctd]==0){ sol_t.y[idslctd]=1;ymodified=true;}
			 res_graph.tt_flow[idslctd]+=bestgamma;
			 res_graph.capa[idslctd]-=bestgamma;
			 tabuList[idslctd]=true;
			 //std::cout<<idslctd+1<<" "<<sol_t.arcs[idslctd].i<<"-"<<sol_t.arcs[idslctd].j<<" is tabu? "<<tabuList[idslctd]<<" flow = "<<res_graph.tt_flow[idslctd]<<std::endl;
			 //std::cout<<idslctd+1<<" "<<sol_t.arcs[idslctd].i<<"-"<<sol_t.arcs[idslctd].j<<" flow= "<<res_graph.tt_flow[idslctd]<<"/"<<sol_t.capacities[idslctd]<<" "<<sol_t.y[idslctd]<<std::endl;
		}
		else{//if(sense==2) negative
			 res_graph.tt_flow[idslctd]-=bestgamma;
			 res_graph.capa[idslctd]+=bestgamma;
			 if(res_graph.tt_flow[idslctd]==0){ sol_t.y[idslctd]=0;ymodified=true;}
			 tabuList[idslctd]=true;
			 //std::cout<<idslctd+1<<" "<<sol_t.arcs[idslctd].i<<"-"<<sol_t.arcs[idslctd].j<<" is tabu? "<<tabuList[idslctd]<<" flow = "<<res_graph.tt_flow[idslctd]<<std::endl;
			 //std::cout<<idslctd+1<<" "<<sol_t.arcs[idslctd].i<<"-"<<sol_t.arcs[idslctd].j<<" flow= "<<res_graph.tt_flow[idslctd]<<"/"<<sol_t.capacities[idslctd]<<" "<<sol_t.y[idslctd]<<std::endl;
		}
	}
    
    //modify other arcs in the cycle
    for (int p=1; p<bestcycle.size(); ++p) {
        int a = bestcycle[p].a;
        if(xk>0){
			if(bestcycle[p].node == sol_t.arcs[a].j){//positive
				if(res_graph.tt_flow[a]==0){ sol_t.y[a]=1;ymodified=true;tabuList[a]=true;}
				sol_t.x[xk-1][a]+=bestgamma;
				res_graph.tt_flow[a]+=bestgamma;
				res_graph.capa[a]-=bestgamma;
				//std::cout<<a+1<<" "<<sol_t.arcs[a].i<<"-"<<sol_t.arcs[a].j<<" is tabu? "<<tabuList[a]<<" flow = "<<res_graph.tt_flow[a]<<std::endl;
				//std::cout<<a+1<<" "<<sol_t.arcs[a].i<<"-"<<sol_t.arcs[a].j<<" flow= "<<sol_t.x[xk-1][a]<<"/"<<res_graph.tt_flow[a]<<" "<<sol_t.y[a]<<std::endl;
			}
			else{//negative
				sol_t.x[xk-1][a]-=bestgamma;
				res_graph.tt_flow[a]-=bestgamma;
				res_graph.capa[a]+=bestgamma;
				if(res_graph.tt_flow[a]==0){ sol_t.y[a]=0;ymodified=true;tabuList[a]=true;}
				//std::cout<<a+1<<" "<<sol_t.arcs[a].i<<"-"<<sol_t.arcs[a].j<<" is tabu? "<<tabuList[a]<<" flow = "<<res_graph.tt_flow[a]<<std::endl;
				//std::cout<<a+1<<" "<<sol_t.arcs[a].i<<"-"<<sol_t.arcs[a].j<<" flow= "<<sol_t.x[xk-1][a]<<"/"<<res_graph.tt_flow[a]<<" "<<sol_t.y[a]<<std::endl;
			}
        }
        else{//positive
			if(bestcycle[p].node == sol_t.arcs[a].j){
				if(res_graph.tt_flow[a]==0){ sol_t.y[a]=1;ymodified=true;tabuList[a]=true;}
				res_graph.tt_flow[a]+=bestgamma;
				res_graph.capa[a]-=bestgamma;
				//std::cout<<a+1<<" "<<sol_t.arcs[a].i<<"-"<<sol_t.arcs[a].j<<" is tabu? "<<tabuList[a]<<" flow = "<<res_graph.tt_flow[a]<<std::endl;
				//std::cout<<a+1<<" "<<sol_t.arcs[a].i<<"-"<<sol_t.arcs[a].j<<" flow= "<<res_graph.tt_flow[a]<<"/"<<sol_t.capacities[a]<<" "<<sol_t.y[a]<<std::endl;
			}
			else{//negative
				res_graph.tt_flow[a]-=bestgamma;
				res_graph.capa[a]+=bestgamma;
				if(res_graph.tt_flow[a]==0){ sol_t.y[a]=0;ymodified=true;tabuList[a]=true;}
				//std::cout<<a+1<<" "<<sol_t.arcs[a].i<<"-"<<sol_t.arcs[a].j<<" is tabu? "<<tabuList[a]<<" flow = "<<res_graph.tt_flow[a]<<std::endl;
				//std::cout<<a+1<<" "<<sol_t.arcs[a].i<<"-"<<sol_t.arcs[a].j<<" flow= "<<res_graph.tt_flow[a]<<"/"<<sol_t.capacities[a]<<" "<<sol_t.y[a]<<std::endl;
			}
		}
	}
    
    return ymodified;
}














