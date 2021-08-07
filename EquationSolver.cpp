#include "EquationSolver.h"

EquationSolver::EquationSolver(const char *appid) {
	this->searcher = new WAEngine(std::string(appid));
}

EquationSolver::~EquationSolver() {
	delete this->searcher;
}

bool EquationSolver::solveEquation(std::string search, std::vector<std::string> *results, std::string *img) {
	this->searcher->query.setInput(search);
	
	std::string contents;
	std::cout << "Searching '" << this->searcher->getURL() << "'..." << std::endl;
	if (!WAEngine::DownloadURL(this->searcher->getURL(), &contents)) {
		std::cerr << "Downloading error!" << std::endl;
		return false;
	}
	
	if (!this->searcher->Parse(contents)) return false;
	
	// plot?
	WAPod *response;
	std::vector<const char *> plot = {"Implicit plot", "Surface plot", "Plot", "Plots", "Visual representation"};
	for (auto searchs : plot) {
		response = this->searcher->getPod(searchs);
		if (response != nullptr) {
			WASubpod *subpod = response->getSubpods()[0];
			if (subpod->hasImage()) *img = std::string( subpod->getImage()->getSrc() );
			return true;
		}
	}
	
	// solution?
	std::vector<const char *> one_solution = {"Solution", "Numerical solution", "Complex solution", "Result", "Real solution", "Decimal approximation", "Decimal form"
												"Solutions", "Numerical solutions", "Complex solutions", "Results", "Real solutions"};
	for (auto searchs : one_solution) {
		response = this->searcher->getPod(searchs);
		if (response != nullptr) {
			for (auto subpod : response->getSubpods()) results->push_back(std::string( subpod->getPlainText() ));
		}
	}
	
	return true;
}