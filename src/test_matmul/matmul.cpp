#include <iostream>
#include <fstream>
#include <string>

#include "../include/timer.cpp"
#include "../include/lighght.hpp"

template<class Tk, class Ok, class Mt>
class MatrixRowObject : public lighght::Item<Tk, Ok, Mt> {
    public:

    std::vector<int> vec;
    std::vector<int> solution;

    void set_vector(std::vector<int> v)
    {
        vec = v;
        if(this->myTableKey != 0)
            return;
        
        for (int i = 0; i < vec.size(); i++)
            solution.push_back(0);
    }

    void do_work() override
    {
        if(this->myTableKey != 0)
            return;

        for(int i = 0; i < vec.size(); i++)
        {
            this->pull(1, i, i);
        }
    }

    Mt foreign_pull(int tag) override {
        return vec;
    }

    void returning_pull(lighght::Message<Tk, Ok, Mt> returning_message) override
    {
        for(int i = 0; i < vec.size(); i++)
            solution.at(returning_message.tag) += vec[i] * returning_message.value[i];
    }
};


int main(int argc, char* argv[]) {


    lighght::init();
    if(lighght::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;

    using table_k = int;
    using object_k = int;
    using msg_type = std::vector<int>;

    auto worker = lighght::create_worker<table_k, object_k, msg_type>(GasnetDirect);

    lighght::add_table<MatrixRowObject, table_k, object_k, msg_type>(worker, 0, true);
    lighght::add_table<MatrixRowObject, table_k, object_k, msg_type>(worker, 1, true);
    /* 
        matrix_a        matrix_b
        [1  2  3 ]     [11  12  13]
        [4  5  6 ]  x  [14  15  16]
        [7  8  9 ]     [17  18  19]
    */

    std::vector<int> matrix_a[3] = {{1,2,3}, {4,5,6}, {7,8,9}};
    std::vector<int> matrix_b[3] = {{11,14,17}, {12,15,18}, {13,16,19}};
    

    for(int i = 0; i < 3; i++)
    {
        lighght::insert_and_return<MatrixRowObject, table_k, object_k, msg_type>(worker, 0, i)->set_vector(matrix_a[i]);
        lighght::insert_and_return<MatrixRowObject, table_k, object_k, msg_type>(worker, 1, i)->set_vector(matrix_b[i]);
    }

    upcxx::barrier();
    lighght::cycle(worker, true);


    int passed = 0;
    int failed = 0;

    std::vector<std::vector<int>> solution = {{90,96,102}, {216,231,246}, {342,366,390}};
    for(int i = 0; i < 3; i++)
    {
        if(worker->get_partition(0, i) == lighght::rank_me())
        {
            for(auto obj_iterator : *(worker->tables[0]->get_objects()))
            {
                if(obj_iterator.first == i)
                {
                    for(int j = 0; j < 3; j++)
                    {
                        int item = reinterpret_cast<MatrixRowObject<table_k, object_k, msg_type>*>(obj_iterator.second)->solution[j];
                        std::cout << item << "  ";
                        if(item == solution[i][j])
                            passed++;
                        else
                            failed++;
                    }
                    std::cout << std::endl;
                }
            }
        }
        upcxx::barrier();
    }

    upcxx::allreduce(passed, std::plus<int>()).wait();
    upcxx::allreduce(failed, std::plus<int>()).wait();

    upcxx::barrier();

    if(upcxx::rank_me() == 0)
    {
        std::cout << std::endl << "Results:" << std::endl << "Failed " << failed << " tests" << std::endl;
        std::cout << "Passed " << passed << " tests" << std::endl;
    }

    //auto new_obj = lighght::insert_and_return<SiteObject, table_k, object_k, msg_type>(worker, 0, url);
    //new_obj->add_link(link);


	lighght::finalize();
}

