#include "EquationSolver.h"

void solveEquation() {
	WAEngine search;
	search.query.setInput("x-1=4");
	search.query.addFormat("html");
	search.query.addFormat("plaintext");
	//search.setAppID(); // TODO

	string queryURL = search.getURL();

	char *data;
	search.Parse(data);
	int numPod = search.getCountPods();
	WAPod * pods = search.getPods();

	for (int i = 0; i < numPod; i++)
	{
		cout << "Pod "   << i << endl;
		cout << "Title:" << pods[i].getTitle() << endl;
		cout << "ID:"    << pods[i].getID()    << endl;

		// Get a count of sub-blocks
		int numSubPod = pods[i].getCountSubpods();
		int numStates = pods[i].getCountStates();
		
		WASubpod * subpods = pods[i].getSubpods();
		WAPodState * states = pods[i].getStates();
		
		// Enumerate a subpods
		for (int j = 0; j < numSubPod; j++)
		{
			cout << "\tSubPod "  << j << endl;
			// Get a subpod attributes
			cout << "\t\tTitle:"   << subpods[j].getTitle() << endl;
			// Get a built-in img attributes
			cout << "\tImg" << endl;
			cout << "\t\tTitle:" << subpods[j].getImage()->getTitle() << endl;
			cout << "\t\tSrc:"   << subpods[j].getImage()->getSrc() << endl;

		}
		
		// Enumerate a states
		for (int j = 0; j < numStates; j++)
		{
			cout << "\tStates " << j << endl;
			cout << "\t\tName:" << states[j].getName() << endl;
		}
	}
}