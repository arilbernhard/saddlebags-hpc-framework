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

    saddlebags::add_table<SiteObject>(worker, 0, true);

    std::ifstream infile("example_data/simple_graph.txt");


    std::string src_buffer, dst_buffer;

    while(infile >> src_buffer) {

        infile >> dst_buffer;

        int url = stoi(src_buffer);
        int link = stoi(dst_buffer);

        if(worker->get_partition(0, url) == saddlebags::rank_me())
        {
            auto new_obj = saddlebags::insert_and_return<SiteObject>(worker, 0, url);
            new_obj->add_link(link);
        }
    }

    saddlebags::cycles(worker, 3);


    saddlebags::finalize();
}

