
#ifndef MESSAGE_HPP
#define MESSAGE_HPP


template<typename TableKey, typename ObjectKey>
class Message {

    public:
        TableKey dest_table;
        ObjectKey dest_object;
        int value;
};



template<typename TableKey, typename ObjectKey>
class PullMessage : public Message<TableKey, ObjectKey>{
    public:
        TableKey src_table;
        ObjectKey src_object;
};


template<typename TableKey, typename ObjectKey>
class PushMessage : public Message<TableKey, ObjectKey>{


};

#endif