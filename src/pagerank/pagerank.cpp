#include <iostream>
#include <fstream>
#include <string>

#include "../include/saddlebags.hpp"



template<class Tk, class Ok, class Mt>
class SiteObject : public saddlebags::Item<Tk, Ok, Mt> {
    public:

    float page_rank = 1;
    float new_page_rank = 0;
    std::vector<int> links;

    void add_link(int new_link) {
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
    }

};


int main(int argc, char* argv[]) {

    saddlebags::init();
    if(saddlebags::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;

    auto worker = saddlebags::create_worker<int, int, float>(Buffering);
    worker->set_replication(3);
    //worker->ordered_pulls = true;
    saddlebags::add_table<SiteObject>(worker, 0, true);

    //std::ifstream infile("example_data/simple_graph.txt");
   // std::ifstream infile("/home/aril/BigDataBench_MPI_V3.1/SearchEngine/MPI_Pagerank/data-PageRank/Google_genGraph_20.txt");
    std::ifstream infile("/home/aril/Documents/Google_genGraph_17.txt");
    //std::ifstream infile("graph.txt");

    std::string src_buffer, dst_buffer;

    while(infile >> src_buffer) {

        infile >> dst_buffer;

        int url = stoi(src_buffer);
        int link = stoi(dst_buffer);

        if(worker->get_partition(0, url) == saddlebags::rank_me())
        {
            auto new_obj = saddlebags::insert_and_return<SiteObject>(worker, 0, url);
            //lighght::insert_object(worker, 0, src_buffer);
            new_obj->add_link(link);
        }
    }

    saddlebags::cycles(worker, 3);


    /*int url = 123;
    auto item = saddlebags::lookup_item<SiteObject>(worker, 0, url);
    if(item != NULL)
        std::cout << "Pagerank of page '" << url << "' is " << item->page_rank << std::endl; 
*/
	saddlebags::finalize();
}

