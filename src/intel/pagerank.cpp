#include <iostream>
#include <fstream>
#include <string>
#include "cpucounters.h"


#include "../include/timer.cpp"
#include "../include/lighght.hpp"

template<class Tk, class Ok, class Mt>
class SiteObject : public lighght::Item<Tk, Ok, Mt> {
    public:

    float page_rank = 1;
    float new_page_rank = 0;
    int cycle = 1;
    std::vector<int> links;

    void add_link(int new_link)
    {
        this->links.push_back(new_link);
    }

    void do_work() override {
        for(auto it : this->links)
        {
            this->push(0, it, page_rank/((float)links.size()));
            //if(cycle == 2)
            //if(it == 1)
            //    std::cout << lighght::Object<Tk, Ok, Mt>::myObjectKey << " pushes " << page_rank/((float)links.size()) << " to " << it << std::endl;

        }
    }

    void foreign_push(Mt val) override {
        new_page_rank += val;
        //if(lighght::Object<Tk, Ok, Mt>::myObjectKey == 5)
        //    std::cout << lighght::Object<Tk, Ok, Mt>::myObjectKey << " receives " << val << std::endl;
    }

    void finishing_work() override {
        page_rank = new_page_rank;
        new_page_rank = 0;
        //if(cycle == 1)
        //    std::cout << upcxx::rank_me() << " " << lighght::Object<Tk, Ok, Mt>::myObjectKey << " has rank " << page_rank << std::endl;
        cycle++;
    }
};


int main(int argc, char* argv[]) {
    lighght::init();


  PCM * m = PCM::getInstance();
  SystemCounterState before_sstate = getSystemCounterState();



    if(lighght::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;

    using table_k = int;
    using object_k = int;
    using msg_type = float;

    auto worker = lighght::create_worker<table_k, object_k, msg_type>(BufferingWorker);

    lighght::add_table<SiteObject>(worker, 0, true);

    //std::ifstream infile("example_data/simple_graph.txt");
    std::ifstream infile("/home/aril/BigDataBench_MPI_V3.1/SearchEngine/MPI_Pagerank/data-PageRank/Google_genGraph_23.txt");
    //std::ifstream infile("graph.txt");

    std::string src_buffer, dst_buffer;

    while(infile >> src_buffer) {

        infile >> dst_buffer;

        int url = stoi(src_buffer);
        int link = stoi(dst_buffer);


        if(worker->get_partition(0, url) == lighght::rank_me())
        {
            //std::cout << url << " is in partition " << lighght::rank_me() << std::endl;
            //std::cout << "inserting " << src_buffer << std::endl;
            auto new_obj = lighght::insert_and_return<SiteObject>(worker, 0, url);
            new_obj->add_link(link);
        }
    }



    upcxx::barrier();

    for(int i = 1; i < 5; i++)
        lighght::cycle(worker, true);
    




      SystemCounterState after_sstate = getSystemCounterState();

    if(upcxx::rank_me() == 0) {
        std::cout << "Instructions per clock: " << getIPC(before_sstate,after_sstate) << std::endl
        << "L3 cache hit ratio: " << getL3CacheHitRatio(before_sstate,after_sstate) << std::endl
        << "Bytes read: " << getBytesReadFromMC(before_sstate,after_sstate) << std::endl
        << "Power used: " << getConsumedJoules(before_sstate,after_sstate) <<" joules"<< std::endl
        << std::endl;
    }

  PCM::getInstance()->cleanup();
	lighght::finalize();


}

