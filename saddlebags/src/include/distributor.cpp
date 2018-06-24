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
class CyclicDistributor : public BaseDistributor<ItemKey_T> {
    public:

    int distribute(int key) override
    {
        return key % upcxx::rank_n(); 
    }
};

template<typename ItemKey_T>
class CyclicDistributor2D : public BaseDistributor<ItemKey_T> {
    public:

    int distribute(std::pair<int, int> key) override
    {
        return (key.first + key.second) % upcxx::rank_n(); 
    }
};


} //end namespace

#endif
