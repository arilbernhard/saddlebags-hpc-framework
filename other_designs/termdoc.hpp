using namespace std;

typedef struct TermDoc {
	string term;
	string doc;

	friend ostream &operator<<(ostream&, const TermDoc&);

	bool operator==(const TermDoc &other) const
	{
		if ((term.compare(other.term) == 0) && (doc.compare(other.doc) == 0)) {
			return 1;
		}
		return 0;
	}

} termdoc_t;