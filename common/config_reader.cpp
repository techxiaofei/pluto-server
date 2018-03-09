#include "config_reader.h"

namespace pluto
{

ConfigReader::ConfigReader(const char* filename):buffer_(NULL)
{
	_LoadFile(filename);
}

ConfigReader::~ConfigReader()
{
	free(buffer_);
}

char* ConfigReader::GetAllConfig()
{
	return buffer_;
}

void ConfigReader::_LoadFile(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp)
	{
		printf("can not open %s,errno = %d\n", filename,errno);
		return;
	}

	fseek(fp, 0, SEEK_END);
	int size = (int)ftell(fp);
	rewind(fp);

	buffer_ = (char*)malloc(sizeof(char)*size);
	if (buffer_ == NULL)
	{
		printf("ConfigReader::_LoadFile buffer null\n");
		return;
	}

	int result = (int)fread(buffer_,1,size, fp);
	if (result != size)
	{
		printf("ConfigReader::_LoadFile size error:%d,%d\n",result,size);
	}

	fclose(fp);
	load_success_ = true;
}

}