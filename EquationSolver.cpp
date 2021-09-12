#include "EquationSolver.h"

EquationSolver::EquationSolver(const char *appid) {
	this->searcher = new WAEngine(std::string(appid));
}

EquationSolver::~EquationSolver() {
	delete this->searcher;
}

void EquationSolver::solveEquation(std::string searchURL, std::vector<std::string> *results, std::string *img) {
	std::string contents;
	std::string url = this->searcher->getURL(searchURL);
	std::cout << "Searching '" << url << "'..." << std::endl;
	if (!WAEngine::DownloadURL(url, &contents)) {
		std::cerr << "Downloading error!" << std::endl;
		return;
	}
	
	WAResult search = this->searcher->getResult(contents);
	
	// plot?
	WAPod response;
	bool found = search.getPod("Implicit plot", &response);
	if (!found) found = search.getPod("Surface plot", &response);
	if (found) {
		WASubpod subpod = response.getSubpods()[0];
		if (subpod.hasImage()) *img = std::string( subpod.getImage()->getSrc() );
	}
	else {
		// 1 solution
		std::vector<const char *> one_solution = {"Solution", "Numerical solution", "Complex solution", "Result", "Real solution",
													"Solutions", "Numerical solutions", "Complex solutions", "Results", "Real solutions"};
		for (auto searchs : one_solution) {
			if (search.getPod(searchs, &response)) {
				for (auto subpod : response.getSubpods()) results->push_back(std::string( subpod.getPlainText() ));
			}
		}
	}
}