#ifndef TABLECONTAINER_H
#define TABLECONTAINER_H

#include <iostream>
#include <unordered_map>
#include <functional>

#include "data_object.hpp"


template <typename TableKey, typename ObjectKey>
class TableContainerBase
{
    public:

    bool is_global = false;

    virtual void insert(ObjectKey k)
    {
        std::cout << "base container insert" << std::endl;
    }
};




template <typename TableKey, typename ObjectKey, typename ObjectType>
class TableContainer : public TableContainerBase<TableKey, ObjectKey> {
    public:
    
    ObjectType* mapped_object;


    TableContainer(TableKey k)
    {
        std::cout << "INHERITED CONTAINER CONSTRUCTOR " << std::endl;
        //tps = std::remove_reference<decltype(new ObjectType())>;
    }

    void insert(ObjectKey k) override
    {

        std::cout << "INHERITED insert" << std::endl;
        //mapped_objects.insert({k, factory_lambda()});
        std::cout << "before creation" << std::endl;
        //mapped_objects.insert({k, new ObjectType()});
        mapped_object = new ObjectType();

        ObjectType obj;
        obj.on_create();
        mapped_object->on_create();

        //mapped_object->hello_world();

        reinterpret_cast<ObjectType*>(&obj)->on_create();
        reinterpret_cast<ObjectType*>(mapped_object)->on_create();
        
        std::cout << "after creation" << std::endl;
        
        mapped_object->hello_world();

        //mapped_objects[k]->on_create();
        //mapped_objects[k]->myObjectKey = k;
        //mapped_objects[k]->myTableKey = myTableKey;
    }
};

#endif