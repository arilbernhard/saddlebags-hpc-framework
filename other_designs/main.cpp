#include "upcsky.cpp"


using namespace std;


/* Define TermDoc type */

typedef struct TermDoc {
	string term;
	string doc;

	friend ostream &operator<<(ostream&, const TermDoc&);

	//Define equality operator for TermDoc type
	bool operator==(const TermDoc &other) const
	{
		if ((term.compare(other.term) == 0) && (doc.compare(other.doc) == 0)) {
			return 1; //if both document and term is equal, return 1
		}
		return 0;
	}

} termdoc_t;

//Define out-stream function for TermDoc type
ostream &operator<<(ostream& stream, const TermDoc& obj){
	stream << "DOC: " << obj.doc << " | TERM: " << obj.term;
	return stream;
}


//Define hash function for TermDoc type
namespace std {
	template <>
	struct hash<TermDoc>
	{
		std::size_t operator()(const TermDoc& k) const
		{
			string tmp = k.term + k.doc;
			return CityHash32((const char*)tmp.c_str(), tmp.size());
		}
	};

}

/* End of TermDoc type */

template<class W>
class TermDocObject : public Object<W> {
	public:
	void on_create(TermDoc key)
	{
		//(*worker).term.push(key.term, 1);
		//cout << this->worker->doc.is_global << " vss " << upcxx::rank_me() << endl;
		//cout << this->value << endl;
		//cout << "oncreate termdoc" << endl;
	}
};


class TFIDFWorker : public Worker {
public:
	ObjectList<upcxx::dist_object<TFIDFWorker>, string, Object<TFIDFWorker>> doc = ObjectList<upcxx::dist_object<TFIDFWorker>, string, Object<TFIDFWorker>>(false, this);
	ObjectList<upcxx::dist_object<TFIDFWorker>, string, Object<TFIDFWorker>> term = ObjectList<upcxx::dist_object<TFIDFWorker>, string, Object<TFIDFWorker>>(true, this);
	ObjectList<upcxx::dist_object<TFIDFWorker>, TermDoc, TermDocObject<TFIDFWorker>> termdoc = ObjectList<upcxx::dist_object<TFIDFWorker>, TermDoc, TermDocObject<TFIDFWorker>>(false, this);
	int hello = 0;
};








int main(int argc, char* argv[]) {
    upcxx::init();
	cout << "UPCXX::Init from rank " << upcxx::rank_me() << endl;

	upcxx::dist_object<TFIDFWorker> worker = upcxx::dist_object<TFIDFWorker>(TFIDFWorker());
	upcxx::barrier();
	//worker->termdoc->worker = worker;

	worker->hello = 123;


	string document_name = "scanfile" + to_string(upcxx::rank_me()) + ".txt";
    ifstream infile(document_name);
    infile.seekg (0, infile.end);
    int file_length = infile.tellg();
    infile.seekg (0, infile.beg);

    while(infile)
    {
        if(infile.tellg() >= file_length)
            break;	

        string word = "";        
        infile >> word;

		TermDoc td;
		td.term = word;
		td.doc = document_name;

		(*worker).termdoc.push(td, 1);
    }


	upcxx::barrier();

	cout << "first hello " << worker->hello << endl;
	cout << "second hello " << worker->termdoc.worker->hello << endl;
	cout << "third hello " << worker->termdoc.mapped_objects.begin()->second.worker->hello << endl;

	upcxx::barrier();
	
	upcxx::finalize();
    

    return 0;
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
term -> number of documents with TERM
doc -> total number of terms in doc


termdoc pulls doc, now has number of occurences of term in doc, and total number of terms in doc
termdoc can now calculate term frequency

term knows the number of docs with a term and can calculate idf (IF IT KNOWS TOTAL NUMBER OF DOCS)

each termdoc pulls idf from term, and can calculate tf-idf





*/
