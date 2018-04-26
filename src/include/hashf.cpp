
#ifndef HASHF_CPP
#define HASHF_CPP

#include <string>
#include <vector>
#include "cityhash/src/city.h"


int c_abs(int input)
{
	if(input >= 0)
		return input;
	return input*-1;
}

int hashf(std::string key)
{
	return c_abs(CityHash32((const char*)key.c_str(), (size_t)key.size()));
}


int hashf(int key)
{
	return c_abs(CityHash32((const char*)(&key), sizeof(key)));
}

int hashf(std::vector<std::string> key)
{
	std::string tmp = "";

	for (auto s : key)
	{  
		tmp += s;
	}
	return hashf(tmp);
}


#endif