#ifndef DISTRIBUTOR_CPP
#define DISTRIBUTOR_CPP

#include <upcxx/upcxx.hpp>
#include "cityhash/src/city.h"

namespace saddlebags
{

template<typename ItemKey_T>
class BaseDistributor
{
    public:
    virtual int distribute(ItemKey_T key) = 0;
};

template<typename ItemKey_T>
class HashDistributor : public BaseDistributor<ItemKey_T> {
    public:

    int distribute(int key) override
    {
        return CityHash32((const char*)(&key), sizeof(key)) % upcxx::rank_n(); 
    }


    int distribute(std::string key) override
    {
        return CityHash32((const char*)key.c_str(), (size_t)key.size()) % upcxx::rank_n();
    }
/*
    int distribute(std::pair<int,int> key)
    {
        long k = key.first + key.second;
        return CityHash32((const char*)(&k), sizeof(k)) % upcxx::rank_n();
    }

    int distribute(int key)
    {
        return CityHash32((const char*)(&key), sizeof(key)) % upcxx::rank_n();
    }

    int distribute(std::vector<std::string> key)
    {
        std::string tmp;
        for (auto it : key)
        {
            tmp += it;
        }

        return distribute(tmp);
    }*/

};


} //end namespace

#endif
