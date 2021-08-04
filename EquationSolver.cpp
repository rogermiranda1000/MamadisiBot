#include "EquationSolver.h"

void solveEquation() {
	WAEngine search;
	search.query.setInput("text for search");
	search.query.addFormat("html");
	search.query.addFormat("plaintext");

	string queryURL = search.getURL();

	std::cout << queryURL << std::endl;
}