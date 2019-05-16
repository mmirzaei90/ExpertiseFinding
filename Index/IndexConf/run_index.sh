#!/bin/bash
for y in 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017; do
BuildConfFile="/cshome/mmirzaei/Desktop/ProposalSimilarity/Index/IndexConf/Qpapers/"$y"/Qpapers_BuildIndex.conf"
BuildIndex $BuildConfFile
done
for y in 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017; do
BuildConfFile="/cshome/mmirzaei/Desktop/ProposalSimilarity/Index/IndexConf/Rpapers/"$y"/Rpapers_BuildIndex.conf"
BuildIndex $BuildConfFile
done
for y in 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017; do
BuildConfFile="/cshome/mmirzaei/Desktop/ProposalSimilarity/Index/IndexConf/Reviewers/"$y"/Reviewers_BuildIndex.conf"
BuildIndex $BuildConfFile
done



