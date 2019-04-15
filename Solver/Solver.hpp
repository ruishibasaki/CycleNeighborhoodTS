#ifndef CG_H
#define CG_H

#include <ilcplex/ilocplex.h>
//#include <pair>
#include <vector>
#include <string>
#include "estruturas.hpp"





struct CG {

	// ambiente - gerencia memoria e identificadores para os objetos
	IloEnv env;

	// modelo - representa o modelo a ser otimizado
	IloModel model;
	
	// representa o CPLEX. motor que faz tudo acontecer
	IloCplex * cplex;

	// funcao objetivo
	IloObjective fObj;

	// variaveis
	IloArray <IloNumVarArray> x;
	IloArray<IloRange> capacity_const;

	// construtores
	CG(){};

	virtual ~CG();


	void initialize(const graph & G);
	
	// resolve o problema
	bool solve();
	
	void modify(const graph & G);
	// seta os parametros
	void set_parameters();

	// cria modelo
	void create_model(const graph & G);

	// cria variaveis
	void create_variables(const graph & G);

	// cria funcao objetivo
	void create_objective_function(const graph & G);
	
	//void GenerateCutsI();
	// cria restricoes
	void create_constraints(const graph & G);


	// retorna valor da funcao objetivo
	double getSolution(std::vector<std::vector<double> > & x);
};

#endif // CG_H
