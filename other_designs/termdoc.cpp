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


ostream &operator<<(ostream& stream, const TermDoc& obj){
	stream << "DOC: " << obj.doc << " | TERM: " << obj.term;
	return stream;
}

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