#include <upcxx/upcxx.hpp>
#include <iostream>
#include <fstream>
#include <string>

#include "include/data_object.hpp"
#include "include/worker.cpp"
#include "include/hashf.cpp"

/*
class SpecificObject : public DataObject {
    public:
    void hello_world() override {
        cout << "hi from inherited object" << endl;
    }
};
*/






enum TableId { Term, Doc, TermDoc };

template<class TableKey, class ObjectKey>
class TermDocObject : public DataObject<TableKey, ObjectKey> {
    public:

    int total_documents = 4;

    void on_create() override {
        //std::vector<std::string> term_vector = {this->myObjectKey[0]};
        //this->push(Term, term_vector, 1);
        std::cout << "on create termdoc object " << total_documents << std::endl;
    }

    void hello_world() override {
        std::cout <<" INHERITED HELLOWOLRD" << std::endl;
    }
};



template<class TableKey, class ObjectKey>
class TermObject : public DataObject<TableKey, ObjectKey> {
    public:
        int total_documents = 4;
};


int main(int argc, char* argv[]) {


    upcxx::init();

    using key_T = TableId;
    using value_T = std::vector<std::string>;

    auto worker = Worker<key_T, value_T>();

    worker.add_table<TermDocObject<key_T, value_T>>(Doc, false);

    value_T word_vector = {"heija"};

    worker.do_insert(Doc, word_vector);



    //worker.add_table<TermObject<TableId, std::vector<std::string>>>(Term, true);
    //worker.add_table<TermDocObject<TableId, std::vector<std::string>>>(TermDoc, false);


/*
    // Read file
	std::string document_name = "../example_documents/scanfile" + std::to_string(upcxx::rank_me()) + ".txt";
    std::ifstream infile(document_name);
    infile.seekg (0, infile.end);
    int file_length = infile.tellg();
    infile.seekg (0, infile.beg);

    std::vector<std::string> doc_vector = {document_name};

    while(infile)
    {
        if(infile.tellg() >= file_length)
            break;	

        std::string word = "";        
        infile >> word;
        std::vector<std::string> word_vector = {word};
        std::vector<std::string> termdoc_vector = {word, document_name};

        worker.do_insert(Doc, doc_vector);
        worker.do_insert(TermDoc, termdoc_vector);


        std::cout << word << std::endl;
        std::cout << worker.tables[Doc]->mapped_objects[doc_vector]->value << std::endl;

    }

    worker.cycle();

    std::cout << worker.tables[Doc]->mapped_objects[doc_vector]->value << std::endl;
*/
	upcxx::finalize();
}



/*
For each term in doc
	push 1 to termdoc(term, doc)

when termdoc is created:
	push 1 to TERM

push TERM COUNT to doc(doc)

for each TermDoc:
	pull doc to obtain Term Frequency

---------------

now we have:
termdoc -> number of times TERM occur in DOC
term -> number of unique documents with TERM
doc -> total number of terms in doc


termdoc pulls doc, now has number of occurences of term in doc, and total number of terms in doc
termdoc can now calculate term frequency

term knows the number of docs with a term and can calculate idf (IF IT KNOWS TOTAL NUMBER OF DOCS)

each termdoc pulls idf from term, and can calculate tf-idf





*/
