#include <iostream>
#include <fstream>
#include <string>

#include "../../include/timer.cpp"
#include <upcxx/upcxx.hpp>
#include "../../include/cityhash/src/city.h"


class PageItem {
    
    public:
    float page_rank = 1;
    float new_page_rank = 0;
    std::vector<int> links;
};

int get_partition(int key)
{
    return abs(CityHash32((const char*)&key, sizeof(key))) % upcxx::rank_n();
}

int main(int argc, char* argv[]) {

    //Benchmark_Timer read_timer;
    //read_timer = Benchmark_Timer();


    upcxx::init();

    //if(upcxx::rank_me() == 0)
    //    std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;

    std::ifstream infile("/home/aril/Documents/Google_genGraph_20.txt");
    //std::ifstream infile("example_data/simple_graph.txt");
    //std::ifstream infile("/home/aril/BigDataBench_MPI_V3.1/SearchEngine/MPI_Pagerank/data-PageRank/Google_genGraph_14.txt");
    //std::ifstream infile("graph.txt");

    std::string src_buffer, dst_buffer;

    std::unordered_map<int, PageItem> mapped_objects;
    auto dist_map = upcxx::dist_object<std::unordered_map<int, PageItem>>(mapped_objects);
    upcxx::barrier();


    while(infile >> src_buffer) {

        infile >> dst_buffer;

        int url = stoi(src_buffer);
        int link = stoi(dst_buffer);

        if(get_partition(url) == upcxx::rank_me())
        {
            auto iterator = (*dist_map).find(url);
            if(iterator == (*dist_map).end())
            {
                PageItem newitem;
                newitem.links.push_back(link);
                (*dist_map).insert({url, newitem}); 

            }
            else {
                iterator->second.links.push_back(link);
            }

        }
    }

    upcxx::barrier();


    for(int i = 0; i < 10; i++)
    {
        for(auto obj_iterator : (*dist_map))
        {
            for(auto link : obj_iterator.second.links)
            {
                int target_partition = get_partition(link);
                float new_pr = obj_iterator.second.page_rank / (float)obj_iterator.second.links.size();
                if(target_partition == upcxx::rank_me())
                {
                    (*dist_map)[link].new_page_rank += new_pr;
                }
                else
                {
                    upcxx::rpc(target_partition, [](upcxx::dist_object<std::unordered_map<int, PageItem>> &table, int target_link, float new_page_rank) {
                        (*table)[target_link].new_page_rank += new_page_rank;
                    }, dist_map, link, new_pr);
                }
                upcxx::progress();
            }
        }

        upcxx::barrier();
        upcxx::discharge();
        upcxx::barrier();
        
        for(auto obj_iterator : (*dist_map))
        {
            (*dist_map)[obj_iterator.first].page_rank = obj_iterator.second.new_page_rank;
            (*dist_map)[obj_iterator.first].new_page_rank = 0;
        }

        upcxx::barrier();
    }

    /*read_timer.stop();
    if(get_partition("34267") == upcxx::rank_me())
    {
        std::cout << "specific page rank = " << (*dist_map)["34267"].page_rank << std::endl;
    }*/
    
    /*
    for(int i = 0; i < upcxx::rank_n(); i++)
    {
        if(i == upcxx::rank_me())
        {
            for(auto obj_iterator : (*dist_map))
            {
                std::cout << obj_iterator.first << " : " << obj_iterator.second.page_rank << std::endl;
            }
        }
        upcxx::barrier();
    }*/



    /*if(upcxx::rank_me() == 0)
        read_timer.print();*/

	upcxx::finalize();
}

