all: matrixproduct

matrixproduct:
	g++ -O2 matrixproduct.cpp -lpapi -o matrixproduct

clean:
	rm matrixproduct logfile.txt