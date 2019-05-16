#!/bin/sh 
for y in 2007 2010 2011 2012 2013 2014 2015 2016 2017;do
numLSA="3"
dir="/cshome/mmirzaei/Desktop/ProposalSimilarity/Rpapers/"$y
numC="5"
python3 /cshome/mmirzaei/Desktop/LSA+Kmeans/document_clustering.py $numC $dir "/cshome/mmirzaei/Desktop/ProposalSimilarity/Clustering/ClusterOutput/TXT/"$y"/Kmeans_Rpapers_numC_"$numC"_LSA_"$numLSA".csv" --lsa=$numLSA
done
