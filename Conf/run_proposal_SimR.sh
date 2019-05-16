#!/bin/bash 
#Put the path you copy our code folder into it here. 
basePath="/cshome/mmirzaei/Desktop"
#Creating a seprate directory for all awards in each year (Rpapers, Qpapers, Reviewers).
#Creating a build file to index Rpapers, QPapers, Reviewers
cd $basePath"/ProposalSimilarity"
#Cleaning data
for y in 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017; do
rm -r $basePath"/ProposalSimilarity/Clustering/ClusterIndex/"$y"/*"
rm -r $basePath"/ProposalSimilarity/Index/Qpapers/"$y"/*"
rm -r $basePath"/ProposalSimilarity/Index/Rpapers/"$y"/*"
rm -r $basePath"/ProposalSimilarity/Index/Reviewers/"$y"/*"
done
for y in 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017; do
./CreateAppropriatFolders $y $basePath
done
numC="5" #Num of clusters
numLSA="3" #Num of LSA Components
#Building index for Qpapers, Rpapers and Reviewers in each year
for y in 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017; do
BuildConfFile=$basepPath"/ProposalSimilarity/Index/IndexConf/Qpapers/"$y"/Qpapers_BuildIndex.conf"
BuildIndex $BuildConfFile
done
for y in 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017; do
BuildConfFile=$basepPath"/ProposalSimilarity/Index/IndexConf/Rpapers/"$y"/Rpapers_BuildIndex.conf"
BuildIndex $BuildConfFile
done
for y in 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017; do
BuildConfFile=$basepPath"/ProposalSimilarity/Index/IndexConf/Reviewers/"$y"/Reviewers_BuildIndex.conf"
BuildIndex $BuildConfFile
done
#Clustering Rpapers in each year
for y in 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017;do
dir=$basepPath"/ProposalSimilarity/Rpapers/"$y
python3 $basepPath"/ProposalSimilarity/Clustering/code/document_clustering.py" $numC $dir $basepPath"/ProposalSimilarity/Clustering/ClusterOutput/TXT/"$y"/Kmeans_Rpapers_numC_"$numC"_LSA_"$numLSA".csv" --lsa=$numLSA
done
for y in 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017; do
clusterData=$basepPath"/ProposalSimilarity/Clustering/ClusterData/"$y"/clusterData_Kmeans_Rpapers_numC_"$numC"_LSA_"$numLSA".trectext"
clustering=$basepPath"/ProposalSimilarity/Clustering/ClusterOutput/TXT/"$y"/Kmeans_Rpapers_numC_"$numC"_LSA_"$numLSA".csv"
clusterConfPath=$basepPath"/ProposalSimilarity/Clustering/Conf/ClusterIndexConf/"$y
dataPath=$basepPath"/ProposalSimilarity/Clustering/Conf/ClusterIndexConf/"$y"/Clusterfiles.txt"
BuildConfFile=$basepPath"/ProposalSimilarity/Clustering/Conf/ClusterIndexConf/"$y"/BuildIndexCluster.conf"
assocFile=$basepPath"/ProposalSimilarity/assoc/assoc_R_P_"$y".csv"
ridAssocFile=$basepPath"/ProposalSimilarity/assoc/ReviewerIdAssoc"_$y".txt"
allFileIndexPath=$basepPath"/ProposalSimilarity/Clustering/ClusterIndex/"$y"/Kmeans_Rpapers_numC_"$numC"_LSA_"$numLSA".*"
ClusterIndex=$basepPath"/ProposalSimilarity/Clustering/ClusterIndex/"$y"/Kmeans_Rpapers_numC_"$numC"_LSA_"$numLSA".key"
CommunityMethodPath=$basepPath"/ProposalSimilarity"
outputFile=$basepPath"/ProposalSimilarity/Output/P_R_MRPLRA_"$y".txt"
finaloutputfile=$basepPath"/ProposalSimilarity/FinalOutput/P_R_MRPLRA_"$y".csv"
rwIndex=$basepPath"/ProposalSimilarity/Index/Reviewers/"$y"/rIndex_"$y"_WithoutStopWords.key"
qIndex=$basepPath"/ProposalSimilarity/Index/Qpapers/"$y"/qpIndex_"$y"_WithoutStopWords.key"
rpIndex=$basepPath"/ProposalSimilarity/Index/Rpapers/"$y"/rpIndex_"$y"_WithoutStopWords.key"
lambda="0.9"
beta="0.5"
alpha="0.6"
#num of top researchers per proposal
pqout="5"
cd $CommunityMethodPath
./CreateDataFile $rpIndex $clusterData $clustering
cd
cd $clusterConfPath
rm $dataPath
echo $clusterData > $dataPath
rm $BuildConfFile
rm -r $allFileIndexPath
echo "<parameters>\n<index>"$basepPath"/ProposalSimilarity/Clustering/ClusterIndex/"$y"/Kmeans_Rpapers_numC_"$numC"_LSA_"$numLSA"</index>\n<indexType>key</indexType>\n<dataFiles>"$basepPath"/ProposalSimilarity/Clustering/Conf/ClusterIndexConf/"$y"/Clusterfiles.txt</dataFiles>\n<docFormat>trec</docFormat>\n<stopwords>"$basepPath"/ProposalSimilarity/Index/IndexConf/stoplist.dft</stopwords>\n<memory>2G</memory>\n</parameters>" >> $BuildConfFile
BuildIndex $BuildConfFile
cd
cd $CommunityMethodPath
./MRP_LRA $assocFile $ridAssocFile $clustering $rwIndex $qIndex $ClusterIndex $lambda $beta $alpha $outputFile $pqout
./CreateCSVoutput $ridAssocFile $outputFile $finaloutputfile
done



