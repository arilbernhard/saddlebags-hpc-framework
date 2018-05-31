#include <iostream>
#include <fstream>
#include <string>

#include "../include/saddlebags.hpp"

#include <Eigen/Dense>


/*  This program requires the C++ math library Eigen.

    Eigen matrices are converted to std vectors, since serialization of other types is not yet implemented

    This is a temporary workaround: Serialization is in the UPC++ specification, and listed as an upcoming feature
    see https://groups.google.com/forum/#!searchin/upcxx/2018.1|sort:date/upcxx/TSrsU5y0HBc/T67IeSJXAgAJ
*/

std::vector<int> mat_to_vector(Eigen::MatrixXi mat)
{
    std::vector<int> vec;
    vec.push_back(mat(0,0));
    vec.push_back(mat(0,1));
    vec.push_back(mat(1,0));
    vec.push_back(mat(1,1));
    return vec;
}

Eigen::MatrixXi vector_to_mat(std::vector<int> vec)
{
    Eigen::MatrixXi mat(2,2);
    mat << vec[0], vec[1], vec[2], vec[3];
    return mat;
}


/*  Block matrix class
    Each item represents one block from a matrix. All entries in a table will together resemble an entire matrix*/
template<class Tk, class Ok, class Mt>
class DistribMatBlock : public saddlebags::Item<Tk, Ok, Mt> {
    public:
    
    Eigen::MatrixXi my_mat;
    Eigen::MatrixXi my_sol = Eigen::MatrixXi(2,2);
    Eigen::MatrixXi buffer = Eigen::MatrixXi(2,2);

    int cycle = 0;

    //Set values of the block; initialize multiplication buffer and solutions
    void set_vector(Eigen::MatrixXi v)
    {
        my_mat = v;
        buffer << 1,0,0,1;
        my_sol << 0,0,0,0;
    }

    //For each cycle, move across both matrix dimensions and pull blocks
    //Pull from the horizontal dimension  
    void do_work() override
    {
        this->pull(0, {this->myItemKey.first, cycle});
        this->pull(1, {cycle, this->myItemKey.second});
    }

    Mt foreign_pull(int tag) override
    {
        return mat_to_vector(my_mat);
    }

    //When a pull returns, multiply the buffer with the pulled values
    //This works because pulls are returned in the same order as they were sent
    void returning_pull(saddlebags::Message<Tk, Ok, Mt> returning_message) override
    {
        auto retval = vector_to_mat(returning_message.value);
        buffer = buffer * retval;
    }

    //Update solution at the end of each cycle
    void finishing_work() override {
        cycle++;
        my_sol = my_sol + buffer;
        buffer << 1,0,0,1;
    }
};


int main(int argc, char* argv[]) {


    saddlebags::init();
    if(saddlebags::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;

    using pair = std::pair<int, int>;

    /*  Create a worker and add one table for each matrix
        Tables are indexed with integers, matrix blocks are indexed with integer pairs (x/y coordinates) */
    auto worker = saddlebags::create_worker<int, pair, std::vector<int>>();
    worker->ordered_pulls = true;
    saddlebags::add_table<saddlebags::CyclicDistributor2D, DistribMatBlock>(worker, 0, true);
    saddlebags::add_table<saddlebags::CyclicDistributor2D, DistribMatBlock>(worker, 1, true);

    /* Matrices to use: mat_a will be multiplied by mat_b */
    Eigen::MatrixXi mat_a(4,4);
    mat_a <<    1,  2,  3,  4,
                5,  6,  7,  8,
                9,  10, 11, 12,
                13, 14, 15, 16;

    Eigen::MatrixXi mat_b(4,4);
    mat_b <<    21, 22, 23, 24,
                25, 26, 27, 28,
                29, 30, 31, 32,
                33, 34, 35, 36;

    /* Matrices are 4x4, blocks are 2x2, there are 2 blocks in each dimension of the matrix*/
    const int mat_dim = 4;
    const int blk_dim = 2;
    int blocks = mat_dim / blk_dim;    

    /* Insert matrix blocks into tables. Eigen matrices' block method is used to devise blocks as sub-matrices from a matrix*/
    for(int i = 0; i < blocks; i++)
    {
        for(int j = 0; j < blocks; j++)
        {
            if(worker->get_partition(0, pair(i, j)) == saddlebags::rank_me()) {
                saddlebags::insert_and_return<DistribMatBlock>(worker, 0, pair(i, j))->set_vector(mat_a.block<blk_dim,blk_dim>(blk_dim*i, blk_dim*j));
                saddlebags::insert_and_return<DistribMatBlock>(worker, 1, pair(i, j))->set_vector(mat_b.block<blk_dim,blk_dim>(blk_dim*i, blk_dim*j));
            }
        }
    }

    /* Perform cycles equal to the number of blocks per dimension, so that the entire matrix is traversed in the work method */
    saddlebags::cycles(worker, blocks);


    /* Print the solution */
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            if(worker->get_partition(0, pair(i, j)) == saddlebags::rank_me())
            {
                std::cout << "[" << saddlebags::lookup_item<DistribMatBlock>(worker, 0, pair(i, j))->my_sol << "]" << std::endl;
            }
            upcxx::barrier();
        }
    }

	saddlebags::finalize();
}

