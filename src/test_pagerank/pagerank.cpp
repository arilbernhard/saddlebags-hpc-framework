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


int main(int argc, char* argv[])
{
    lighght::init();
    if(lighght::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;

    using table_k = int;
    using object_k = int;
    using msg_type = float;

    auto worker = lighght::create_worker<table_k, object_k, msg_type>(Buffering);

    lighght::add_table<SiteObject>(worker, 0, true);

    std::ifstream infile("example_data/simple_graph.txt");
    //std::ifstream infile("/home/aril/Documents/Google_genGraph_15.txt");
    //std::ifstream infile("/home/aril/BigDataBench_MPI_V3.1/SearchEngine/MPI_Pagerank/data-PageRank/Google_genGraph_17.txt");
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

    std::cout << "end of insertion phase" << std::endl;



    for(int i = 1; i < 4; i++)
    {
        upcxx::barrier();
        lighght::cycle(worker, true);
        if(upcxx::rank_me() == 0)
            std::cout << "CYCLE " << i << std::endl;
        upcxx::barrier();

        std::string filename = "example_data/solution"+std::to_string(i)+".txt";
        std::ifstream solfile(filename);
        
        float solution[9];

        while(solfile >> src_buffer) {
            solfile >> dst_buffer;
            solution[stoi(src_buffer)] = stof(dst_buffer);
        }

        int items = 0;

        for(auto table_iterator : worker->tables)
        {
            for(auto obj_iterator : *(table_iterator.second->get_objects()))
            {
                items += 1;
                auto obj = obj_iterator.second;
                auto page_rank = reinterpret_cast<SiteObject<table_k, object_k, msg_type>*>(obj)->page_rank;
                int url = obj_iterator.first;
                if(url < 9)
                {
                    if(abs(solution[url] - page_rank) > 0.01)
                        std::cout << "Failed: " << url << " has rank " << page_rank << " (expected " << solution[url] << ")" << std::endl;
                }
            }
        }

        std::cout << "Partition " << upcxx::rank_me() << " has in total " << items << " items" << std::endl;



    }



	lighght::finalize();
}

