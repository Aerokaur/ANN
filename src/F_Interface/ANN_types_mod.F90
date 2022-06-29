module ANN_types_mod
    use IEEE_ARITHMETIC, ONLY : IEEE_SELECTED_REAL_KIND
    use iso_c_binding,   ONLY : C_INT, &
                                C_SIZE_T, &
                                C_DOUBLE

    implicit none
    private

    integer, parameter :: i32 = selected_int_kind(9)
    integer, parameter :: i64 = selected_int_kind(18)
    integer, parameter :: sp  = IEEE_SELECTED_REAL_KIND(p=6,  r=37)
    integer, parameter :: dp  = IEEE_SELECTED_REAL_KIND(p=15, r=307)

    !!!!!----- PUBLIC INTERFACE
    integer, parameter, public :: ANN_INT    = C_INT
    integer, parameter, public :: ANN_REAL = C_DOUBLE
    integer, parameter, public :: ANN_SIZE_T = C_SIZE_T

end module ANN_types_mod

