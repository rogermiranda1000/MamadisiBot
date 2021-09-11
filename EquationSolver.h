#pragma once

#include <iostream> // print the error messages
#include "WAEngine.h"

class EquationSolver {
public:
	EquationSolver(const char *appid);
	~EquationSolver();
	
	void solveEquation(std::string searchURL, std::vector<std::string> *results, std::string *img);
	
private:
	WAEngine *searcher;
};