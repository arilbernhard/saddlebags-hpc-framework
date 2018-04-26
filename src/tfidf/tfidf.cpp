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
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "../include/timer.cpp"
#include "../include/lighght.hpp"


enum TableId { Term, Doc, TermDoc };

template<class Tk, class Ok, class Mt>
class TermDocObject : public lighght::Item<Tk, Ok, Mt> {
    public:
    float term_frequency = 0;
    float inv_doc_frequency = 0;
    float occurences = 0;

    void on_create() override {

        this->push(Term, {this->myItemKey[0]}, 1);
    }

    void refresh() override {
        this->occurences += 1;
    }

    void do_work() override {
        this->pull(Term, {this->myItemKey[0]});
        this->pull(Doc, {this->myItemKey[1]});
    }

    void returning_pull(lighght::Message<Tk, Ok, Mt> returning_message) override {
        if(returning_message.src_table == Doc) {
            term_frequency = this->occurences / (float)returning_message.value;
        }
        else if(returning_message.src_table == Term)
        {
            inv_doc_frequency = returning_message.value;
            this->value = term_frequency * inv_doc_frequency;
            //std::cout << "TFIDF of " << this->myObjectKey[0] << " in " << this->myObjectKey[1] << " = " << tfidf << std::endl;
        }
    }

};



template<class Tk, class Ok, class Mt>
class TermObject : public lighght::Item<Tk, Ok, Mt> {
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
class DocObject : public lighght::Item<Tk, Ok, Mt> {
    public:

    void on_create() override {
        if(this->myItemKey[0] == "Dudley_R._Herschbach")
        {
            std::cout << "=======================dudley CREATED==========" << std::endl;
        }
    }

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
    Benchmark_Timer read_timer;
    read_timer = Benchmark_Timer();


    upcxx::init();
    if(upcxx::rank_me() == 0)
        std::cout << "running with " << upcxx::rank_n() << " ranks" << std::endl;
    upcxx::barrier();

    using key_T = TableId;
    using value_T = std::vector<std::string>;
    using message_T = float;

    auto worker = lighght::create_worker<key_T, value_T, message_T>(Buffering);

    lighght::add_table<TermDocObject>(worker, TermDoc, false);
    lighght::add_table<TermObject>(worker, Term, true);
    lighght::add_table<Distributor, DocObject>(worker, Doc, false);

    std::ifstream file_list("../../datasets/filenames");
    
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

    int inserted_objects = 0;
    
    for(int i = 0; i < my_files_per_rank; i++) {
        std::string filename = "";
        file_list >> filename;

        std::string document_name = "../../datasets/wikidump/" + filename;

        std::ifstream infile(document_name);

        while(infile)
        {
            std::string word = "";
            infile >> word;
		    inserted_objects += 1;
            insert_object(worker, Doc, {filename});
            insert_object(worker, TermDoc, {word, filename});

        }
    }
    

    
    // Read file

    lighght::cycle(worker, false);
    lighght::cycle(worker, true);
    upcxx::barrier();
    read_timer.stop();
    if(upcxx::rank_me() == 0)
        read_timer.print();
	std::cout << upcxx::rank_me() << " -> inserted objects = " << inserted_objects << std::endl;

	upcxx::finalize();
}


