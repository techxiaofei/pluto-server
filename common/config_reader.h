#ifndef __PLUTO_CONFIG_H__
#define __PLUTO_CONFIG_H__

#include <string>
#include <map>
#include "util.h"

namespace pluto
{

class ConfigReader
{
public:
	ConfigReader(const char* filename);
	~ConfigReader();

	char* GetAllConfig();
private:
	void _LoadFile(const char* filename);

	bool load_success_;
	char* buffer_;
	
};

}

#endif