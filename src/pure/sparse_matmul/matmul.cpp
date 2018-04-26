#include <iostream>
#include <fstream>
#include <string>

#include "../../include/timer.cpp"
#include <upcxx/upcxx.hpp>
#include "../../include/cityhash/src/city.h"

#include <algorithm> // for generate()
#include <cstdlib> // for rand()

size_t NNZPR = 64;  // 512;  // number of non-zero elements per row
size_t M = 2048;  // 1024*1024; // matrix size (M-by-M)

template<typename T>
void print_sparse_matrix(T *A, size_t *col_indices, size_t *row_offsets, size_t nrows)
{
  size_t i, j, k;

  for (i = 0; i < nrows; i++) {
    for (j = 0; j < col_indices[i]; j++) {
      printf("[%lu] %lg\t ", j, A[j]);
    }
    printf("\n");
  }
}

int random_int()
{
  //void srand ( unsigned int seed );
  return rand() % M;
}

double random_double()
{
  long long tmp;

  tmp = rand();
  tmp = (tmp << 32) | rand();
  return (double) tmp;
}

template<typename T>
void spmv(const T * A, const size_t * col_indices, const size_t * row_offsets, size_t nrows, const T * x, T *  y)
{
  for (size_t i = 0; i < nrows; i++) {
    T tmp = 0;
    for (size_t j = row_offsets[i]; j < row_offsets[i + 1]; j++) {
      tmp += A[j] * x[col_indices[j]];
    }
    y[i] = tmp;
  }
}

template<typename T>
void gen_matrix(T *my_A,
                size_t *my_col_indices,
                size_t *my_row_offsets,
                size_t nrows)
{
  int num_nnz = NNZPR * nrows;

  // generate the sparse matrix
  // each place only generates its own portion of the sparse matrix
  my_row_offsets[0] = 0;
  for (size_t row = 0; row < nrows; row++) {
    my_row_offsets[row + 1] = (row + 1) * NNZPR;
  }

  // Generate the data and the column indices
  for (int i = 0; i < num_nnz; i++) {
    my_col_indices[i] = random_int();
    my_A[i] = random_double();
  }
}



int main(int argc, char* argv[]) {
    upcxx::init();

    //Benchmark_Timer read_timer;
    //read_timer = Benchmark_Timer();
    int P = upcxx::rank_n();

    upcxx::global_ptr<double> *A= new upcxx::global_ptr<double> [P];
    assert(A != NULL);
    upcxx::global_ptr<size_t> *col_indices = new upcxx::global_ptr<size_t> [P];
    assert(col_indices != NULL);
    upcxx::global_ptr<size_t> *row_offsets = new upcxx::global_ptr<size_t> [P];
    assert(row_offsets != NULL);
    size_t nrows;
    upcxx::global_ptr<double> *x = new upcxx::global_ptr<double> [P];
    assert(x != NULL);
    upcxx::global_ptr<double> *y = new upcxx::global_ptr<double> [P];
    assert(y != NULL);

    nrows = M / P;  // assume M can be divided by P

    // allocate memory
    for (int i = 0; i < P; i++) {
      A[i] = upcxx::new_array<double>(NNZPR * nrows);
      col_indices[i] = upcxx::new_array<size_t>(NNZPR * nrows);
      row_offsets[i] = upcxx::new_array<size_t>(nrows + 1);
      x[i] = upcxx::new_array<double>(M);
      y[i] = upcxx::new_array<double>(nrows);
    }

    // range all_p(0, ranks());
    for (int i = 0; i < P; i++) {
        if(i == upcxx::rank_me()) {
            gen_matrix<double>(
                A[i].local(),
                col_indices[i].local(),
                row_offsets[i].local(),
                nrows);
        }
    }

    double *my_x = (double *) x[upcxx::rank_me()].local();
    std::generate(my_x, my_x + M, random_double);


    for (int i = 0; i < P; i++) {
        if(i==upcxx::rank_me()) {
            spmv<double>(
                    A[i].local(),
                    col_indices[i].local(),
                    row_offsets[i].local(),
                    nrows,
                    x[i].local(),
                    y[i].local());
        }
    }


    upcxx::barrier();


    printf("spmv_cpu result is:\n");
    for (int i=0; i<P; i++)
    {
        if(upcxx::rank_me() == i)
        {
            printf("cpu place %d: ", i);
            for(int j=0; j<20; j++)
            {
                printf("y[%llu] = %4.1f\n", i*nrows+j, y[i].local()[j]);
            }
        }
        upcxx::barrier();
    }

/*    if(upcxx::rank_me() == 0) {
        double *my_x = (double *) x[0].local();
        std::generate(my_x, my_x + M, random_double);

        for(int i = 1; i < upcxx::rank_n(); i++)
        {
            upcxx::rpc(i, [](upcxx::global_ptr<double> ptr, size_t len) {
                
            }, x[0], M).wait();
            //upcxx::rput(x[0].local(), x[i], M).wait();
        }
    }*/

    upcxx::barrier();


	upcxx::finalize();
}

