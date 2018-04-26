#ifndef WORKER_CPP
#define WORKER_CPP

#include <upcxx/upcxx.hpp>
#include <unordered_map>
#include <iostream>
#include <typeinfo>

#include "table_container.cpp"
#include "enums.h"


//Define hash functions in the std namespace so that they can be used in unordered_map 
namespace std {
	template <>
	struct hash<vector<string>>
	{
		std::size_t operator()(const vector<string>& k) const
		{
            string tmp = "";

            for (auto s : k)
            {  
                tmp += s;
            }

            return hash<string>{}(tmp);
			//string tmp = k.term + k.doc;
			//return CityHash32((const char*)tmp.c_str(), tmp.size());
		}
	};

    template <>
    struct hash<pair<int,int>>
    {
        std::size_t operator()(const pair<int,int>& k) const
        {
            int tmp = k.first + k.second;
            return hash<int>{}(tmp);
        }
    };

}






namespace lighght
{


template<typename TableKey_T, typename ItemKey_T, typename Msg_T>
class Worker {
    public:
    std::unordered_map<TableKey_T, TableContainerBase<TableKey_T, ItemKey_T, Msg_T>*, std::hash<int>> tables; //TODO Dont assume int
    
    std::vector<std::unordered_map<TableKey_T, std::unordered_map<ItemKey_T, std::vector<Msg_T>>, std::hash<int>>> out;
    
    std::vector<Message<TableKey_T, ItemKey_T, Msg_T>> out_push_buffer;
    
    SendingMode sending_mode;
    bool ordered_pulls = false;
    bool pulls_before_pushes = true;

    Worker(SendingMode mode)
    {
        if(upcxx::rank_me() == 0)
            std::cout << "Mode is " << mode << std::endl;

        sending_mode = mode;

        if(mode == Combining)
        {
            for(int i = 0; i < upcxx::rank_n(); i++)
            {
                std::unordered_map<TableKey_T, std::unordered_map<ItemKey_T, std::vector<Msg_T>>, std::hash<int>> buf;
                out.push_back(buf);
            }
        }
    }

    void set_mode(SendingMode mode)
    {
        sending_mode = mode;
    }

    int get_partition(TableContainerBase<TableKey_T, ItemKey_T, Msg_T>* table, ItemKey_T obj)
    {
        return table->partition(obj) % upcxx::rank_n();
    }

    int get_partition(TableKey_T table_key, ItemKey_T obj)
    {
        auto target = tables[table_key];
        return target->partition(obj) % upcxx::rank_n();
    }

    bool check_if_partition(TableKey_T table, ItemKey_T obj)
    {
        return get_partition(table, obj) == upcxx::rank_me();
    }

    void apply_pushes()
    {
        for (auto table_iterator : tables)
        {
            for(auto obj_iterator : *(table_iterator.second->get_objects()))
            {

                obj_iterator.second->apply_push_buffer();
            }
        }
    }

    void apply_pulls()
    {
        for (auto table_iterator : tables)
        {
            for(auto obj_iterator : *(table_iterator.second->get_objects()))
            {
                obj_iterator.second->apply_returning_pull_buffer();
            }
        }
    }

    void do_object_work()
    {
        for (auto table_iterator : tables)
        {
            for(auto obj_iterator : (*(table_iterator.second->get_objects())))
            {
                obj_iterator.second->do_work();
            }
        }
    }

    void create_ordered_buffer()
    {
        for (auto table_iterator : tables)
        {
            for(auto obj_iterator : *(table_iterator.second->get_objects()))
            {
                obj_iterator.second->create_ordered_buffer();
            }
        }
    }

    void do_object_finishing_work()
    {
        for (auto table_iterator : tables)
        {
            for(auto obj_iterator : *(table_iterator.second->get_objects()))
            {
                obj_iterator.second->finishing_work();
            }
        }
    }

