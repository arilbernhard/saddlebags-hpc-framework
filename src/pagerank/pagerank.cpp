#include <iostream>
#include <fstream>
#include <string>

#include "../include/timer.cpp"
#include "../include/lighght.hpp"



template<class Tk, class Ok, class Mt>
class SiteObject : public lighght::Item<Tk, Ok, Mt> {
    public:

    float page_rank = 1;
    float new_page_rank = 0;
    std::vector<int> links;

    void add_link(int new_link)
    {
        this->links.push_back(new_link);
    }

    void do_work() override {
        for(auto it : this->links)
        {
            this->push(0, it, page_rank/((float)links.size()));
        }
    }

    void foreign_push(Mt val) override {
        new_page_rank += val;
    }



    void finishing_work() override {
        page_rank = new_page_rank;
        new_page_rank = 0;
//        std::cout << this->myItemKey << " has rank " << page_rank << std::endl;
    }

};


int main(int argc, char* argv[]) {

    //Benchmark_Timer read_timer;
    //read_timer = Benchmark_Timer();


    lighght::init();
//    if(lighght::rank_me() == 0)
//        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;

    using table_k = int;
    using object_k = int;
    using msg_type = float;


    auto worker = lighght::create_worker<table_k, object_k, msg_type>(Direct);

    lighght::add_table<SiteObject>(worker, 0, true);

    //std::ifstream infile("example_data/simple_graph.txt");
   // std::ifstream infile("/home/aril/BigDataBench_MPI_V3.1/SearchEngine/MPI_Pagerank/data-PageRank/Google_genGraph_20.txt");
    std::ifstream infile("/home/aril/Documents/Google_genGraph_20.txt");
    //std::ifstream infile("graph.txt");

    std::string src_buffer, dst_buffer;

    while(infile >> src_buffer) {

        infile >> dst_buffer;

        int url = stoi(src_buffer);
        int link = stoi(dst_buffer);


        if(worker->get_partition(0, url) == lighght::rank_me())
        {
            auto new_obj = lighght::insert_and_return<SiteObject>(worker, 0, url);
            //lighght::insert_object(worker, 0, src_buffer);
            new_obj->add_link(link);
        }
    }


    upcxx::barrier();
    //std::cout << "insertion done" << std::endl;

    for(int i = 0; i < 10; i++)
        lighght::cycle(worker, true);



    /*read_timer.stop();


    if(lighght::rank_me() == 0)
        read_timer.print();
*/
    upcxx::barrier();

  /*  std::string test("34267");
    if(worker->get_partition(0, test) == upcxx::rank_me())
    {
        auto it = lighght::get_item<SiteObject, table_k, object_k, msg_type>(worker, 0, test);
        std::cout << "specific page rank = " << it->page_rank << std::endl;
    }
*/

	lighght::finalize();
}

