#include <iostream>
#include <fstream>
#include <string>

#include "../include/timer.cpp"
#include "../include/lighght.hpp"


int main(int argc, char* argv[])
{
    lighght::init();
    if(lighght::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;

    Robin_Map<int, int> mapped_objects;

    std::ifstream infile("example_data/data.txt");

    std::string src_buffer, dst_buffer;

    while(infile >> src_buffer) {

        infile >> dst_buffer;

        int url = stoi(src_buffer);
        int link = stoi(dst_buffer);

        mapped_objects.insert(url,link);
    }

    int passed = 0;
    int failed = 0;

    for(auto obj_iterator : mapped_objects)
    {
        if(obj_iterator.second == obj_iterator.first * 3)
            passed += 1;
        else
            failed +=1;
    }

    std::cout<<passed<<" correct elements"<<std::endl<<failed<<" wrong elements"<<std::endl;

	lighght::finalize();
}

