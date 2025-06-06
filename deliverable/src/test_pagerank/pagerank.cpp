#include <iostream>
#include <fstream>
#include <string>

#include "../include/saddlebags.hpp"

template<class Tk, class Ok, class Mt>
class SiteObject : public saddlebags::Item<Tk, Ok, Mt> {
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


        }
    }

    void foreign_push(Mt val) override {
        new_page_rank += val;

    }

    void finishing_work() override {
        page_rank = new_page_rank;
        new_page_rank = 0;

        cycle++;
    }
};


int main(int argc, char* argv[])
{
    saddlebags::init();
    if(saddlebags::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;


    auto worker = saddlebags::create_worker<int, int, float>(Buffering);
    


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


    for(int i = 1; i < 4; i++)
    {
        saddlebags::cycle(worker, true);
        if(upcxx::rank_me() == 0)
            std::cout << "CYCLE " << i << std::endl;

        std::string filename = "example_data/solution"+std::to_string(i)+".txt";
        std::ifstream solfile(filename);
        
        float solution[9];

        while(solfile >> src_buffer) {
            solfile >> dst_buffer;
            solution[stoi(src_buffer)] = stof(dst_buffer);
        }

        int items = 0;


        /*  Different methods of accessing data follows.
            Correctness is checked by comparing url with solution
            The results of the following loops are equivalent */ 

        //1) Lookup items based on their url. Lookup will return NULL on partitions where it does not exist
        for(int url = 1; url < 9; url++)
        {
            auto item = saddlebags::lookup_item<SiteObject>(worker, 0, url);
            if(item == NULL)
                continue;
            auto page_rank = item->page_rank;
            if(abs(solution[url] - page_rank) > 0.01)
                std::cout << "Failed: " << url << " has rank " << page_rank << " (expected " << solution[url] << ")" << std::endl;
        }

        //2) Similar to 1, but check the distribution logic rather than relying on what lookup_item returns
        //the differentiation between the two is useful because it can be ensured whether an item exists in any table
        for(int url = 1; url < 9; url++)
        {
            if(worker->check_if_partition(0, url))
            {
                auto item = saddlebags::lookup_item<SiteObject>(worker, 0, url);
                auto page_rank = item->page_rank;
                if(abs(solution[url] - page_rank) > 0.01)
                    std::cout << "Failed: " << url << " has rank " << page_rank << " (expected " << solution[url] << ")" << std::endl;
            }
        }

        //3) Iterate through all items on each partition
        for(auto obj_iterator : saddlebags::iterate_table<SiteObject>(worker, 0))
        {
            items += 1;
            int url = obj_iterator.first;
            auto page_rank = obj_iterator.second->page_rank;
            if(url < 9)
            {
                if(abs(solution[url] - page_rank) > 0.01)
                    std::cout << "Failed: " << url << " has rank " << page_rank << " (expected " << solution[url] << ")" << std::endl;
            }
        }


        std::cout << "Partition " << upcxx::rank_me() << " has in total " << items << " items" << std::endl;
    }



    saddlebags::finalize();
}

