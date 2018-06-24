/*
Term Frequency-Inverse Document Frequency algorithm

Documents are represented by their file-name in a "doc" table
Words are represented in a "term" table
A word's occurrence in a document is represented in a "termdoc" table

For each term in doc
	push 1 to termdoc(term, doc)
when termdoc is created:
	push 1 to TERM
push TERM COUNT to doc(doc)
for each TermDoc:
	pull doc to obtain Term Frequency

---------------

Now we have:
TermDoc: number of times TERM occur in DOC
Term: number of unique documents with TERM
Doc: total number of terms in Doc

1 TermDoc pulls Doc, now has number of occurences of term in Doc, and total number of terms in Doc
2 TermDoc can now calculate term frequency
3 Term knows the number of docs with a term and can calculate idf
4 Each TermDoc pulls idf from term, and can calculate tf-idf

This approach assumes that the total number of documents is known, and fixed
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "../include/saddlebags.hpp"


#define TERM_TABLE 0
#define DOC_TABLE 1
#define TERMDOC_TABLE 2



template<class Tk, class Ok, class Mt>
class TermDocObject : public saddlebags::Item<Tk, Ok, Mt> {
    public:
    float term_frequency = 0;
    float inv_doc_frequency = 0;
    float occurences = 0;

    void on_create() override {

        this->push(TERM_TABLE, {this->myItemKey[0]}, 1);
    }

    void refresh() override {
        this->occurences += 1;
    }

    void do_work() override {
        this->pull(TERM_TABLE, {this->myItemKey[0]});
        this->pull(DOC_TABLE, {this->myItemKey[1]});
    }

    void returning_pull(saddlebags::Message<Tk, Ok, Mt> returning_message) override {
        if(returning_message.src_table == DOC_TABLE) {
            term_frequency = this->occurences / (float)returning_message.value;
        }
        else if(returning_message.src_table == TERM_TABLE)
        {
            inv_doc_frequency = returning_message.value;
            this->value = term_frequency * inv_doc_frequency;
        }
    }

};



template<class Tk, class Ok, class Mt>
class TermObject : public saddlebags::Item<Tk, Ok, Mt> {
    public:
    Mt foreign_pull(int tag) override
    {
        return log((1036.0+1) / (this->value));
    }

    void foreign_push(float val) override {
        this->value += val;
    }
};

template<class Tk, class Ok, class Mt>
class DocObject : public saddlebags::Item<Tk, Ok, Mt> {
    public:

    void refresh() override {
        this->value += 1;
    }

    void foreign_push(Mt val) override {
        this->value += val;
    }

    Mt foreign_pull(int tag) override
    {
        return this->value;
    }

};


int main(int argc, char* argv[]) {
    upcxx::init();
    if(upcxx::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;
    
    /* 1 Create worker and tables */

    auto worker = saddlebags::create_worker<int, std::vector<std::string>, float>(Buffering);

    saddlebags::add_table<TermDocObject>(worker, TERMDOC_TABLE, false);
    saddlebags::add_table<TermObject>(worker, TERM_TABLE, true);
    saddlebags::add_table<DocObject>(worker, DOC_TABLE, false);




    /* 2 Designate filenames to partitions */

    std::ifstream file_list("example_dataset/filenames");
    std::string file_count_str = "";
    file_list >> file_count_str;
    int file_count = stoi(file_count_str);
    int base_files_per_rank = file_count / upcxx::rank_n();
    int my_files_per_rank = base_files_per_rank;

    if(upcxx::rank_me() == upcxx::rank_n() - 1)
    {
        my_files_per_rank += file_count - (base_files_per_rank * upcxx::rank_n());
    }

	std::cout << upcxx::rank_me() << " files per rank " << my_files_per_rank << std::endl;

    for(int i = 0; i < base_files_per_rank * upcxx::rank_me(); i++)
    {
        std::string filename = "";
        file_list >> filename;
    }



    /* 3 Insert objects*/

    int inserted_objects = 0;
    
    for(int i = 0; i < my_files_per_rank; i++) {
        std::string filename = "";
        file_list >> filename;

        std::string document_name = "example_dataset/wikidump/" + filename;

        std::ifstream infile(document_name);

        while(infile)
        {
            std::string word = "";
            infile >> word;
		    inserted_objects += 1;

            //For every word in a file:
            //1 insert an item with the document name (to count total number of words in document)
            //2 insert an item with term/document combination (to count unique occurrences of a term)

            insert_object(worker, DOC_TABLE, {filename});
            insert_object(worker, TERMDOC_TABLE, {word, filename});

        }
    }
    
    //Perform cycle without work (communication only) to create all items
    saddlebags::cycle(worker, false);

    //Perform cycle with work, to calculate tf-idf
    saddlebags::cycle(worker, true);
    
    
    upcxx::barrier();
    std::cout << upcxx::rank_me() << " -> inserted objects = " << inserted_objects << std::endl;

    upcxx::finalize();
}


