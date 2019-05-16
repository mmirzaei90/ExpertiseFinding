#include <stdio.h>
#include <stdlib.h>
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

#include "IndexManager.hpp"
#include "BasicDocStream.hpp"
#include "ResultFile.hpp"
#include "RetParamManager.hpp"

using namespace std;
using namespace lemur::api;
using namespace boost::algorithm;

void readPaperClusterFile(string index, string cs_data, string cs_txtFormat)
{
	Index * index_ = IndexManager::openIndex(index);
	

	//cout<<"************"<<cs<<endl;
	ofstream cs_data_out(cs_data.c_str());
	
	
	//map<string , string > docClust;
	map<int , list<int> > clust;

	ifstream fout(cs_txtFormat.c_str());

	string line;
	while(getline(fout, line))
	{
		istringstream iss(line);
		int did,cid;
		iss>>did>>cid;
		cout<<did<<cid<<endl;
		clust[cid].push_back(did);
		
	}
	fout.close();

	for(map<int , list<int> >::iterator it = clust.begin(); it != clust.end(); it++)
	{
		cout<<" Cluster "<<it->first<<" has "<<it->second.size()<<" number of points"<<endl;
		cs_data_out << "<DOC>"<< endl;
		cs_data_out << "<DOCNO>" << it->first << "</DOCNO>" << endl;
		cs_data_out << "<TEXT>" << endl;
		for(list<int>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			
			int docId =index_->document(to_string(static_cast<long long>(*it2)));
 			
			lemur::api::TermInfoList* termList = index_->termInfoListSeq(index_->document(to_string(static_cast<long long>(*it2))));
			termList->startIteration();   
			lemur::api::TermInfo* tEntry;
			while (termList->hasMore()) {
				
				tEntry = termList->nextEntry(); 
				string term = index_->term(tEntry->termID());
				cs_data_out<<term<<" ";
			}
			delete termList;
			cs_data_out << "\n";
		}
		cs_data_out<< "</TEXT>" << endl;
		cs_data_out<<"</DOC>"<<endl; 
		
	}
	cs_data_out.close();

	delete index_;
}

int main(int argc, char* argv[])
{
	readPaperClusterFile(argv[1],argv[2],argv[3]);
}
