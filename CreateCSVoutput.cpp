#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <cmath>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <boost/algorithm/string.hpp>
#include <experimental/filesystem>

using namespace boost::algorithm;
using namespace std;
namespace fs = std::experimental::filesystem;
void readAssocFile(string rin,map<int , string> & ridAssoc)
{
	ifstream fout2(rin.c_str());
	string line2;
	while(getline(fout2, line2))
	{
		vector<string> cells;
		split(cells,line2, is_any_of("\t"));
		int rid = atoi(cells[0].c_str());
		string name = cells[1];
		ridAssoc[rid] = name;
	}
	fout2.close();
}
void convertOutputToCSV(string output,map<int , string> ridAssoc, string newoutput)
{
       ifstream fin(output.c_str());
       string line;
	map<int , list<int> > P_R;
	while(getline(fin, line))
	{
		istringstream iss(line);
		int pid,rid,rank;
		double score;
		string q0,EXP;
		iss >>pid>>q0>>rid>>rank>>score>>EXP;
		P_R[pid].push_back(rid);
	}
	fin.close();

	ofstream out(newoutput.c_str());

	for( map<int , list<int> >::iterator pit = P_R.begin(); pit != P_R.end(); pit++)
	{
		out<<pit->first<<"\t";
		int count = 0;
		for(list<int>::iterator rit = pit->second.begin(); rit != pit->second.end(); rit++)
		{
			if(count != pit->second.size()-1)
				out<<ridAssoc[*rit]<<"|";
			else
				out<<ridAssoc[*rit]<<endl;
			count++;	
		}
	}
	out.close();
}

int main(int argc, char* argv[])
{
	map<int , string> ridAssoc;
	readAssocFile(argv[1], ridAssoc);
	convertOutputToCSV(argv[2], ridAssoc, argv[3]);
}
