#Example Makefile
# current path
MY_PATH = /home/skaur/software/ANN/src/F_Interface

# Add your ANN PATH here
ANN_PATH = /home/skaur/software/ANN

# C++ path here
CC = g++

# HDF5 fortran compiler here
FF = /nfs/gce/projects/FLASH5/software/hdf5/install/bin/h5pfc

# Name of the executable
EXE = ann_test

default:
	$(CC) -c ANN_C_interface.cpp -I$(ANN_PATH)/include
	$(FF) -c ANN_types_mod.F90
	$(FF) -c ANN_mod.F90
	mv *.mod $(ANN_PATH)/include/
clean:
	rm -rf *.o
	rm -rf *.mod
