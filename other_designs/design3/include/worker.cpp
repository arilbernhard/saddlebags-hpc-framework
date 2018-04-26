#ifndef WORKER_CPP
#define WORKER_CPP

#include <unordered_map>
#include <iostream>
#include <typeinfo>

#include "table_container.cpp"
//#include "hashf.cpp"

template<typename TableKey, typename ObjectKey>
class Worker {

    public:
    TableContainerBase<TableKey, ObjectKey>* table;

    void do_insert(TableKey tableKey, ObjectKey objectKey)
    {
        table->insert(objectKey);
    }


    template<class ObjectType>
    void add_table(TableKey tableKey, bool is_global)
    {
        table = new TableContainer<TableKey, ObjectKey, ObjectType>(tableKey);
    }


};

#endif