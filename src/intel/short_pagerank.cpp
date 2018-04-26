#include <iostream>
#include <fstream>
#include <string>
#include "../include/lighght.hpp"

template<class Tk, class Ok, class Mt>
class SiteObject : public lighght::Object<Tk, Ok, Mt> {
    public:

    float page_rank = 1;
    float new_page_rank = 0;
    std::vector<std::string> links;

    void add_link(std::string new_link) {
        this->links.push_back(new_link);
    }

    void do_work() override {
        for(auto it : this->links) {
            lighght::Object<Tk, Ok, Mt>::push(0, it, page_rank/((float)links.size()));
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
    upcxx::init();

    using table_k = int;
    using object_k = std::string;
    using msg_type = float;

    auto worker = lighght::create_worker<table_k, object_k, msg_type>();
    worker->add_table<SiteObject<table_k, object_k, msg_type>>(0, true);

    std::ifstream infile("graph.txt");
    std::string src_buffer, dst_buffer;

    while(infile >> src_buffer) {
        infile >> dst_buffer;
        if(worker->get_partition(0, src_buffer) == upcxx::rank_me()) {
            auto new_obj = lighght::insert_and_return<SiteObject, table_k, object_k, msg_type>(worker, 0, src_buffer);
            new_obj->add_link(dst_buffer);
        }
    }

    for(int i = 0; i < 3; i++)
        lighght::cycle(worker, true);

	upcxx::finalize();
}