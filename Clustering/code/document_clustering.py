# Author: Peter Prettenhofer <peter.prettenhofer@gmail.com>
#         Lars Buitinck
# License: BSD 3 clause

from __future__ import print_function

from sklearn.decomposition import TruncatedSVD
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.feature_extraction.text import HashingVectorizer
from sklearn.feature_extraction.text import TfidfTransformer
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import Normalizer
from sklearn import metrics

from sklearn.cluster import KMeans, MiniBatchKMeans

import logging
from optparse import OptionParser
import sys
from time import time

import numpy as np

from os import listdir
from os.path import isfile, join, splitext

import pdb



# parse commandline arguments
op = OptionParser()
op.add_option("--lsa",
              dest="n_components", type="int",
              help="Preprocess documents with latent semantic analysis.")
op.add_option("--no-smartInit",
              action="store_true", dest="random", default=False,
              help="Use random initialization.")
op.add_option("--no-idf",
              action="store_false", dest="use_idf", default=True,
              help="Disable Inverse Document Frequency feature weighting.")
op.add_option("--use-hashing",
              action="store_true", default=False,
              help="Use a hashing feature vectorizer")
op.add_option("--n-features", type=int, default=10000,
              help="Maximum number of features (dimensions)"
                   " to extract from text.")
op.add_option("--verbose",
              action="store_true", dest="verbose", default=False,
              help="Print progress reports inside k-means algorithm.")

#print(__doc__)
#op.print_help()

(opts, args) = op.parse_args()
if len(args) != 3:
    op.error("this script takes exactly two argument.")
    sys.exit(1)


###############################################################################

data = []
filenames = [f for f in listdir(args[1]) if isfile(join(args[1], f))]
for fname in filenames:
    with open(join(args[1], fname)) as f:
        data.append(f.read())

true_k = int(args[0])

t0 = time()
if opts.use_hashing:
    if opts.use_idf:
        # Perform an IDF normalization on the output of HashingVectorizer
        hasher = HashingVectorizer(n_features=opts.n_features,
                                   stop_words='english', non_negative=True,
                                   norm=None, binary=False)
        vectorizer = make_pipeline(hasher, TfidfTransformer())
    else:
        vectorizer = HashingVectorizer(n_features=opts.n_features,
                                       stop_words='english',
                                       non_negative=False, norm='l2',
                                       binary=False)
else:
    vectorizer = TfidfVectorizer(max_df=0.5, max_features=opts.n_features,
                                 min_df=2, stop_words='english',
                                 use_idf=opts.use_idf)
X = vectorizer.fit_transform(data)



if opts.n_components:
    t0 = time()

    svd = TruncatedSVD(opts.n_components)
    normalizer = Normalizer(copy=False)
    lsa = make_pipeline(svd, normalizer)

    X = lsa.fit_transform(X)

    explained_variance = svd.explained_variance_ratio_.sum()



###############################################################################
# Do the actual clustering

if opts.random:
    km = KMeans(n_clusters=true_k, init='random', max_iter=100, n_init=1,
                verbose=opts.verbose)
else:
    km = KMeans(n_clusters=true_k, init='k-means++', max_iter=100, n_init=1,
                verbose=opts.verbose)

km.fit(X)

print(metrics.silhouette_score(X, km.labels_, sample_size=1000), "," ,km.inertia_)


if not opts.use_hashing:
#    print("Top terms per cluster:")

    if opts.n_components:
        original_space_centroids = svd.inverse_transform(km.cluster_centers_)
        order_centroids = original_space_centroids.argsort()[:, ::-1]
    else:
        order_centroids = km.cluster_centers_.argsort()[:, ::-1]

    terms = vectorizer.get_feature_names()


out_labels = ""

#for l in km.labels_:
#	out_labels += str(l) + ", "
#with open(args[2], "w") as f:
#	f.write(out_labels[:-2])

for i,l in enumerate(km.labels_):
	out_labels += splitext(filenames[i])[0] + " " + str(l) + "\n"
with open(args[2], "w") as f:
	f.write(out_labels[:-1])
