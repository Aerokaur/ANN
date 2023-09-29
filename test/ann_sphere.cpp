//-----------------------------------------------------------------------------------------------
// ann_sphere
// This program builds kd and bd tree for sphere case. It compares the time taken 
// for both trees built using different parameters.
// To compile this program: ` sh run_sphere.sh `
//-----------------------------------------------------------------------------------------------
#include <cstdlib>		 // C standard library
#include <cstdio>		 // C I/O (for sscanf)
#include <cstring>		 // string manipulation
#include <fstream>		 // file I/O
#include <ANN/ANN.h>	 // ANN declarations
#include "rand.h"		 // random point generation
#include <ANN/ANNx.h>	 // more ANN declarations
#include <ANN/ANNperf.h> // performance evaluation
#ifndef CLOCKS_PER_SEC	 // define clocks-per-second if needed
#define CLOCKS_PER_SEC 1000000
#endif
using namespace std; // make std:: accessible
//----------------------------------------------------------------------
typedef enum
{
	DATA,
	QUERY
} PtType; // point types

//----------------------------------------------------------------------
//	Parameters that are set in getArgs()
//----------------------------------------------------------------------
void getArgs(int argc, char **argv); // get command-line arguments

int k = 1;					// number of nearest neighbors
int dim = 3;				// dimension
double eps = 0;				// error bound
int maxPts = 10000000;		// maximum number of data points
const int STRING_LEN = 500; // max string length
int query_size;
istream *dataIn = NULL;	 // input for data points
istream *queryIn = NULL; // input for query points
int bucket_size = 1;	 // bucket size
ANNsplitRule split;		 // splitting rule
ANNshrinkRule shrink;	 // shrinking rule

void readPts(		   // read data/query points from file
	ANNpointArray &pa, // point array (returned)
	int &n,			   // number of points
	char *file_nm,	   // file name
	PtType type);	   // point type (DATA, QUERY)

void Error(			 // error routine
	const char *msg, // error message
	ANNerr level)	 // abort afterwards
{
	if (level == ANNabort)
	{
		cerr << "ann_test: ERROR------->" << msg << "<-------------ERROR\n";
		exit(1);
	}
	else
	{
		cerr << "ann_test: WARNING----->" << msg << "<-------------WARNING\n";
	}
}

void readPts(
	ANNpointArray &pa, // point array (returned)
	int &n,			   // number of points
	char *file_nm,	   // file name
	PtType type)	   // point type (DATA, QUERY)
{
	int i;
	//--------------------------------------------------------------------
	//	Open input file and read points
	//--------------------------------------------------------------------
	ifstream in_file(file_nm); // try to open data file
	if (!in_file)
	{
		cerr << "File name: " << file_nm << "\n";
		Error("Cannot open input data/query file", ANNabort);
	}
	// allocate storage for points
	if (pa != NULL)
		annDeallocPts(pa); // get rid of old points
	pa = annAllocPts(n, dim);

	for (i = 0; i < n; i++)
	{ // read the data
		if (!(in_file >> pa[i][0]))
			break;
		for (int d = 1; d < dim; d++)
		{
			in_file >> pa[i][d];
		}
	}

	char ignore_me;		  // character for EOF test
	in_file >> ignore_me; // try to get one more character
	if (!in_file.eof())
	{ // exhausted space before eof
		if (type == DATA)
			Error("`data_size' too small. Input file truncated.", ANNwarn);
		else
			Error("`query_size' too small. Input file truncated.", ANNwarn);
	}
	n = i; // number of points read

	//--------------------------------------------------------------------
	//	Print summary
	//--------------------------------------------------------------------
	// if (stats > SILENT) {
	// 	if (type == DATA) {
	// 		cout << "[Read Data Points:\n";
	// 		cout << "  data_size  = " << n << "\n";
	// 	}
	// 	else {
	// 		cout << "[Read Query Points:\n";
	// 		cout << "  query_size = " << n << "\n";
	// 	}
	// 	cout << "  file_name  = " << file_nm << "\n";
	// 	cout << "  dim        = " << dim << "\n";
	// 											// print if results requested
	// 	if ((type == DATA && stats >= SHOW_PTS) ||
	// 		(type == QUERY && stats >= QUERY_RES)) {
	// 		cout << "  (Points:\n";
	// 		for (i = 0; i < n; i++) {
	// 			cout << "    " << i << "\t";
	// 			printPoint(pa[i], dim);
	// 			cout << "\n";
	// 		}
	// 		cout << "  )\n";
	// 	}
	// 	cout << "]\n";
	// }
}

void generatePts(			  // generate data/query points
	ANNpointArray &pa,		  // point array (returned)
	int n,					  // number of points
	PtType type,			  // point type
	ANNbool new_clust,		  // new cluster centers desired?
	ANNpointArray src = NULL, // source array (for PLANTED)
	int n_src = 0);			  // source size (for PLANTED)

