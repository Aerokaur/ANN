module ANN_mod
   use iso_c_binding
   use ANN_types_mod
   implicit none
   public :: ann_buildTree, ann_kSearch, ann_destroyTree

   interface
      subroutine ann_buildTree_C(C_rows, C_cols, C_dataPts, kdTree) bind(c, name="ann_buildTree_c")
         import
         implicit none
         integer(ANN_INT), intent(in), value :: C_rows, C_cols
         type(c_ptr), value :: C_dataPts
         type(c_ptr) :: kdTree
      end subroutine ann_buildTree_C
   end interface

   interface
      subroutine ann_kSearch_C(queryPt, C_dim, C_k, nnIdx, dists, C_eps, kdTree) bind(c, name="ann_kSearch_c")
         import
         implicit none
         type(c_ptr), value :: queryPt
         integer(ANN_INT), intent(in), value :: C_k, C_dim
         real(ANN_REAL), intent(in), value :: C_eps
         type(c_ptr), value :: kdTree
         type(c_ptr):: nnIdx
         type(c_ptr):: dists
      end subroutine ann_kSearch_C
   end interface

   interface
      subroutine ann_destroyTree_C(kdTree) bind(c, name="ann_destroyTree_c")
         import
         implicit none
         type(c_ptr) :: kdTree
      end subroutine ann_destroyTree_C
   end interface

contains
   subroutine ann_buildTree(rows, cols, dataPts, kdTree)
      type(c_ptr), value :: dataPts
      integer(ANN_INT), intent(in) :: rows
      integer(ANN_INT), intent(in) :: cols
      type(c_ptr) :: kdTree
      call ann_buildTree_C(rows, cols, dataPts, kdTree)
   end subroutine ann_buildTree

   subroutine ann_kSearch(queryPt, dim, k, nnIdx, dists, eps, kdTree)
      type(c_ptr), value :: queryPt
      integer(ANN_INT), intent(in) :: k, dim
      real(ANN_REAL), intent(in) :: eps
      type(c_ptr), value :: kdTree
      type(c_ptr) :: nnIdx
      type(c_ptr) :: dists
      call ann_kSearch_C(queryPt, dim, k, nnIdx, dists, eps, kdTree)
   end subroutine ann_kSearch

   subroutine ann_destroyTree(kdTree)
      type(c_ptr) :: kdTree
      !!call ann_destroyTree_C(kdTree)
      kdTree = c_null_ptr
   end subroutine ann_destroyTree

end module ANN_mod
