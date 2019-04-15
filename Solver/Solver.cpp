#include "Solver.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#define INF 9999999
using namespace std;



CG::~CG() {

	try {
		cplex->clearModel();
		x.end();
		capacity_const.end();
		fObj.end();
		model.end();
		env.end();

		delete cplex;

	} catch (IloException& e) {
		std::cerr << "ERROR: " << e.getMessage() << std::endl;
	} catch (...) {
		std::cerr << "Error" << std::endl;
	}
}

void CG::initialize(const graph & G){
	model = IloModel(env);
	create_model(G);
	cplex = new IloCplex(model);
	set_parameters();
}



bool CG::solve() {

	IloTimer timer(env);
	timer.restart();
	
	cplex->solve();
	
	
	if(cplex->getStatus() == IloAlgorithm::Infeasible)
		cout << "Model infeasible" << endl;
	else if(cplex->getStatus() == IloAlgorithm::InfeasibleOrUnbounded)
		cout << "Model infeasible or unbounded" << endl;
	//else
		//cout << "Model feasible" << endl;

	//if(cplex->getStatus() == IloAlgorithm::Optimal){
		//cout << "Optimal solution found" << endl;
	//}
	
	return true;
}



void CG::create_model(const graph & G) {
	create_variables(G);
	create_objective_function(G);
	create_constraints(G);
}

void CG::create_variables(const graph & G) {

	x = IloArray <IloNumVarArray> (env, G.nDemands);
	for (int k = 0; k < G.nDemands; ++k) {
		x[k] = IloNumVarArray(env,G.nArcs);
		for(int i=0; i<G.nArcs; ++i){
			x[k][i] = IloNumVar(env);
		}
	}

}



void CG::create_objective_function(const graph & G) {
	// cria funcao objetivo
	IloExpr obj(env);
	for(int i =0; i<G.nArcs; ++i){
		for(int k = 0; k < G.nDemands; k++){
			obj += G.costs[i] * x[k][i] ;
		}
	}
	
	
	//(*obj) += +cx;
	fObj = IloMinimize(env, obj);
	model.add(fObj);
}


void CG::create_constraints(const graph & G) {

	//flot
	for (int k = 0; k < G.nDemands; ++k) {
		for (int i = 1; i <= G.nNodes; i++) {
			IloExpr constraint(env);
			
			for(int e =0; e<G.nArcs; ++e){
				if(i == G.arcs[e].i)
					constraint += x[k][e];
				else if(i == G.arcs[e].j)
					constraint -= x[k][e];
			}

			if(i == G.d_k[k].O)
				constraint -= G.d_k[k].quantity;
			else if( i == G.d_k[k].D)
				constraint += G.d_k[k].quantity;
			
			model.add(constraint == 0);
			constraint.end();
		}
	}
	
	//capacity 1
	capacity_const = IloArray<IloRange>(env,G.nArcs);
	for(int e=0; e<G.nArcs; ++e){		
		IloExpr constraint(env);
		
		for (int k = 0; k < G.nDemands; k++) {
			constraint += x[k][e];
		}
		
		capacity_const[e] =constraint <= 0;
		model.add(capacity_const[e]);
		constraint.end();
	}
	
}

void CG::modify(const graph & G){
	//std::cout<<"modify LP"<<std::endl;
	for(int e=0; e<G.nArcs-G.nDemands; ++e){
		IloNum ub =  G.capacities[e]*G.y[e];
		capacity_const[e].setUB(ub);
		//if(ub!=0)
			//std::cout<<e+1<<" "<<ub<<" "<<capacity_const[e].getUB()<<std::endl;
	}
	for(int e=G.nArcs-G.nDemands; e<G.nArcs; ++e){
		capacity_const[e].setUB(G.capacities[e]);
	}
}


double CG::getSolution(std::vector<std::vector<double> > & _x){
	double value;
	try{
		value = cplex->getValue(fObj);
		for (int k = 0; k < _x.size(); ++k)
			for(int i=0; i<_x[k].size(); ++i){
				_x[k][i] = cplex->getValue(x[k][i]);
				//if(_x[k][i]>0.0)
				//std::cout<<k+1<<" "<<i+1<<" "<<_x[k][i]<<std::endl;
			}
	}
	catch(const IloCplex::Exception & e){
		cout << e.getMessage() << endl;
	}
	return value;
}



void CG::set_parameters() {
	cplex->setParam(IloCplex::Threads, 0);
	cplex->setParam(IloCplex::RootAlg, 3);
	cplex->setParam(IloCplex::ClockType, 1);
	
	cplex->setOut(env.getNullStream());
	cplex->setParam(IloCplex::TiLim, 3600); // Time limit in seconds

}








