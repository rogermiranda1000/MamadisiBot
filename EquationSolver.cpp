#include "EquationSolver.h"

EquationSolver::EquationSolver(const char *appid) {
	this->_searcher = new WAEngine(std::string(appid));
}

EquationSolver::~EquationSolver() {
	delete this->_searcher;
}

bool EquationSolver::solveEquation(std::string search_str, std::vector<std::string> *results, std::string *img) {
	std::string contents;
	std::string url = _searcher->getURL(search_str);
	std::cout << "Searching '" << url << "'..." << std::endl;
	if (!WAEngine::DownloadURL(url, &contents)) {
		std::cerr << "Downloading error!" << std::endl;
		return false;
	}
	
	WAResult search = _searcher->getResult(contents);
	WAPod response;
	std::vector<const char *> plot = {"Implicit plot", "Surface plot", "Plot", "Plots", "Visual representation"};
	for (auto searchs : plot) {
		if (search.getPod(searchs, &response)) {
			WASubpod subpod = response.getSubpods()[0];
			if (subpod.hasImage()) *img = std::string( subpod.getImage()->getSrc() );
			return true;
		}
	}
	
	// solution?
	std::vector<const char *> one_solution = {"Solution", "Numerical solution", "Complex solution", "Result", "Real solution", "Decimal approximation", "Decimal form", "Exact result"
												"Solutions", "Numerical solutions", "Complex solutions", "Results", "Real solutions"};
	for (auto searchs : one_solution) {
		if (search.getPod(searchs, &response)) {
			for (auto subpod : response.getSubpods()) results->push_back(std::string( subpod.getPlainText() ));
		}
	}
	
	return true;
}