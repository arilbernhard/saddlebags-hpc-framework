#ifndef TABLECONTAINER_H
#define TABLECONTAINER_H

#include <iostream>
#include <unordered_map>
#include <functional>
#include <cmath>

#include "data_item.cpp"
#include "distributor.cpp"
#include "hash_map.cpp"
#include <assert.h>

#define ROBIN_HASH

namespace saddlebags
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
    virtual int partition(ItemKey_T itemKey) = 0;
    #ifdef ROBIN_HASH
    virtual Robin_Map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>* get_items() = 0;
    #else
    virtual std::unordered_map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>* get_items() = 0;
    #endif
    virtual void insert_preexisting(ItemKey_T k, Item<TableKey_T, ItemKey_T, Msg_T>* obj) = 0;
    virtual void insert_newly_created(ItemKey_T k, Item<TableKey_T, ItemKey_T, Msg_T>* obj) = 0;
    virtual void push(ItemKey_T key, Msg_T val) = 0;
    virtual Msg_T foreign_pull(ItemKey_T key, int tag) = 0;
    virtual void bulk_push(std::unordered_map<ItemKey_T, std::vector<Msg_T>> messages) = 0;
    virtual void direct_push(ItemKey_T key, Msg_T val) = 0;

    virtual void replicate() = 0;
    virtual void set_replication_level(int lvl) = 0;
};

template <typename TableKey_T, typename ItemKey_T, typename Msg_T, typename ItemType>
class TableContainer : public TableContainerBase<TableKey_T, ItemKey_T, Msg_T> {
    public:

    BaseDistributor<ItemKey_T>* distributor;
    int key_partition_space;
    
    std::vector<std::vector<std::pair<ItemKey_T, ItemType>>> replicated_buf;
    int replication_level = 0;

    #ifdef ROBIN_HASH
    Robin_Map<ItemKey_T, ItemType*> mapped_items;
    Robin_Map<ItemKey_T, ItemType*> replicated_items;

    Robin_Map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>* get_items() override
    {
        return reinterpret_cast<Robin_Map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>*>(&mapped_items);
    }
    #else

    std::unordered_map<ItemKey_T, ItemType*> mapped_items;
    std::unordered_map<ItemKey_T, ItemType*> replicated_items;

    std::unordered_map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>* get_items() override
    {
        return reinterpret_cast<std::unordered_map<ItemKey_T, Item<TableKey_T, ItemKey_T, Msg_T>*>*>(&mapped_items);
    }

    #endif

    TableContainer(BaseDistributor<ItemKey_T>* new_distributor)
    {
        distributor = new_distributor;
    }
    
    TableContainer()
    {
        distributor = NULL;
        key_partition_space = INT32_MAX / upcxx::rank_n();
    }

    void set_replication_level(int lvl) override
    {
        for(int i = 0; i < lvl; i++)
        {
            std::vector<std::pair<ItemKey_T, ItemType>> buf;
            replicated_buf.push_back(buf);
        }
        replication_level = lvl;
    }

    void replicate() override 
    {
        if(replication_level < 1)
            return;
        
        std::vector<std::pair<ItemKey_T, ItemType>> my_items;
        for(auto obj_iterator : mapped_items)
        {
            my_items.push_back(std::make_pair(obj_iterator.first, *obj_iterator.second));
        }
        for(int i = 0; i < replication_level; i++)
        {
            int target_partition = (upcxx::rank_me() + i+1) % upcxx::rank_n();

            upcxx::rpc(target_partition, [](upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &dw, TableKey_T tk, std::vector<std::pair<ItemKey_T, ItemType>> buf, int index) {
                auto base_table = dw->tables[tk];
                auto derived_table = reinterpret_cast<TableContainer<TableKey_T, ItemKey_T, Msg_T, ItemType>*>(base_table);
                derived_table->receive_replication(buf, index);
            }, *(this->parent_dist_worker), this->myTableKey, my_items, i);
        }
    }

    void receive_replication(std::vector<std::pair<ItemKey_T, ItemType>> incoming_replica, int index)
    {
        replicated_buf[index] = incoming_replica;
    }

    ItemType* create_new_item(ItemKey_T key)
    {
        auto newobj = new ItemType();
        newobj->parent_dist_worker = this->parent_dist_worker;
        newobj->myItemKey = key;
        newobj->myTableKey = this->myTableKey;
        newobj->on_create();
        newobj->refresh();
        return newobj;
    }

