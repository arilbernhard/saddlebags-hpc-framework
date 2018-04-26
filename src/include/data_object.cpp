#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <vector>
#include <iostream>
#include "message.cpp"

#include "enums.h"


namespace lighght
{

    
template<typename TableKey_T, typename ItemKey_T, typename Msg_T> class Worker;

template<class TableKey_T, class ItemKey_T, class Msg_T>
class Item {
    public:
    Msg_T value;

    TableKey_T myTableKey;
    ItemKey_T myItemKey;
    std::vector<Message<TableKey_T, ItemKey_T, Msg_T>> out_pull_buffer;
    std::vector<Message<TableKey_T, ItemKey_T, Msg_T>> out_push_buffer;
    std::vector<Message<TableKey_T, ItemKey_T, Msg_T>> returning_pull_buffer;
    //std::vector<Message<TableKey_T, ItemKey_T>> in_pull_buffer; //TODO remove extra buffers
    //std::vector<Message<TableKey_T, ItemKey_T>> in_push_buffer;
    std::vector<Msg_T> in_push_buffer;

    Worker<TableKey_T, ItemKey_T, Msg_T>* parent_worker; //TODO Remove non-distributed worker
    upcxx::dist_object<Worker<TableKey_T, ItemKey_T, Msg_T>>* parent_dist_worker;

    int next_seqnum = 0;

    Item()
    {
        
    }

    void direct_push(TableKey_T destTableKey, ItemKey_T destObjectKey, Msg_T val)
    {
        perform_remote_push(*(parent_dist_worker), destTableKey, destObjectKey, val);
    }


    //Add push message to outgoing buffer
    void push(TableKey_T destTableKey, ItemKey_T destObjectKey, Msg_T val)
    {
        Message<TableKey_T, ItemKey_T, Msg_T> msg;
        
        switch((*parent_dist_worker)->sending_mode) {

            case Combining : 
                push_buf(*parent_worker, destTableKey, destObjectKey, val);
                break;

            case Direct :
                direct_push(destTableKey, destObjectKey, val);
                break;

            case Buffering :
                msg.dest_table = destTableKey;
                msg.dest_object = destObjectKey; 
                msg.src_table = myTableKey;
                msg.src_object = myItemKey;
                msg.value = val;
                out_push_buffer.push_back(msg);
                break;
            
            case BufferingWorker :
                msg.dest_table = destTableKey;
                msg.dest_object = destObjectKey; 
                msg.src_table = myTableKey;
                msg.src_object = myItemKey;
                msg.value = val;
                (*parent_dist_worker)->out_push_buffer.push_back(msg);
                break;
            case GasnetDirect :
                perform_direct_remote_push(*(parent_dist_worker), destTableKey, destObjectKey, val);
                break;

        }
    }

    //Add pull message to outgoing buffer
    void pull(TableKey_T destTableKey, ItemKey_T destObjectKey, int tag)
    {
        Message<TableKey_T, ItemKey_T, Msg_T> msg;
        msg.dest_table = destTableKey;
        msg.dest_object = destObjectKey; 
        msg.src_table = myTableKey;
        msg.src_object = myItemKey;
        msg.tag = tag;
        msg.seqnum = next_seqnum;
        next_seqnum += 1;
        out_pull_buffer.push_back(msg);
    }

    void pull(TableKey_T destTableKey, ItemKey_T destObjectKey)
    {
        Message<TableKey_T, ItemKey_T, Msg_T> msg;
        msg.dest_table = destTableKey;
        msg.dest_object = destObjectKey; 
        msg.src_table = myTableKey;
        msg.src_object = myItemKey;
        msg.tag = 0;
        msg.seqnum = next_seqnum;
        next_seqnum += 1;
        out_pull_buffer.push_back(msg);
    }

    void create_ordered_buffer()
    {
        returning_pull_buffer.clear();
        for(int i = 0; i < next_seqnum; i++)
        {
            Message<TableKey_T, ItemKey_T, Msg_T> msg;
            returning_pull_buffer.push_back(msg);
        }
    }

    void add_returning_pull_buffer(Message<TableKey_T, ItemKey_T, Msg_T> msg)
    {
        if((*parent_dist_worker)->ordered_pulls == true)
        {
            returning_pull_buffer[msg.seqnum] = msg;
        }
        else
        {
            returning_pull_buffer.push_back(msg);
        }
    }

    void apply_returning_pull_buffer()
    {
        for(int i = 0; i < returning_pull_buffer.size(); i++)
        {

            auto msg = returning_pull_buffer[i];
            returning_pull(msg);
        }

        returning_pull_buffer.clear();
    }

    void apply_push_buffer()
    {
        while (!in_push_buffer.empty())
        {
            auto msg = in_push_buffer.back();
            foreign_push(msg);
            in_push_buffer.pop_back();
        }
    }

    void clear_outgoing_buffers()
    {
        out_pull_buffer.clear();
        out_push_buffer.clear();
        next_seqnum = 0;
    }


    /**
     * Called when the object is created AND when the table container attempts to create an identical object
     */
    virtual void refresh()
    {
        
    }

    /**
     * Called when the object is created. Only called once.
     */
    virtual void on_create()
    {
        //std::cout << "base object on_create" << std::endl;
    }

    //Called when something is pushed to this object
    virtual void foreign_push(Msg_T val)
    {

    }

    //Called when something is pulled from this object
    //potential TODO: add incoming message as argument
    virtual Msg_T foreign_pull(int tag)
    {
        return value;
    }

    //Called when a pull originating at this object completes
    virtual void returning_pull(Message<TableKey_T, ItemKey_T, Msg_T> returning_message)
    {

    }
    
    virtual void do_work()
    {
        //std::cout << "doing work from "<< myTableKey << std::endl;
    }

    virtual void finishing_work()
    {

    }


};


}//end namespace


#endif