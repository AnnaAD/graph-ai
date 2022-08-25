csr-triangle: csr_triangle.c
	gcc-11 -o csr_triangle -fopenmp csr_triangle.c -O3 -ftree-vectorize
triangle: triangle.c
	gcc-11 -o triangle -fopenmp triangle.c -O3 -ftree-vectorize
clean:
	rm graph