    void clear_buffers()
    {
        for (auto table_iterator : tables)
        {
            for(auto obj_iterator : *(table_iterator.second->get_objects()))
            {
                obj_iterator.second->clear_outgoing_buffers();
            }
        }
    }

};

void init() {
    upcxx::init();
}


void finalize() {
    upcxx::finalize();
}

void barrier() {
    upcxx::barrier();
}

int rank_me() {
    return upcxx::rank_me();
}


/*
template<class ObjectType, class DistributorType>
void add_table(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_Type>> &worker, TableKey_T tableKey, bool is_global)
{
    tables.insert({tableKey, new TableContainer<TableKey_T, ItemKey_T, Msg_T, ObjectType, DistributorType>()});
    tables[tableKey]->is_global = is_global;
    tables[tableKey]->myTableKey = tableKey;
    tables[tableKey]->parent_worker = this;
    tables[tableKey]->parent_dist_worker = worker;

    if(worker->sending_mode == Combining)
    {
        for(int i = 0; i < upcxx::rank_n(); i++)
        {
            std::unordered_map<ItemKey_T, std::vector<Msg_T>> buf;
            out[i].insert({tableKey, buf});
        }
    }
}*/
template<class DistributorType, template<class TableKey_T, class ItemKey_T, class Msg_Type> class ObjectType, class TableKey_T, class ItemKey_T, class Msg_Type>
void add_table(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_Type>> &worker, TableKey_T tableKey, bool is_global)
{
    worker->tables.insert({tableKey, new TableContainer<TableKey_T, ItemKey_T, Msg_Type, ObjectType<TableKey_T, ItemKey_T, Msg_Type>, DistributorType>()});
    worker->tables[tableKey]->is_global = is_global;
    worker->tables[tableKey]->myTableKey = tableKey;
    worker->tables[tableKey]->parent_worker = &(*worker);
    worker->tables[tableKey]->parent_dist_worker = worker;

    if(worker->sending_mode == Combining)
    {
        for(int i = 0; i < upcxx::rank_n(); i++)
        {
            std::unordered_map<ItemKey_T, std::vector<Msg_Type>> buf;
            worker->out[i].insert({tableKey, buf});
        }
    }
}



template<template<class TableKey_T, class ItemKey_T, class Msg_Type> class ObjectType, class TableKey_T, class ItemKey_T, class Msg_Type>
void add_table(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_Type>> &worker, TableKey_T tableKey, bool is_global)
{
    worker->tables.insert({tableKey, new TableContainer<TableKey_T, ItemKey_T, Msg_Type, ObjectType<TableKey_T, ItemKey_T, Msg_Type>, Distributor>()});
    worker->tables[tableKey]->is_global = is_global;
    worker->tables[tableKey]->myTableKey = tableKey;
    worker->tables[tableKey]->parent_worker = &(*worker);
    worker->tables[tableKey]->parent_dist_worker = &worker;

    if(worker->sending_mode == Combining)
    {
        for(int i = 0; i < upcxx::rank_n(); i++)
        {
            std::unordered_map<ItemKey_T, std::vector<Msg_Type>> buf;
            worker->out[i].insert({tableKey, buf});
        }
    }
}




template<template<class TableKey_T, class ItemKey_T, class Msg_Type> class ObjectType, class TableKey_T, class ItemKey_T, class Msg_Type>
ObjectType<TableKey_T, ItemKey_T, Msg_Type>* insert_and_return(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_Type>> &worker,
                        TableKey_T table_key, ItemKey_T item_key)
{
    auto target_table = worker->tables[table_key];
    auto target_map = target_table->get_objects();

    auto it = target_map->find(item_key);
    if(it == target_map->end()) {
        //create new object of ObjectType
        auto new_obj = new ObjectType<TableKey_T, ItemKey_T, Msg_Type>();
        //target_table->insert_newly_created(item_key, new_obj);
        
        new_obj->myItemKey = item_key;
        new_obj->parent_worker = &(*worker);
        new_obj->parent_dist_worker = &worker;
        new_obj->myTableKey = table_key;
        new_obj->on_create();
        new_obj->refresh();

        #ifdef ROBIN_HASH
        target_map->insert(item_key, new_obj);
        #else
        target_map->insert({item_key, new_obj});
        #endif

        return new_obj;
    }
    else {
        auto obj = reinterpret_cast<ObjectType<TableKey_T, ItemKey_T, Msg_Type>*>((*it).second);
        obj->refresh();
        return obj;
    }
}



template<template<class TableKey_T, class ItemKey_T, class Msg_Type> class ObjectType, class TableKey_T, class ItemKey_T, class Msg_Type>
ObjectType<TableKey_T, ItemKey_T, Msg_Type>* lookup_item(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_Type>> &worker,
                        TableKey_T table_key, ItemKey_T item_key)
{
    auto target_table = worker->tables[table_key];
    auto target_map = target_table->get_objects();

    auto it = target_map->find(item_key);
    if(it == target_map->end()) {
        return NULL;
    }
    else {
        auto obj = reinterpret_cast<ObjectType<TableKey_T, ItemKey_T, Msg_Type>*>((*it).second);
        obj->refresh();
        return obj;
    }
}



template<template<class TableKey_T, class ItemKey_T, class Msg_Type> class ObjectType, class TableKey_T, class ItemKey_T, class Msg_Type>
ObjectType<TableKey_T, ItemKey_T, Msg_Type>* get_item(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_Type>> &worker,
                        TableKey_T table_key, ItemKey_T item_key)
{
    auto target_table = worker->tables[table_key];
    auto target_map = *(target_table->get_objects());

    auto it = target_map.find(item_key);
    if(it == target_map.end()) {
        return NULL;
    }
    else {
        return reinterpret_cast<ObjectType<TableKey_T, ItemKey_T, Msg_Type>*>(it->second);
    }
}

template<class key_T, class value_T, class message_T>
upcxx::dist_object<Worker<key_T, value_T, message_T>> create_worker()
{
    Worker<key_T, value_T, message_T> w(Buffering);
    return upcxx::dist_object<Worker<key_T, value_T, message_T>>(w);
}

template<class key_T, class value_T, class message_T>
upcxx::dist_object<Worker<key_T, value_T, message_T>> create_worker(SendingMode mode)
{
    Worker<key_T, value_T, message_T> w(mode);
    return upcxx::dist_object<Worker<key_T, value_T, message_T>>(w);
}

template<typename TableKey_T, typename ItemKey_T, typename Msg_T>
void cycle(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &worker, bool do_work)
{
    //1) Work-phase
    if(do_work)
        worker->do_object_work();

    //std::cout << "work complete" << std::endl;

    if(worker->ordered_pulls == true)
        worker->create_ordered_buffer();

    upcxx::barrier();

    //2) Communication phase
    //std::cout << upcxx::rank_me() << " communication phase" << std::endl;    
    if(worker->sending_mode != Direct && worker->sending_mode != GasnetDirect)
    {
        send_pushes(worker);
    }
    send_pulls(worker);
    //std::cout << "end of communication phase" << std::endl;


    upcxx::barrier();
    //std::cout << upcxx::rank_me() << " communication complete" << std::endl;

    //3) Reset phase
    if(worker->pulls_before_pushes)
    {
        worker->apply_pulls();
        worker->apply_pushes();
    }
    else
    {
        worker->apply_pushes();
        worker->apply_pulls();
    }


    worker->clear_buffers();
    upcxx::barrier();

    //4) Finishing phase
    worker->do_object_finishing_work();
    upcxx::barrier();

}



template<typename TableKey_T, typename ItemKey_T, typename Msg_T>
void insert_object_global(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &worker, TableKey_T tableKey, ItemKey_T item_key)
{
    auto target_table = worker->tables[tableKey];
    int target_partition = worker->get_partition(target_table, item_key);
    if(target_partition == upcxx::rank_me())
    {
        target_table->insert(item_key);
    }
    else
    {
        upcxx::rpc_ff(target_partition, [](ItemKey_T ok) {
            //TODO
            std::cout << "this is a rpc" << std::endl;
        }, item_key);

    }
}

template<typename TableKey_T, typename ItemKey_T, typename Msg_T>
void insert_object(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &worker, TableKey_T tableKey, ItemKey_T item_key)
{
    auto target_table = worker->tables[tableKey];
    if(target_table->is_global == true)
        return insert_object_global(worker, tableKey, item_key);
    target_table->insert(item_key);
}


template<typename TableKey_T, typename ItemKey_T, typename Msg_T, typename ObjectType>
void insert_existing_global(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &worker, TableKey_T tableKey, ItemKey_T item_key, ObjectType new_object)
{
    auto target_table = worker->tables[tableKey];
    int target_partition = worker->get_partition(target_table, item_key);
    if(target_partition == upcxx::rank_me())
    {
            target_table->insert_existing(item_key, &new_object);
    }
    else
    {
        std::cout << "need to globally insert " << item_key << std::endl;
        upcxx::rpc_ff(target_partition, [](upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &dw, 
                        TableKey_T tk, ItemKey_T ok, ObjectType new_obj) {
            new_obj.do_work();
            dw->tables[tk]->insert_existing(ok, &new_obj);
        }, worker, tableKey, item_key, new_object);

    }
}

template<typename TableKey_T, typename ItemKey_T, typename Msg_T, typename ObjectType>
void insert_existing(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &worker, TableKey_T tableKey, ItemKey_T item_key, ObjectType new_object)
{
    auto target_table = worker->tables[tableKey];
    if(target_table->is_global == true) {
        return insert_existing_global(worker, tableKey, item_key, new_object);
    }
    target_table->insert_existing(item_key, &new_object);
}

template<typename TableKey_T, typename ItemKey_T, typename Msg_T>
void push_buf(Worker<TableKey_T, ItemKey_T, Msg_T> &w, TableKey_T table_key, ItemKey_T item_key, Msg_T message)
{
    auto target_table = w.tables[table_key];
    if(target_table->is_global == true)
    {
        int target_partition = w.get_partition(table_key, item_key);

        if(target_partition != upcxx::rank_me())
        {
            auto it = w.out[target_partition][table_key].find(item_key);

            if(it == w.out[target_partition][table_key].end())
            {
                std::vector<Msg_T> send_vector;
                send_vector.push_back(message);
                w.out[target_partition][table_key].insert({item_key, send_vector});
            }
            else
            {
                w.out[target_partition][table_key][item_key].push_back(message);
            }
            return;
        }
    }
    target_table->push(item_key, message);
}

template<typename TableKey_T, typename ItemKey_T, typename Msg_T>
void send_pushes(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &worker)
{
    //for (auto table_iterator : worker->tables)
   // {
        /*
        for(auto obj_iterator : *(table_iterator.second->get_objects()))
        {                
            for(auto msg_iterator : obj_iterator.second->out_push_buffer)
            {
                auto target_table = worker->tables[msg_iterator.dest_table];
                if(target_table->is_global == true)
                {
                    int target_partition = worker->get_partition(table_iterator.second, msg_iterator.dest_object);

                    if(target_partition != upcxx::rank_me())
                    {
                        auto it = worker->out[target_partition].find(msg_iterator.dest_object);

                        if(it == worker->out[target_partition].end())
                        {
                            std::vector<Msg_T> send_vector;
                            send_vector.push_back(msg_iterator.value);
                            worker->out[target_partition].insert({msg_iterator.dest_object, send_vector});
                        }
                        else
                        {
                            worker->out[target_partition][msg_iterator.dest_object].push_back(msg_iterator.value);
                        }
                        continue;
                    }
                }
                target_table->push(msg_iterator.dest_object, msg_iterator.value);
            }
        }*/

    //     for(int i = 0; i < upcxx::rank_n(); i++)
    //     {
    //         if(i == upcxx::rank_me()) {
    //             continue;
    //         }

    //         upcxx::rpc_ff(i, [](upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &dw, TableKey_T tk,
    //                         std::unordered_map<ItemKey_T, std::vector<Msg_T>> incoming)
    //         {
    //             dw->tables[tk]->bulk_push(incoming);
    //         }, worker, table_iterator.first, worker->out[i][table_iterator.first]);
    //     }
    // }

    if(worker->sending_mode == Combining){
        //Send pushes
        //For each target partition
        for(int i = 0; i < upcxx::rank_n(); i++)
        {
            //For each table
            for (auto table_iterator : worker->out[i])
            {
                //Send a map containing <ItemKey_T, Vector<Push Messages>>
                upcxx::rpc_ff(i, [](upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &dw, TableKey_T tk, std::unordered_map<ItemKey_T, std::vector<Msg_T>> incoming)
                {
                    dw->tables[tk]->bulk_push(incoming);
                }, worker, table_iterator.first, table_iterator.second);
            }
        }

        //Clear out buffers
        for(int i = 0; i < upcxx::rank_n(); i++)
        {
            for (auto table_iterator : worker->tables)
            {
                std::unordered_map<ItemKey_T, std::vector<Msg_T>> buf;
                worker->out[i][table_iterator.first] = buf;
            }
        }
    }
    else if(worker->sending_mode == Buffering) {
        for (auto table_iterator : worker->tables)
        {
            for(auto obj_iterator : *(table_iterator.second->get_objects()))
            {
                for(auto msg_iterator : obj_iterator.second->out_push_buffer)
                {
                    perform_remote_push(worker, msg_iterator.dest_table, msg_iterator.dest_object, msg_iterator.value);
                }
            }
        }
        upcxx::progress();
    }
    else if(worker->sending_mode == BufferingWorker)
    {
        for(auto msg_iterator : worker->out_push_buffer)
        {
            perform_remote_push(worker, msg_iterator.dest_table, msg_iterator.dest_object, msg_iterator.value);
        }
    }

    upcxx::progress();
}

template<typename TableKey_T, typename ItemKey_T, typename Msg_T>
void perform_remote_push(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &worker, TableKey_T dest_table, ItemKey_T dest_obj, Msg_T msg_val)
{
    auto target_table = worker->tables[dest_table];
    if(target_table->is_global == true)
    {
        int target_partition = worker->get_partition(target_table, dest_obj);

        if(target_partition != upcxx::rank_me())
        {
            upcxx::rpc(target_partition, [](upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &dw,
            Msg_T val, TableKey_T tk, ItemKey_T ok) {
                dw->tables[tk]->push(ok, val);
            }, worker, msg_val, dest_table, dest_obj);
            upcxx::progress();
            return;
        }
    }
    target_table->push(dest_obj, msg_val);
}

template<typename TableKey_T, typename ItemKey_T, typename Msg_T>
void perform_direct_remote_push(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &worker, TableKey_T dest_table, ItemKey_T dest_obj, Msg_T msg_val)
{
    auto target_table = worker->tables[dest_table];
    if(target_table->is_global == true)
    {
        int target_partition = worker->get_partition(target_table, dest_obj);

        if(target_partition != upcxx::rank_me())
        {
            upcxx::rpc(target_partition, [](upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &dw,
            Msg_T val, TableKey_T tk, ItemKey_T ok) {
                dw->tables[tk]->push(ok, val);
            }, worker, msg_val, dest_table, dest_obj);
            upcxx::progress();
            return;
        }
    }
    target_table->push(dest_obj, msg_val);
}

template<typename TableKey_T, typename ItemKey_T, typename Msg_T>
void send_pulls(upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &worker)
{
    for (auto table_iterator : worker->tables)
    {
        for(auto obj_iterator : *(table_iterator.second->get_objects()))
        {                
            for(auto msg_iterator : obj_iterator.second->out_pull_buffer)
            {
                auto target_table = worker->tables[msg_iterator.dest_table];
                if(target_table->is_global == true)
                {
                    int target_partition = worker->get_partition(target_table, msg_iterator.dest_object);
                    if(target_partition != upcxx::rank_me())
                    {
                        auto fut = upcxx::rpc(target_partition,
                            [](upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>> &dw, TableKey_T tk, ItemKey_T ok, int tag)
                            {
                                return dw->tables[tk]->foreign_pull(ok, tag);
                            }, worker, msg_iterator.dest_table, msg_iterator.dest_object, msg_iterator.tag).then(
                            [obj_iterator, msg_iterator](Msg_T retval)
                            {
                                Message<TableKey_T, ItemKey_T, Msg_T> new_msg;
                                new_msg.src_table = msg_iterator.dest_table;
                                new_msg.dest_table = msg_iterator.src_table;
                                new_msg.src_object = msg_iterator.dest_object;
                                new_msg.dest_object = msg_iterator.src_object;
                                new_msg.value = retval;
                                new_msg.tag = msg_iterator.tag;
                                new_msg.seqnum = msg_iterator.seqnum;
                                obj_iterator.second->add_returning_pull_buffer(new_msg);
                            });
                        fut.wait();//TODO: don't wait here/investigate if RPCs are handled while waiting, if so, waiting here is OK
                        upcxx::progress();
                        continue;
                    }
                }
                Message<TableKey_T, ItemKey_T, Msg_T> new_msg;
                new_msg.value = target_table->foreign_pull(msg_iterator.dest_object, msg_iterator.tag);
                new_msg.tag = msg_iterator.tag;
                new_msg.src_table = msg_iterator.dest_table;
                new_msg.dest_table = msg_iterator.src_table;
                new_msg.src_object = msg_iterator.dest_object;
                new_msg.dest_object = msg_iterator.src_object;
                new_msg.seqnum = msg_iterator.seqnum;
                obj_iterator.second->add_returning_pull_buffer(new_msg);
            }
        }
    }
    upcxx::progress();
}

} //end namespace


#endif