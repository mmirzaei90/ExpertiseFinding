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

void convertTxtToTrecTextQP(string dirPath,string trecdirpath, string filepath, string qorR,string indexname, string year,string basePath)
{
    string filedirpath = filepath+"/"+year+"/"+qorR+"_BuildFile.txt";
    ofstream fp(filedirpath.c_str());
    for (const auto & entry : fs::directory_iterator(dirPath+"/"+year))
    {
       ifstream fin(entry.path());
       string line;
       string content="";
	while(getline(fin, line))
	{
		content += line+"\n";
	}
	fin.close();
	size_t lastindex = entry.path().filename().string().find_last_of(".");
        string rawname = entry.path().filename().string().substr(0, lastindex); 
	string newpath = trecdirpath+"/"+year+"/"+rawname+".trectext";
	ofstream fout(newpath.c_str());
	fout<<"<DOC>\n<DOCNO>"<<rawname<<"</DOCNO>\n<TEXT>\n"<<content<<"\n</TEXT>\n</DOC>\n"<<endl;
	fout.close();
	
	fp<<newpath<<endl;
	
    }
    fp.close();

    	string buildindexconf = filepath+"/"+year+"/"+qorR+"_BuildIndex.conf";
	ofstream bic(buildindexconf.c_str());
	bic<<"<parameters>\n<index>"<<basePath<<"/ProposalSimilarity/Index/"<<qorR<<"/"<<year<<"/"<<indexname<<"_"<<year<<"_WithoutStopWords</index>\n<indexType>key</indexType>\n<dataFiles>"<<filedirpath<<"</dataFiles>\n<docFormat>web</docFormat>\n<stopwords>"<<basePath<<"/ProposalSimilarity/Index/IndexConf/stoplist.dft</stopwords>\n<memory>2G</memory>\n</parameters>"<<endl;
	bic.close();
}
void convertTxtToTrecTextReviewer(string dirPath,string trecdirpath, string filepath, string qorR,string indexname, string year,string basePath )
{
    string RID_path = basePath+"/ProposalSimilarity/assoc/ReviewerIdAssoc_"+year+".txt";
    ofstream researcher_ID_year(RID_path.c_str());

    string filedirpath = filepath+"/"+year+"/"+qorR+"_BuildFile.txt";
    ofstream fp(filedirpath.c_str());
    int rid = 1;
    for (const auto & entry : fs::directory_iterator(dirPath+"/"+year))
    {
       ifstream fin(entry.path());
       string line;
       string content="";
	//reading first line
	while(getline(fin, line))
	{
		content += line+"\n";
	}
	fin.close();
	size_t lastindex = entry.path().filename().string().find_last_of(".");
        string rawname = entry.path().filename().string().substr(0, lastindex); 
	string newpath = trecdirpath+"/"+year+"/"+to_string(rid)+".trectext";
	ofstream fout(newpath.c_str());
	if (content != "")
		fout<<"<DOC>\n<DOCNO>"<<rid<<"</DOCNO>\n<TEXT>\n"<<content<<"\n</TEXT>\n</DOC>\n"<<endl;

	researcher_ID_year<<rid<<"\t"<<"\""<<rawname<<"\""<<endl;

	fout.close();
	
	fp<<newpath<<endl;
        rid++;
	
    }
    fp.close();
    researcher_ID_year.close();

    	string buildindexconf = filepath+"/"+year+"/"+qorR+"_BuildIndex.conf";
	ofstream bic(buildindexconf.c_str());
	bic<<"<parameters>\n<index>"<<basePath<<"/ProposalSimilarity/Index/"<<qorR<<"/"<<year<<"/"<<indexname<<"_"<<year<<"_WithoutStopWords</index>\n<indexType>key</indexType>\n<dataFiles>"<<filedirpath<<"</dataFiles>\n<docFormat>web</docFormat>\n<stopwords>"<<basePath<<"/ProposalSimilarity/Index/IndexConf/stoplist.dft</stopwords>\n<memory>2G</memory>\n</parameters>"<<endl;
	bic.close();
}
void readCSVFile(string path,string year, string qpaper, string Rpaper, string Reviewers,string basePath)
{
	string origPath = path + "/award_"+year+".csv";
	ifstream fout(origPath.c_str());
	cout<<origPath<<endl;
	map<string, list<string>> R_P;	
	list<string> researcherList;

	string line;
	//reading first line
	getline(fout, line);
	while(getline(fout, line))
	{
		//cout<<line<<endl;
		vector<string> cells;
		ofstream qin;
		ofstream rin;
		fstream researcher;
		split(cells,line, is_any_of("\t"));
		//cout<<cells.size()<<endl;
		string awardID = "";
		for(int i=0; i<cells.size(); i++)
		{
			erase_all(cells[i], "\"");
			//cout<<cells[i]<<endl;
			if(i==0)
			{
				if(isdigit(cells[i][0]))
				{
					string nPath = qpaper+"/"+year+"/"+cells[i]+".txt";
					string pPath = Rpaper+"/"+year+"/"+cells[i]+".txt";
					cout<<nPath<<" "<<pPath<<endl;
					awardID = cells[i];
					rin.open(pPath.c_str());
					qin.open(nPath.c_str());	
				}
				else
					break;
			}
			else
			{
				if(cells[i] != "")
				{
					if (i==1)
					{
						researcherList.push_back(cells[i]);
						if(awardID != "")
						{
							map<string, list<string>>::iterator it = R_P.find(cells[i]);
							if(it!= R_P.end())
								R_P[cells[i]].push_back(awardID);
							else
							{
								list<string> temp;
								temp.push_back(awardID);
								R_P[cells[i]] = temp;
							}
						}
			   			string rpath = Reviewers+"/"+year+"/"+cells[i]+".txt";
						cout<<rpath<<endl;
			  			researcher.open (rpath.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
					}
					else
					{
						cout<<cells[i]<<endl;
						qin<<cells[i]<<endl;
						rin<<cells[i]<<endl;
						researcher<<cells[i]<<endl;
						
					}
				}
			}
		}
		researcher.close();
		qin.close();
		rin.close();
		 
	}
	fout.close();


	researcherList.unique();
	int y = atoi(year.c_str()) - 1;
	while (y > 2005)
	{
		string origPath2 = path + "/award_"+to_string(y)+".csv";
		cout<<origPath2<<endl;
		ifstream fout2(origPath2.c_str());

		string line2;
		//reading first line
		getline(fout2, line2);
		while(getline(fout2, line2))
		{
			vector<string> cells;
			string awardId;
			ofstream rin;
			fstream researcher;
			split(cells,line2, is_any_of("\t"));
			for(int i=0; i<cells.size(); i++)
			{
				erase_all(cells[i], "\"");
				if(i==0)
				{
					if(isdigit(cells[i][0]))
					{
						awardId = cells[i];	
					}
					else
						break;
				}
				else
				{
					if(cells[i] != "")
					{
						if (i==1)
						{
						   string rpath = Reviewers+"/"+year+"/"+cells[i]+".txt";
					           string pPath = Rpaper+"/"+year+"/"+awardId+".txt";
						   rin.open(pPath.c_str());
						   researcher.open (rpath.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);

						   std::list<string>::iterator findIter = std::find(researcherList.begin(), researcherList.end(), cells[i]);
						   if( findIter != researcherList.end())
						   {
						           R_P[cells[i]].push_back(awardId);
							
						   }
						   else
						   {
							researcherList.push_back(cells[i]);
							list<string> temp;
							temp.push_back(awardId);
							R_P[cells[i]] = temp;

						   }
						}
						else
						{
							cout<<"in writing previous file"<<endl;
							rin<<cells[i]<<endl;
							researcher<<cells[i]<<endl;
						}
					}
				}
			}
			researcher.close();
			rin.close();
		}
		fout2.close();
		y--;
	}

	researcherList.unique();

	string R_P_path = basePath+"/ProposalSimilarity/assoc/assoc_R_P_"+year+".csv";
	

	ofstream R_P_year(R_P_path.c_str());
	

	for(map<string,list<string>>::iterator rit = R_P.begin(); rit != R_P.end(); rit++)
	{
		for(list<string>::iterator pit = rit->second.begin(); pit != rit->second.end(); pit++)
		{
			R_P_year<<"\""<<rit->first<<"\""<<"\t"<<*pit<<endl;
		}
	}
	R_P_year.close();

	
}

int main(int argc, char* argv[])
{
	string s(argv[2]);
	string awardPath = s+"/ProposalSimilarity/awards";
	string qpaperPath =s + "/ProposalSimilarity/Qpapers";
	string rpaperPath =s + "/ProposalSimilarity/Rpapers";
	string reviewerPath =s + "/ProposalSimilarity/Reviewers";
	string qpapertrecPath =s + "/ProposalSimilarity/Qpapers_trectext";
	string rpapertrecPath =s + "/ProposalSimilarity/Rpapers_trectext";
	string reviewertrecPath =s + "/ProposalSimilarity/Reviewers_trectext";
	string qindex =s + "/ProposalSimilarity/Index/IndexConf/Qpapers";
	string rindex =s + "/ProposalSimilarity/Index/IndexConf/Rpapers";
	string reviewerindex =s + "/ProposalSimilarity/Index/IndexConf/Reviewers";
	
	readCSVFile(awardPath,argv[1], qpaperPath,rpaperPath,reviewerPath,argv[2]);
	convertTxtToTrecTextQP(qpaperPath,qpapertrecPath,qindex,"Qpapers","qpIndex",argv[1],argv[2]);
	convertTxtToTrecTextQP(rpaperPath,rpapertrecPath,rindex,"Rpapers","rpIndex",argv[1],argv[2]);
	convertTxtToTrecTextReviewer(reviewerPath,reviewertrecPath,reviewerindex,"Reviewers","rIndex",argv[1],argv[2]);
}
