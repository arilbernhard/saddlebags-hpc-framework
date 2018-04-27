
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

namespace saddlebags
{

template<typename TableKey, typename ObjectKey, typename MessageType>
class Message {

    public:        
        TableKey dest_table;
        ObjectKey dest_item;
        TableKey src_table;
        ObjectKey src_item;
        
        MessageType value;
        int tag;

        int seqnum;
};

} //end namespace

#endif