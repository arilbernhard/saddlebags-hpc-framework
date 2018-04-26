#include <upcxx/upcxx.hpp>
#include "cityhash/src/city.h"
#include <iostream>
#include <fstream>

using namespace std;

class Worker {

};

template<class W>
class Object {
public:
	int value;
	W* worker;

	void push(int to_push) {
		value += to_push;
	}

	int pull() {
		return value;
	}

    template<class K>
	void on_create(K key)
	{
		//cout << "base on create" << endl;
		return;
	}
};

/*int hashf(string key)
{
	return CityHash32((const char*)key.c_str(), key.size());
}*/

template<class T>int hashf( T key)
{
   cout << "UNSUPPORTED TYPE INPUT FOR HASH FUNCTION. Provide hash function to objectlist class" << endl;
   return 1;
}

template<>int hashf<string>(string key)
{
	return CityHash32((const char*)key.c_str(), key.size());
}

template <class W, class K, class V>
class ObjectList {
	public:
	unordered_map<K, V> mapped_objects;
	bool is_global;
	int home;
    int something = 3;
	W worker;

	int (*hash_func)(K) = hashf;

	ObjectList(bool global) {
		home = upcxx::rank_me();
        something = 4;
		is_global = global;
	}

	void push(K key, int to_push)
	{
		if(!is_global)
			return local_push(key, to_push);
		
		int target_partition = partition(hashf(key));
		if(target_partition == home)
			return local_push(key, to_push);
		return global_push(key, to_push, target_partition);
	}

	int partition(int hash)
	{
		return hash % upcxx::rank_n();
	}

	void local_push(K key, int to_push)
	{
		auto iterator = mapped_objects.find(key);
		if(iterator == mapped_objects.end())
		{
			auto newObj = V();
			newObj.value = to_push;
			//newObj.worker = worker;
			newObj.on_create(key);
			mapped_objects.insert({key, newObj});
			//cout << "inserting " << key << endl;
		}
		else {
			iterator->second.push(to_push);
		}
	}

	void global_push(K key, int to_push, int target_partition)
	{
		cout <<"partition" << home << " GLOBAL PUSH " << to_push << " to " << target_partition << endl;
		upcxx::rpc_ff(target_partition, [this](K k,int p) {
			local_push(k, p);
		}, key, to_push);
	}

	void print_map()
	{
		for (auto it : mapped_objects) 
			std::cout << " " << it.first << " " << it.second.value << endl;

		cout << upcxx::rank_me() << " Total " << mapped_objects.size() << " objects" << endl;
	}
};







