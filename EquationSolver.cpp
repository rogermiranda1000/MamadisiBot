#include "EquationSolver.h"

EquationSolver::EquationSolver(const char *appid) {
	this->searcher = new WAEngine(std::string(appid));
}

EquationSolver::~EquationSolver() {
	delete this->searcher;
}

void EquationSolver::solveEquation(std::string search, std::vector<std::string> *results, std::string *img) {
	this->searcher->query.setInput(search);
	
	std::string contents;
	std::cout << "Searching '" << this->searcher->getURL() << "'..." << std::endl;
	if (!WAEngine::DownloadURL(this->searcher->getURL(), &contents)) {
		std::cerr << "Downloading error!" << std::endl;
		return;
	}
	
	this->searcher->Parse(contents);
	
	// plot?
	WAPod *response = this->searcher->getPod("Implicit plot");
	if (response == nullptr) response = this->searcher->getPod("Surface plot");
	if (response == nullptr) response = this->searcher->getPod("Plot");
	if (response == nullptr) response = this->searcher->getPod("Plots");
	if (response != nullptr) {
		WASubpod *subpod = response->getSubpods()[0];
		if (subpod->hasImage()) *img = std::string( subpod->getImage()->getSrc() );
	}
	else {
		// solution?
		std::vector<const char *> one_solution = {"Solution", "Numerical solution", "Complex solution", "Result", "Real solution", "Decimal approximation"
													"Solutions", "Numerical solutions", "Complex solutions", "Results", "Real solutions"};
		for (auto searchs : one_solution) {
			response = this->searcher->getPod(searchs);
			if (response != nullptr) {
				for (auto subpod : response->getSubpods()) results->push_back(std::string( subpod->getPlainText() ));
			}
		}
	}
}