int lookUp(							 // look up name in table
	const char *arg,				 // name to look up
	const char (*table)[STRING_LEN], // name table
	int size)						 // table size
{
	int i;
	for (i = 0; i < size; i++)
	{
		if (!strcmp(arg, table[i]))
			return i;
	}
	return i;
}

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
	int nPts;							// actual number of data points
	ANNpointArray dataPts;				// data points
	ANNpointArray query_pts;			// query points
	ANNpoint queryPt;					// query point
	ANNidxArray nnIdx;					// near neighbor indices
	ANNdistArray dists;					// near neighbor distances
	ANNkd_tree *kdTree;					// search structure
	ANNbd_tree *the_tree;				// kd- or bd-tree search structure
	getArgs(argc, argv);				// read command-line arguments
	const int STRING_LEN = 500;			// max string length
	char arg[STRING_LEN];				// all-purpose argument
	queryPt = annAllocPt(dim);			// allocate query point
	dataPts = annAllocPts(maxPts, dim); // allocate data points
	nnIdx = new ANNidx[k];				// allocate near neigh indices
	dists = new ANNdist[k];				// allocate near neighbor dists
	long clock0;						// clock time
	nPts = 0;							// read data points
	cout << "dim " << dim << endl;
	// cout << "Data Points:\n";
	query_pts = annAllocPts(maxPts, dim);
	while (nPts < maxPts && readPt(*dataIn, dataPts[nPts]))
	{
		// printPt(cout, dataPts[nPts]);
		nPts++;
	}
	nPts = 0;
	// cout << "Query Points:\n";
	while (nPts < maxPts && readPt(*queryIn, query_pts[nPts]))
	{
		// printPt(cout, query_pts[nPts]);
		nPts++;
	}
	cout << "#nearest neighbors " << k << endl;
/// if generating query points
#if 0
    cout << "generating query points " << endl;
	ofstream queryOut("sphere_query_10000.pts");
	new_clust = ANNfalse;
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
#endif

	kdTree = new ANNkd_tree( // build search structure
		dataPts,			 // the data points
		nPts,				 // number of points
		dim);				 // dimension of space
	clock0 = clock();		 // start time
	for (int i = 0; i < query_size; i++)
	{
		// cout << "Query point: "; // echo query point
		// printPt(cout, query_pts[i]);

		kdTree->annkSearch( // search
			query_pts[i],	// query point
			k,				// number of near neighbors
			nnIdx,			// nearest neighbors (returned)
			dists,			// distance (returned)
			eps);			// error bound
							//------------------------------------------------------------

		//	Print summary
		//------------------------------------------------------------
		// cout << "\tNN:\tIndex\tDistance\n";
		// for (int i = 0; i < k; i++)
		// {							   // print summary
		// 	dists[i] = sqrt(dists[i]); // unsquare distance
		// 	cout << "\t" << i << "\t" << nnIdx[i] << "\t" << dists[i] << "\n";
		// }
	}
	long prep_time_kd = clock() - clock0; // end of prep time
	cout << " Time for kd search: "
		 << double(prep_time_kd) / CLOCKS_PER_SEC << " sec\n";
	/// bd tree
	clock0 = clock(); // start time
	for (int i = 0; i < query_size; i++)
	{
		// cout << "Query point: "; // echo query point
		// printPt(cout, query_pts[i]);
		//------------------------------------------------------------
		//	Build the tree
		//------------------------------------------------------------
		the_tree = new ANNbd_tree( // build it
			dataPts,			   // the data points
			nPts,				   // number of points
			dim,				   // dimension of space
			bucket_size,		   // maximum bucket size
			ANN_KD_SUGGEST,		   // splitting rule
			ANN_BD_NONE);		   // shrinking rule

		the_tree->annkSearch(
			query_pts[i], // query point
			k,			  // number of near neighbors
			nnIdx,		  // nearest neighbors (returned)
			dists,		  // distance (returned)
			eps);		  // error bound
						  //------------------------------------------------------------
						  //	Print summary
						  //------------------------------------------------------------
						  // cout << "\tNN:\tIndex\tDistance\n";
						  // for (int i = 0; i < k; i++)
						  // {							   // print summary
						  // 	dists[i] = sqrt(dists[i]); // unsquare distance
						  // 	cout << "\t" << i << "\t" << nnIdx[i] << "\t" << dists[i] << "\n";
						  // }
	}
	long prep_time_bd = clock() - clock0; // end of prep time
	cout << " Time for bd search: "
		 << double(prep_time_bd) / CLOCKS_PER_SEC << " sec\n";
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
				" [-qs query_size] [-qf query]\n\n"
			 << "  where:\n"
			 << "    dim      dimension of the space (default = 2)\n"
			 << "    m        maximum number of data points (default = 1000)\n"
			 << "    k        number of nearest neighbors per query (default 1)\n"
			 << "    eps      the error bound (default = 0.0)\n"
			 << "    data     name of file containing data points\n"
			 << " 	 query_size size of query points\n"
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
		else if (!strcmp(argv[i], "-qs"))
		{								  // -query size
			query_size = atoi(argv[++i]); // size of query
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
