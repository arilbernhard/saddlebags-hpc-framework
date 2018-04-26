
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

namespace lighght
{

template<typename TableKey, typename ObjectKey, typename MessageType>
class Message {

    public:        
        TableKey dest_table;
        ObjectKey dest_object;
        TableKey src_table;
        ObjectKey src_object;
        
        MessageType value;
        int tag;

        int seqnum;
};
} //end namespace

#endif