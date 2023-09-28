g++ -c -I../include -O3 ann_sphere.cpp
g++ ann_sphere.o rand.o -o ann_sphere -L../lib -lANN -lm
