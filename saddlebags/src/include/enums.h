#ifndef ENUMS_H
#define ENUMS_H

//SendingModes define the behaviour of outgoing messages from Items
/*
    Combining - messages to the same rank are combined into one message before sending
    Direct - messages are sent immediately, disregarding synchronization
    GasnetBuffering - messages are not buffered but sent immediately - synchronization is achieved by not calling upcxx::progress
    Buffering - messages are stored in buffers at both the sending and receiving end and processed in batches
*/

enum SendingMode {
    Combining,
    Direct,
    GasnetBuffering,
    Buffering
};


#endif