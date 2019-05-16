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

using namespace lemur::api;
using namespace boost::algorithm;
using namespace std;

void readAssoc(string in,string rin, map<int , list<int> >& PRassoc, map<int , list<int> >& RPassoc,map<string , int> & ridAssoc)
{
	ifstream fout(in.c_str());
	ifstream fout2(rin.c_str());
	cout<<rin<<endl;
	cout<<in<<endl;
	string line2;
	while(getline(fout2, line2))
	{
		vector<string> cells;
		//cout<<line2<<endl;
		split(cells,line2, is_any_of("\t"));
		int rid = atoi(cells[0].c_str());
		string name = cells[1];
		//cout<<name<<" "<<rid<<endl;
		ridAssoc[name] = rid;
	}
	fout2.close();

	string line;
	while(getline(fout, line))
	{
		vector<string> cells;
		split(cells,line, is_any_of("\t"));
		string name = cells[0];
		//cout<<name<<endl;
		int pid = atoi(cells[1].c_str());
		int rid = ridAssoc[name];
		//cout<<rid<<" "<<pid<<endl;
		PRassoc[pid].push_back(rid);
		RPassoc[rid].push_back(pid);
	}
	fout.close();
}
void readClusters(string out , map<int , list<int> >& RpClusters)
{
	ifstream fout(out.c_str());

	string line;
	while(getline(fout, line))
	{
		istringstream iss(line);
		int did,cid;
		iss >>did>>cid;
		//cout<<did<<" "<<cid<<endl;
		RpClusters[did].push_back(cid);
		
	}
	fout.close();

}
void readRClusters(map<int , list<int> > PRassoc,map<int , list<int> > RpClusters,map<int , list<int> >&RClusters)
{
	for(map<int , list<int> >::iterator pit = RpClusters.begin(); pit != RpClusters.end(); pit++)
	{
		//cout<<"Proposal is : "<<pit->first<<endl;
		map<int , list<int> >::iterator fit = PRassoc.find(pit->first);
		if(fit != PRassoc.end())
		{
			for(list<int>::iterator  rit = fit->second.begin(); rit != fit->second.end(); rit++)
			{
				//cout<<"Reviewer "<<*rit<<endl;
				RClusters[*rit].insert(RClusters[*rit].end(), pit->second.begin(), pit->second.end());
			}
		}
	}
	
}
void communityBasedEF(string reviewerIndex, string QueryIndex, string clusterIndex,map<int , list<int>> docCluster,double lambda, double beta,double alpha, string output, int paperQuote)
{
	cout<<"in COmmunity"<<endl;
	cout<<reviewerIndex<<" "<<QueryIndex<<" "<<clusterIndex<<endl;
	Index * qIndex = IndexManager::openIndex(QueryIndex);
	Index * rwIndex = IndexManager::openIndex(reviewerIndex);
	Index * cIndex = IndexManager::openIndex(clusterIndex);


	ofstream out(output.c_str());

	
	map<int , map<int , double> > termClusterProb;
	map<int , map<int , double> > terrmDocProb;
	map<int , map<int , double> > terrmPaperProb;
	map<int , double> collectionProb;
	map<int , int> clusterLength;
	int qid;
	
	///Set up part
	int termID_papers;
	for(termID_papers = 1; termID_papers <= qIndex->termCountUnique(); termID_papers++)
	{
		DocInfoList* pList = qIndex->docInfoList(termID_papers);
			pList->startIteration();
			
			DocInfo *pEntry;
			while(pList->hasMore())
			{
				pEntry = pList->nextEntry();
				terrmPaperProb[termID_papers][pEntry->docID()] = (double) pEntry->termCount() / qIndex->docLength(pEntry->docID());
			}
			delete pList;
		
		if(cIndex->term(qIndex->term(termID_papers)) > 0)
		{
			DocInfoList* clutList = cIndex->docInfoList(cIndex->term(qIndex->term(termID_papers)));
			clutList->startIteration();
			
			DocInfo *cEntry;
			while(clutList->hasMore())
			{
				cEntry = clutList->nextEntry();
				termClusterProb[termID_papers][atoi((cIndex->document(cEntry->docID())).c_str())] = cEntry->termCount(); 
			}
			delete clutList;
		}
		if(rwIndex->term(qIndex->term(termID_papers)) > 0)
		{
			DocInfoList* rwList = rwIndex->docInfoList(rwIndex->term(qIndex->term(termID_papers)));
			rwList->startIteration();
			
			DocInfo *rEntry;
			while(rwList->hasMore())
			{
				rEntry = rwList->nextEntry();
				terrmDocProb[termID_papers][rEntry->docID()] = (double) rEntry->termCount() / rwIndex->docLength(rEntry->docID());
			}
			delete rwList;

			collectionProb[termID_papers] = (double) rwIndex->termCount(rwIndex->term(qIndex->term(termID_papers))) / rwIndex->termCount();
		}
			
	}
	
	for(qid = 1; qid<= qIndex->docCount(); qid++)
	{
		//cout<<qIndex->document(qid)<<endl;
		//cout<<qIndex->docLength(qid)<<endl;
		int docID;
		lemur::api::IndexedRealVector pqd_all;
		
		for(docID = 1; docID <= rwIndex->docCount(); docID++)
		{
			double pqd = 0.0;
			int nd = rwIndex->docLength(docID);   
			    
			TermInfoList *termList2 = qIndex->termInfoList(qid);
			termList2->startIteration();  
			lemur::api::TermInfo* tEntry2;
			
			while (termList2->hasMore()) 
			{
				//cout<<"in while"<<endl;
				 tEntry2 = termList2->nextEntry();
				// cout<<qIndex->term(tEntry2->termID())<<endl;

				 if(rwIndex->term(qIndex->term(tEntry2->termID())) > 0)
				 {
					//cout<<" in rwIndex"<<endl;	


					const lemur::api::TERMID_T& termID = tEntry2->termID(); 


					const double& ptc2 = collectionProb[termID];//pt_cluster2[termID];
					const double& pt_theta_d = terrmDocProb[termID][docID];

					const double& ptq = (alpha)*(terrmPaperProb[termID][qid]) + (1-alpha)*(collectionProb[termID]);
					double ptd; // p(t|\theta_d) 

					double ptc1 = 0.0;
					double c_l = 0.0;
					for(list<int>::iterator fit = docCluster[atoi((rwIndex->document(docID)).c_str())].begin(); fit != docCluster[atoi((rwIndex->document(docID)).c_str())].end(); fit++)
					{
						ptc1 += termClusterProb[termID][*fit];    //pt_cluster1[termID];
						c_l += cIndex->docLength(cIndex->document(to_string((static_cast<long long>(*fit)))));
							
					}
					//cout<<ptc1<<" , "<<c_l<<" , "<<pt_theta_d<<" "<<endl;	
					ptc1 = (double) ptc1/c_l;
					
					ptd = (lambda) * (pt_theta_d) + (1-lambda)*(beta*ptc1 + (1-beta)*ptc2);
	
					pqd += (double) ptq * log(ptd);
					
				}

		  	}
			//cout<<"after while"<<endl;
			if(pqd != 0)
				pqd_all.PushValue(docID, pqd );   
		}
		if(pqd_all.size() != 0)
		{
			pqd_all.LogToPosterior();
		
		// sort 
			pqd_all.Sort(); 
		} 
		//cout<<"after sort"<<endl;
		int rank = 0;
		for ( int i = 0; i < pqd_all.size(); i++ ) 
		{
			const double& pqd = pqd_all[i].val;
			int resDocID = pqd_all[i].ind;
			if(rank < paperQuote)
			{
					out<<qIndex->document(qid) <<" Q0 "<<rwIndex->document(resDocID)<< " " <<rank+1<<" "<<pqd<<" EXP"<<endl;
					rank++;
			}
		}
	}
	out.close();

	delete qIndex;
	delete rwIndex;
	delete cIndex;
}

int main(int argc, char* argv[])
{

	map<int , list<int> > PRassoc;
	map<int , list<int> > RPassoc;
	map<string , int> ridAssoc;
	 


	map<int , list<int> > RClusters;
	map<int , list<int> > RpClusters;
	
	readAssoc(argv[1],argv[2]/*ridAssoc*/,PRassoc,RPassoc,ridAssoc);
	readClusters(argv[3],RpClusters);
	readRClusters(PRassoc,RpClusters,RClusters);
	cout<<"after read clusters"<<endl;
	cout<<argv[4]<<" "<<argv[5]<<" "<<argv[6]<<" "<<atof(argv[7])<<endl;
	communityBasedEF(argv[4]/*rwindex*/, argv[5] /*qIndex*/, argv[6]/*cIndex*/,RClusters,atof(argv[7])/*lambda*/, atof(argv[8])/*beta*/,atof(argv[9])/*alpha*/, argv[10] /*Output*/, atoi(argv[11]) /*paperQuote*/);
}
	
	
