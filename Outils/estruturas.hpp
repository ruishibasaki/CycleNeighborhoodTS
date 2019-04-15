//
//  estruturas.hpp
//  NetworkSimplexMCF
//
//  Created by Rui Shibasaki on 17/06/16.
//  Copyright (c) 2016 Rui Shibasaki. All rights reserved.
//

#ifndef NetworkSimplexMCF_estruturas_hpp
#define NetworkSimplexMCF_estruturas_hpp
#include <vector>

struct arc{
    int i, j;
    
    inline arc(){}
    inline arc(int i, int j){
        this->i = i;
        this->j = j;
    }
    
    inline arc & operator=(arc a){
        
        i= a.i;
        j= a.j;
        
        return *this;
    }
    
};

struct pair{
    int node, a;
    inline pair():node(0),a(-1){}
    inline pair(int n, int aa):node(n),a(aa){}
    inline pair & operator=(pair p){
        
        node= p.node;
        a= p.a;
        
        return *this;
    }
};

struct connexion{
    int nodj, arcid0, idx;
    double cost;
    
    inline connexion(){}
    inline connexion(int n, int a, int id, double c):nodj(n),arcid0(a),idx(id),cost(c){}
    
    inline connexion & operator=(connexion a){
        nodj= a.nodj;
        arcid0= a.arcid0; 
        idx = a.idx;
        cost = a.cost;  
        return *this;
    }
    
};

struct demand{
	int O, D;
	int quantity;
	inline demand & operator=(demand a){      
        O= a.O;
        D= a.D;
        quantity = a.quantity;
        return *this;
    }
};



struct graph{
    
    int nArcs, nNodes, nDemands;
    //-1 if does not exists; 0 added arcs; 1 original arcs
    std::vector<arc> arcs;
    std::vector<demand> d_k;
    std::vector<double> capacities;
    std::vector<double> costs;
    std::vector<double> fixed_cost;
    std::vector<std::vector<double> > x;
    std::vector<int>  y;
    
    graph & operator = (const graph & g){
        this->nArcs = g.nArcs;
        this->nNodes = g.nNodes;
        this->nDemands = g.nDemands;
        this->capacities = g.capacities;
        this->costs = g.costs;
        this->fixed_cost = g.fixed_cost;
        this->y = g.y;
        this->d_k = g.d_k;
        this->arcs = g.arcs;
        for(int i= 0; i<x.size(); ++i){
            this->x[i] = g.x[i];
        }
        return *this;
    }
};

struct residual{
	std::vector<std::vector<connexion> >links;
	std::vector<double>tt_flow;
	std::vector<double>capa;
	int nArcs, nNodes;
};


#endif
