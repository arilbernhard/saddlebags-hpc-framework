#include <limits.h>
#include <typeinfo>

using namespace std;

uint32_t fast_range(uint32_t x, uint32_t N) {
  //return x % N;
  return ((uint64_t) x * (uint64_t) N) >> 32 ;
}

template <class K, class V>
class hashtable_entry {
    K key;
    V value;
    hashtable_entry<K,V> * next; //Used for separate chaining
};


template<class T, class T2> int key_compare(T a, T2 b)
{
    cout << "UNSUPPORTED TYPE INPUT FOR KEYCOMPARE FUNCTION" << endl;
    return 0;
}

template<>
int key_compare<string, string>(string a, string b) {
    return strcmp(a.c_str(), b.c_str());
}

template <class K, class V>
class HashTable {
    
    private:
    int initial_hashtable_size = 8;

    public:
    int max_separate_chain_depth = 4;
    int current_hashtable_size;
	hashtable_entry<K,V>** entries;

    HashTable()
    {
        entries = (hashtable_entry_t**)malloc(sizeof(hashtable_entry_t*) * initial_hashtable_size);
        for(int i = 0; i < initial_hashtable_size; i++)
            entries[i] = NULL;
    }


    void table_insert(K key, V value, int hashed_key)
    {

        //Get position in local table, hash is already calculated and passed as argument
        //uint32_t table_position = hashed_key % current_hashtable_size;
        uint32_t table_position = fast_range(hashed_key, current_hashtable_size);

        //Insert key/value as a new entry. If entry exists, insert after existing entries as a linked list
        hashtable_entry_t* current_entry = entries[table_position];

        hashtable_entry_t* prev_entry = NULL;
        int depth = 0;

        while(current_entry != NULL) {

            if(key_compare((*(current_entry->key)), key) == 0) {

                //Add new value
                current_entry->value = value;

                return;                
            }
            
            prev_entry = current_entry;
            depth += 1;
            current_entry = current_entry->next;
        }
        
        current_entry = new hashtable_entry();
        (*current_entry).key= new string(key);
        
        current_entry->next = NULL;
        current_entry->value = 1;

        if(prev_entry != NULL)
            prev_entry->next = current_entry;
        else
            (*db).entries[table_position] = current_entry;


        if(depth > (*db).max_separate_chain_depth && EXPANSION_ENABLED == 1)
            expand_hashtable(db);

    }

};


