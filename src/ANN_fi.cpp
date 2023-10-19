#include <ANN/ANN.h> // ANN declarations
using namespace std;
/** Wrapper functions for ANN interface */
extern "C"
{

  void ann_buildTree_c(int rows, int cols, double *_dataPts, ANNkd_tree *&kdTree)
  {

    ANNpointArray dataPts;             // data points
    dataPts = annAllocPts(rows, cols); // allocate data points
    // C indexing
    for (int i = 0; i < rows; i++)
    {
      for (int j = 0; j < cols; j++)
      {
        dataPts[i][j] = _dataPts[i + j * rows];
      }
    }
    //cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
    //cout << "printing data points in ann_buildTree_c() " << endl;
    int nPts = 0;
    while (nPts < rows)
    {
      //cout << "(" << dataPts[nPts][0];
      for (int j = 1; j < cols; j++)
      {
        //cout << ", " << dataPts[nPts][j];
      }
      //cout << ")\n";
      ++nPts;
    }
    //cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
    // Create a tree object
    kdTree = new ANNkd_tree(dataPts, rows, cols);
  }

  ///\note right now, this function handles only one query point
  void ann_kSearch_c(double *_queryPt, int dim, int k, int *&_nnIdx, double *&_dists, double eps, ANNkd_tree *kdTree)
  {
    ANNpoint queryPt;   // query point
    ANNidxArray nnIdx;  // near neighbor indices
    ANNdistArray dists; // near neighbor distances
    /// allocate dimensions
    queryPt = annAllocPt(dim);
    nnIdx = new ANNidx[k];  // allocate near neigh indices
    dists = new ANNdist[k]; // allocate near neighbor dists
    for (int di = 0; di < dim; ++di)
    {
      queryPt[di] = _queryPt[di];
    }
    kdTree->annkSearch(queryPt, k, nnIdx, dists, eps);
    for (int idx = 0; idx < k; ++idx)
    {
      _dists[idx] = dists[idx];
      _nnIdx[idx] = nnIdx[idx];
    }
    delete nnIdx;
    delete dists;
  }

  // Deallocation of memory 
  void ann_destroyTree_c(ANNkd_tree *kdTree)
  {
    //kdTree->~ANNkd_tree();
    delete kdTree;
    annClose();
  }  
}
