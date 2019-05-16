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

using namespace boost::algorithm;
using namespace std;
void readDeptFile(string path,list<string> &Dept)
{
	ifstream fout(path.c_str());
	string line;
	//reading first line
	getline(fout, line);
	int r = 1;
	while(getline(fout, line))
	{
		Dept.push_back(line);
		r++;
	}
	fout.close();
	Dept.unique();
	cout<<r<<endl;
}

int main(int argc, char* argv[])
{
	list<string> DeptList;
	readDeptFile("/cshome/mmirzaei/Desktop/ProposalSimilarity/assoc/Committee.csv", DeptList);
	cout<<DeptList.size()<<endl;
}
