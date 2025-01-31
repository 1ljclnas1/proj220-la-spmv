all:CSR2_SPMV CSR_SPMV check CSR2_C CSR2_S CSR2_C_MP UTILS CSR2_S_MP

.PHONY : CSR2_SPMV CSR_SPMV CSR2_C_MP check UTILS 
all:CSR2_SPMV CSR_SPMV check CSR2_C CSR2_S CSR2_C_MP 

.PHONY : CSR2_SPMV CSR_SPMV CSR2_C_MP check 

CC = g++

#g++
CSR_SPMV:
	$(CC) -march=native -fopenmp -o CSR_SPMV CSR_SPMV.cpp
CSR2_C:
	$(CC) -march=native -o CSR2_C CSR2_C.cpp 
CSR2_C_ASM:
	$(CC) -march=native  -o CSR2_C_ASM CSR2_C_ASM.cpp 

CSR2_C_MP:
	$(CC) -march=native -fopenmp -o CSR2_C_MP CSR2_C_MP.cpp 
CSR2_S:
	$(CC) -march=native -g  -o CSR2_S CSR2_S.cpp my256dmul.S my256dadd.S my256dload.S my256dfmadd.S -fopenmp
CSR2_S_MP:
	$(CC) -march=native -g  -o CSR2_S_MP CSR2_S_MP.cpp my256dmul.S my256dadd.S my256dload.S my256dfmadd.S -fopenmp
check:
	$(CC) -march=native -fopenmp  -o check check.cpp
UTILS:
	$(CC) -o gl generate_label.cpp -std=c++17 -lstdc++fs
	$(CC) -o formatmtx formatmtx.cpp  -std=c++17 -lstdc++fs

clean:
	rm CSR_SPMV check CSR2_C CSR2_S CSR2_C_MP CSR2_S_MP

