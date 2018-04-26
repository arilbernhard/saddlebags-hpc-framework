#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <vector>
#include <iostream>
#include "message.hpp"

template<class TableKey, class ObjectKey>
class DataObject {
    public:
    int value = 0;


    DataObject()
    {
        std::cout << "base object constructor" << std::endl;
        on_create();
    }

    virtual void on_create() {
        value = 0;
        std::cout << "base object on_create" << std::endl;
    }

    virtual void hello_world() {
        std::cout << " base hello world" << std::endl;
    }

};


#endif