    void push(ItemKey_T key, Msg_T val) override
    {
        auto iterator = mapped_items.find(key);
		if(iterator == mapped_items.end())
		{
            auto newobj = create_new_item(key);
            newobj->in_push_buffer.push_back(val);

            #ifdef ROBIN_HASH
            mapped_items.insert(key, newobj);
            #else
            mapped_items[key] = newobj;
            #endif
        }
        else {
            assert((*iterator).second != NULL);
            (*iterator).second->in_push_buffer.push_back(val);
        }
    }

    void direct_push(ItemKey_T key, Msg_T val) override
    {
        auto iterator = mapped_items.find(key);
		if(iterator == mapped_items.end())
		{
            auto newobj = create_new_item(key);
            newobj->foreign_push(val);

            #ifdef ROBIN_HASH
            mapped_items.insert(key, newobj);
            #else
            mapped_items[key] = newobj;
            #endif
        }
        else {
            assert((*iterator).second != NULL);
            (*iterator).second->foreign_push(val);
        }
    }

    Msg_T foreign_pull(ItemKey_T key, int tag) override
    {
        auto iterator = mapped_items.find(key);
        if(iterator == mapped_items.end())
		{
            auto newobj = create_new_item(key);

            #ifdef ROBIN_HASH
            mapped_items.insert(key, newobj);
            #else
            mapped_items[key] = newobj;
            #endif

            return newobj->foreign_pull(tag);
        }
        else {
            assert((*iterator).second != NULL);
            return (*iterator).second->foreign_pull(tag);
        }
    }

    int partition(ItemKey_T itemKey) override {
        if(distributor != NULL)
            return distributor->distribute(itemKey);
        else
            return default_distribute(itemKey);
    }

    void insert(ItemKey_T key) override
    {
        auto iterator = mapped_items.find(key);
		if(iterator == mapped_items.end())
		{
            ItemType* newobj = create_new_item(key);

            #ifdef ROBIN_HASH
            mapped_items.insert(key, newobj);
            #else
            mapped_items[key] = newobj;
            #endif
		}
		else {
            assert((*iterator).second != NULL);
			(*iterator).second->refresh();
		}
    }

    void insert_preexisting(ItemKey_T k, Item<TableKey_T, ItemKey_T, Msg_T>* obj) override
    {
        ItemType* new_obj = new ItemType(*(reinterpret_cast<ItemType*>(obj)));
        
        new_obj->myItemKey = k;
        new_obj->myTableKey = TableContainerBase<TableKey_T, ItemKey_T, Msg_T>::myTableKey;
        new_obj->parent_dist_worker = this->parent_dist_worker;
        new_obj->on_create();
        new_obj->refresh();

        #ifdef ROBIN_HASH
        mapped_items.insert(k, new_obj);
        #else
        mapped_items[k] = new_obj;
        #endif
    }

    void insert_newly_created(ItemKey_T k, Item<TableKey_T, ItemKey_T, Msg_T>* obj) override
    {
        ItemType* new_obj = reinterpret_cast<ItemType*>(obj);
        
        new_obj->myItemKey = k;
        new_obj->parent_dist_worker = this->parent_dist_worker;
        new_obj->myTableKey = TableContainerBase<TableKey_T, ItemKey_T, Msg_T>::myTableKey;
        new_obj->on_create();
        new_obj->refresh();

        #ifdef ROBIN_HASH
        mapped_items.insert(k, new_obj);
        #else
        mapped_items[k] = new_obj;
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

    int default_distribute(int key)
    {
        //return CityHash32((const char*)(&key), sizeof(key)) / key_partition_space;
        return CityHash32((const char*)(&key), sizeof(key)) % upcxx::rank_n(); 
    }

    int default_distribute(std::string key)
    {
        return CityHash32((const char*)key.c_str(), (size_t)key.size()) % upcxx::rank_n();
    }

    int default_distribute(std::vector<std::string> key)
    {
        std::string tmp;
        for (auto it : key)
        {
            tmp += it;
        }
        return default_distribute(tmp);
    }

};


//Return iterator to item-map in which every item is cast to derived itemtype
template<template<class TableKey_T, class ItemKey_T, class Msg_Type> class ItemType, class TableKey_T, class ItemKey_T, class Msg_Type>
Robin_Map<ItemKey_T, ItemType<TableKey_T, ItemKey_T, Msg_Type>*> 
iterate_table(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_Type>> &worker, TableKey_T table_key)
{
    TableContainerBase<TableKey_T, ItemKey_T, Msg_Type>* base_table = worker->tables[table_key];
    auto derived_table = reinterpret_cast<TableContainer<TableKey_T, ItemKey_T, Msg_Type, ItemType<TableKey_T, ItemKey_T, Msg_Type>>*>(base_table);
    return derived_table->mapped_items;
}

}//end namespace

#endif