#include <cstdlib>	 // C standard library
#include <cstdio>	 // C I/O (for sscanf)
#include <cstring>	 // string manipulation
#include <fstream>	 // file I/O
#include <ANN/ANN.h> // ANN declarations
#include "rand.h"	 // random point generation

#ifndef CLOCKS_PER_SEC // define clocks-per-second if needed
#define CLOCKS_PER_SEC 1000000
#endif
using namespace std; // make std:: accessible
//----------------------------------------------------------------------
typedef enum
{
	DATA,
	QUERY
} PtType; // point types

void generatePts(			  // generate data/query points
	ANNpointArray &pa,		  // point array (returned)
	int n,					  // number of points
	PtType type,			  // point type
	ANNbool new_clust,		  // new cluster centers desired?
	ANNpointArray src = NULL, // source array (for PLANTED)
	int n_src = 0);			  // source size (for PLANTED)

//----------------------------------------------------------------------
//	Parameters that are set in getArgs()
//----------------------------------------------------------------------
void getArgs(int argc, char **argv); // get command-line arguments

int k = 1;		   // number of nearest neighbors
int dim = 2;	   // dimension
double eps = 0;	   // error bound
int maxPts = 1000; // maximum number of data points

istream *dataIn = NULL;	 // input for data points
istream *queryIn = NULL; // input for query points
// ostream *queryOut = NULL; // input for query points
bool readPt(istream &in, ANNpoint p) // read point (false on EOF)
{
	for (int i = 0; i < dim; i++)
	{
		if (!(in >> p[i]))
			return false;
	}
	return true;
}

/// @brief  Generate points in a cube of [-2 2]^3
/// @param pa - // point array (returned)
/// @param n -  // number of points to generate
/// @param type - // point type
/// @param new_clust - // new cluster centers desired?
/// @param src - // source array (if distr=PLANTED)
/// @param n_src -  // source size (if distr=PLANTED)
void generatePts(
	ANNpointArray &pa,
	int n,
	PtType type,
	ANNbool new_clust,
	ANNpointArray src,
	int n_src)
{
	if (pa != NULL)
		annDeallocPts(pa);	  // get rid of any old points
	pa = annAllocPts(n, dim); // allocate point storage
	annUniformPts2(pa, n, dim);
}

void printPt(ostream &out, ANNpoint p) // print point
{
	out << "(" << p[0];
	for (int i = 1; i < dim; i++)
	{
		out << ", " << p[i];
	}
	out << ")\n";
}

int main(int argc, char **argv)
{
	int nPts;				 // actual number of data points
	ANNpointArray dataPts;	 // data points
	ANNpointArray query_pts; // query points
	ANNpoint queryPt;		 // query point
	ANNidxArray nnIdx;		 // near neighbor indices
	ANNdistArray dists;		 // near neighbor distances
	ANNkd_tree *kdTree;
	int query_size;	   // number of queries
	ANNbool new_clust; // generate new clusters?
	// search structure

	getArgs(argc, argv); // read command-line arguments
	cout << "generating query points " << endl;
	ofstream queryOut("sphere_query_10000.pts");
	new_clust = ANNfalse;
	query_pts = NULL;
	query_size = 10000;
	generatePts(	// generate query points
		query_pts,	// point array
		query_size, // number of query points
		QUERY,
		new_clust); // query points);
	cout << "query points generated " << endl;
	cout << "Query points: "; // echo query point
	for (int i = 0; i < query_size; i++)
	{
		printPt(cout, query_pts[i]);
		for (int d = 0; d < dim; d++)
		{
			queryOut << "  " << query_pts[i][d];
		}
		queryOut << "\n";
	}
	queryOut.close();
	queryPt = annAllocPt(dim);			// allocate query point
	dataPts = annAllocPts(maxPts, dim); // allocate data points
	nnIdx = new ANNidx[k];				// allocate near neigh indices
	dists = new ANNdist[k];				// allocate near neighbor dists

	nPts = 0; // read data points

	// cout << "Data Points:\n";
	while (nPts < maxPts && readPt(*dataIn, dataPts[nPts]))
	{
		// printPt(cout, dataPts[nPts]);
		nPts++;
	}

	kdTree = new ANNkd_tree( // build search structure
		dataPts,			 // the data points
		nPts,				 // number of points
		dim);				 // dimension of space

	while (readPt(*queryIn, queryPt))
	{  
		
	    // read query points
		// cout << "Query point: "; // echo query point
		// printPt(cout, queryPt);

		kdTree->annkSearch( // search
			queryPt,		// query point
			k,				// number of near neighbors
			nnIdx,			// nearest neighbors (returned)
			dists,			// distance (returned)
			eps);			// error bound

		cout << "\tNN:\tIndex\tDistance\n";
		for (int i = 0; i < k; i++)
		{							   // print summary
			dists[i] = sqrt(dists[i]); // unsquare distance
			cout << "\t" << i << "\t" << nnIdx[i] << "\t" << dists[i] << "\n";
		}
	}
	delete[] nnIdx; // clean things up
	delete[] dists;
	delete kdTree;
	annClose(); // done with ANN

	return EXIT_SUCCESS;
}

//----------------------------------------------------------------------
//	getArgs - get command line arguments
//----------------------------------------------------------------------

void getArgs(int argc, char **argv)
{
	static ifstream dataStream;	 // data file stream
	static ifstream queryStream; // query file stream

	if (argc <= 1)
	{ // no arguments
		cerr << "Usage:\n\n"
			 << "  ann_sample [-d dim] [-max m] [-nn k] [-e eps] [-df data]"
				" [-qf query]\n\n"
			 << "  where:\n"
			 << "    dim      dimension of the space (default = 2)\n"
			 << "    m        maximum number of data points (default = 1000)\n"
			 << "    k        number of nearest neighbors per query (default 1)\n"
			 << "    eps      the error bound (default = 0.0)\n"
			 << "    data     name of file containing data points\n"
			 << "    query    name of file containing query points\n\n"
			 << " Results are sent to the standard output.\n"
			 << "\n"
			 << " To run this demo use:\n"
			 << "    ann_sample -df data.pts -qf query.pts\n";
		exit(0);
	}
	int i = 1;
	while (i < argc)
	{ // read arguments
		if (!strcmp(argv[i], "-d"))
		{						   // -d option
			dim = atoi(argv[++i]); // get dimension to dump
		}
		else if (!strcmp(argv[i], "-max"))
		{							  // -max option
			maxPts = atoi(argv[++i]); // get max number of points
		}
		else if (!strcmp(argv[i], "-nn"))
		{						 // -nn option
			k = atoi(argv[++i]); // get number of near neighbors
		}
		else if (!strcmp(argv[i], "-e"))
		{									// -e option
			sscanf(argv[++i], "%lf", &eps); // get error bound
		}
		else if (!strcmp(argv[i], "-df"))
		{										 // -df option
			dataStream.open(argv[++i], ios::in); // open data file
			if (!dataStream)
			{
				cerr << "Cannot open data file\n";
				exit(1);
			}
			dataIn = &dataStream; // make this the data stream
		}
		else if (!strcmp(argv[i], "-qf"))
		{										  // -qf option
			queryStream.open(argv[++i], ios::in); // open query file
			if (!queryStream)
			{
				cerr << "Cannot open query file\n";
				exit(1);
			}
			queryIn = &queryStream; // make this query stream
		}
		else
		{ // illegal syntax
			cerr << "Unrecognized option.\n";
			exit(1);
		}
		i++;
	}
	if (dataIn == NULL || queryIn == NULL)
	{
		cerr << "-df and -qf options must be specified\n";
		exit(1);
	}
}
