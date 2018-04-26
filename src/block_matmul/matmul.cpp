#include <iostream>
#include <fstream>
#include <string>

#include "../include/timer.cpp"
#include "../include/lighght.hpp"

#include <Eigen/Dense>

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


template<class Tk, class Ok, class Mt>
class DistribMatBlock : public lighght::Item<Tk, Ok, Mt> {
    public:
    
    Eigen::MatrixXi my_mat; 
    Eigen::MatrixXi my_sol = Eigen::MatrixXi(2,2);
    Eigen::MatrixXi buffer = Eigen::MatrixXi(2,2);

    int cycle = 0;

    void set_vector(Eigen::MatrixXi v)
    {
        my_mat = v;
        buffer << 1,0,0,1;
        my_sol << 0,0,0,0;
    }

    void do_work() override
    {
        this->pull(0, {this->myItemKey.first, cycle});
        this->pull(1, {cycle, this->myItemKey.second});
    }

    Mt foreign_pull(int tag) override
    {
        return mat_to_vector(my_mat);
    }

    void returning_pull(lighght::Message<Tk, Ok, Mt> returning_message) override
    {
        auto retval = vector_to_mat(returning_message.value);
        buffer = buffer * retval;
    }

    void finishing_work() override {
        cycle+=1;
        my_sol = my_sol + buffer;

        buffer << 1,0,0,1;
    }
};


int main(int argc, char* argv[]) {


    lighght::init();
    if(lighght::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;

    using pair = std::pair<int, int>;

    auto worker = lighght::create_worker<int, pair, std::vector<int>>();

    lighght::add_table<DistribMatBlock>(worker, 0, true);
    lighght::add_table<DistribMatBlock>(worker, 1, true);

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

    const int mat_dim = 4;
    const int blk_dim = 2;
    int blocks = mat_dim / blk_dim;    

    for(int i = 0; i < blocks; i++)
    {
        for(int j = 0; j < blocks; j++)
        {
            if(worker->get_partition(0, pair(i, j)) == lighght::rank_me()) {
                lighght::insert_and_return<DistribMatBlock>(worker, 0, pair(i, j))->set_vector(mat_a.block<blk_dim,blk_dim>(blk_dim*i, blk_dim*j));
                lighght::insert_and_return<DistribMatBlock>(worker, 1, pair(i, j))->set_vector(mat_b.block<blk_dim,blk_dim>(blk_dim*i, blk_dim*j));
            }
        }
    }


    upcxx::barrier();
    for(int i = 0; i < blocks; i++) {
        lighght::cycle(worker, true);
    }

    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            if(worker->get_partition(0, pair(i, j)) == lighght::rank_me())
            {
                std::cout << lighght::lookup_item<DistribMatBlock>(worker, 0, pair(i, j))->my_sol << std::endl;
            }
            upcxx::barrier();
        }
    }

	lighght::finalize();
}

