#ifndef TABLECONTAINER_H
#define TABLECONTAINER_H

#include <iostream>
#include <unordered_map>
#include <functional>
#include <cmath>

#include "data_object.cpp"
#include "distributor.cpp"
#include "hash_map.cpp"
#include <assert.h>

#define ROBIN_HASH

namespace lighght
{

template<typename TableKey_T, typename ItemKey_T, typename Msg_T> class Worker;


template <typename TableKey_T, typename ItemKey_T, typename Msg_T>
class TableContainerBase
{
    public:
    TableKey_T myTableKey;
    bool is_global = false;
    Worker<TableKey_T, ItemKey_T, Msg_T>* parent_worker;
    upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>>* parent_dist_worker;

    virtual void insert(ItemKey_T k) = 0;
    virtual int partition(ItemKey_T objectKey) = 0;
    #ifdef ROBIN_HASH
    virtual Robin_Map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>* get_objects() = 0;
    #else
    virtual std::unordered_map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>* get_objects() = 0;
    #endif
    virtual void insert_existing(ItemKey_T k, Item<TableKey_T, ItemKey_T, Msg_T>* obj) = 0;
    virtual void insert_newly_created(ItemKey_T k, Item<TableKey_T, ItemKey_T, Msg_T>* obj) = 0;
    virtual void push(ItemKey_T key, Msg_T val) = 0;
    virtual Msg_T foreign_pull(ItemKey_T key, int tag) = 0;
    virtual void bulk_push(std::unordered_map<ItemKey_T, std::vector<Msg_T>> messages) = 0;
    virtual void direct_push(ItemKey_T key, Msg_T val) = 0;
};


//TODO replace word 'object' with 'item'

template <typename TableKey_T, typename ItemKey_T, typename Msg_T, typename ObjectType, typename DistributorType>
class TableContainer : public TableContainerBase<TableKey_T, ItemKey_T, Msg_T> {
    public:

    DistributorType distributor;
    #ifdef ROBIN_HASH
    Robin_Map<ItemKey_T, ObjectType*> mapped_objects;

    Robin_Map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>* get_objects() override
    {
        return reinterpret_cast<Robin_Map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>*>(&mapped_objects);
    }
    #else

    std::unordered_map<ItemKey_T, ObjectType*> mapped_objects;

    std::unordered_map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>* get_objects() override
    {
        return reinterpret_cast<std::unordered_map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>*>(&mapped_objects);
    }

    #endif

    TableContainer()
    {
        
    }

    ObjectType* create_new_item(ItemKey_T key)
    {
        auto newobj = new ObjectType();
        newobj->parent_worker = this->parent_worker;
        newobj->parent_dist_worker = this->parent_dist_worker;
        newobj->myItemKey = key;
        newobj->myTableKey = this->myTableKey;
        newobj->on_create();
        newobj->refresh();
        return newobj;
    }

    void push(ItemKey_T key, Msg_T val) override
    {
        auto iterator = mapped_objects.find(key);
		if(iterator == mapped_objects.end())
		{
            auto newobj = create_new_item(key);
            newobj->in_push_buffer.push_back(val);

            #ifdef ROBIN_HASH
            mapped_objects.insert(key, newobj);
            #else
            mapped_objects[key] = newobj;
            #endif
        }
        else {
            assert((*iterator).second != NULL);
            (*iterator).second->in_push_buffer.push_back(val);
        }
    }

    void direct_push(ItemKey_T key, Msg_T val) override
    {
        auto iterator = mapped_objects.find(key);
		if(iterator == mapped_objects.end())
		{
            auto newobj = create_new_item(key);
            newobj->foreign_push(val);

            #ifdef ROBIN_HASH
            mapped_objects.insert(key, newobj);
            #else
            mapped_objects[key] = newobj;
            #endif
        }
        else {
            assert((*iterator).second != NULL);
            (*iterator).second->foreign_push(val);
        }
    }

    Msg_T foreign_pull(ItemKey_T key, int tag) override
    {
        auto iterator = mapped_objects.find(key);
		//TODO: Create new item or error if pull item that does not exist
        if(iterator == mapped_objects.end())
		{
            std::cout << " not found " << std::endl;
            Msg_T tmp;
            return tmp;
        }
        else {
            assert((*iterator).second != NULL);
            return (*iterator).second->foreign_pull(tag);
        }
    }

    int partition(ItemKey_T objectKey) override {
        return distributor.distribute(objectKey);
    }

    //TODO inspect these insertions

    void insert(ItemKey_T key) override
    { 
        auto iterator = mapped_objects.find(key);
		if(iterator == mapped_objects.end())
		{
            ObjectType* newobj = create_new_item(key);

            #ifdef ROBIN_HASH
            mapped_objects.insert(key, newobj);
            #else
            mapped_objects[key] = newobj;
            #endif
		}
		else {
            assert((*iterator).second != NULL);
			(*iterator).second->refresh();
		}
    }

    void insert_existing(ItemKey_T k, Item<TableKey_T, ItemKey_T, Msg_T>* obj) override
    {
        ObjectType* new_obj = new ObjectType(*(reinterpret_cast<ObjectType*>(obj)));
        
        new_obj->myItemKey = k;
        new_obj->myTableKey = TableContainerBase<TableKey_T, ItemKey_T, Msg_T>::myTableKey;
        new_obj->parent_worker = TableContainerBase<TableKey_T, ItemKey_T, Msg_T>::parent_worker;
        new_obj->parent_dist_worker = this->parent_dist_worker;
        new_obj->on_create();
        new_obj->refresh();

        #ifdef ROBIN_HASH
        mapped_objects.insert(k, new_obj);
        #else
        mapped_objects[k] = new_obj;
        #endif
    }

    void insert_newly_created(ItemKey_T k, Item<TableKey_T, ItemKey_T, Msg_T>* obj) override
    {
        ObjectType* new_obj = reinterpret_cast<ObjectType*>(obj);
        
        new_obj->myItemKey = k;
        new_obj->parent_worker = TableContainerBase<TableKey_T, ItemKey_T, Msg_T>::parent_worker;
        new_obj->parent_dist_worker = this->parent_dist_worker;
        new_obj->myTableKey = TableContainerBase<TableKey_T, ItemKey_T, Msg_T>::myTableKey;
        new_obj->on_create();
        new_obj->refresh();

        #ifdef ROBIN_HASH
        mapped_objects.insert(k, new_obj);
        #else
        mapped_objects[k] = new_obj;
        #endif
    }

    void bulk_push(std::unordered_map<ItemKey_T, std::vector<Msg_T>> messages) override
    {
        for(auto obj_iterator : messages)
        {
            for(auto val_iterator : obj_iterator.second)
            {
                push(obj_iterator.first, val_iterator);
            }
        }
    }


};

}//end namespace

#endif