all:
	nvcc *.cu *.cpp -lpcap -lcudart -L/usr/local/cuda/lib64 -o $${PWD##*/} --std=c++11
