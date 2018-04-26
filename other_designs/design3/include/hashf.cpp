
#ifndef HASHF_CPP
#define HASHF_CPP

#include <string>
#include "cityhash/src/city.h"


int hashf(std::string key)
{
	return CityHash32((const char*)key.c_str(), (size_t)key.size());
}


int hashf(int key)
{
	return CityHash32((const char*)(&key), sizeof(key));
}


#endif