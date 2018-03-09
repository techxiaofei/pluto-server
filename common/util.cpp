#include "util.h"

unsigned int hash_string(const char *str)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

void split(string& s, string separate, list<string>& ret)
{  
    size_t last = 0;  
    size_t index=s.find_first_of(separate,last);  
    while (index!=std::string::npos)  
    {  
        ret.push_back(s.substr(last,index-last));  
        last=index+1;  
        index=s.find_first_of(separate,last);  
    }  
    if (index-last>0)  
    {  
        ret.push_back(s.substr(last,index-last));  
    }  
}

int min_pow(int num)  
{  
    if (num<=1) return num;  
    if (num & (num-1))   
    {  
        unsigned int r=1;  
        while(num)  
        {  
            num>>=1;  
            r<<=1;  
        }  
        return r;  
    }  
    else  
    {  
        return num;  
    }     
} 