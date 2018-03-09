#include "cjson_helper.h"

JsonHelper::JsonHelper(string& strJson)
{
	m_json = NULL;
	b_parse = false;
	m_str_json = strJson;			
}

JsonHelper::JsonHelper(const char* charJson)
{
	m_json = NULL;
	b_parse = false;
	m_str_json = charJson;
}

JsonHelper::JsonHelper()
{
	m_json = cJSON_CreateObject();
	if (m_json == NULL)
	{
		b_parse = false;
	}
	else
	{
		b_parse = true;
	}
	m_str_json = "";
}

JsonHelper::~JsonHelper()
{
	if(m_json)
	{
		cJSON_Delete(m_json);
	}  

}

bool JsonHelper::_ParseGet()
{
	m_json = cJSON_Parse(m_str_json.c_str());
	b_parse = true;
	return b_parse;
}

bool JsonHelper::_ParseSet()
{
	m_json = cJSON_CreateObject();
	b_parse = true;
	return b_parse;
}


bool JsonHelper::GetJsonItem(const char* node_name, string & result)
{
	result = "";
	if (!b_parse)
	{
		_ParseGet();		//没分析 就分析一次
	}
	if(m_json == NULL)
	{
		return false;
	}
	cJSON* obj_find = cJSON_GetObjectItem(m_json, node_name);

	if(obj_find == NULL)
	{
		return false;
	}
	result = obj_find->valuestring;
	return true;
}

bool JsonHelper::GetJsonItem(const char* node_name, char** result)
{
	if (!b_parse)
	{
		_ParseGet();		//没分析 就分析一次
	}
	if(m_json == NULL)
	{
		return false;
	}
	cJSON* obj_find = cJSON_GetObjectItem(m_json, node_name);

	if(obj_find == NULL)
	{
		return false;
	}
	*result = obj_find->valuestring;
	return true;
}


bool JsonHelper::GetJsonItem(const char* node_name, int & result)
{
	result = 0;
	if (!b_parse)
	{
		_ParseGet();		//没分析 就分析一次
	}
	if(m_json == NULL)
	{
		return false;
	}
	cJSON* obj_find = cJSON_GetObjectItem(m_json, node_name);

	if(obj_find == NULL)
	{
		return false;
	}
	result = obj_find->valueint;
	return true;

}

bool JsonHelper::GetJsonItem2(const char* node1, const char* node2, string & result)
{
	result = "";
	if (!b_parse)
	{
		_ParseGet();		//没分析 就分析一次
	}

	if(m_json == NULL)
	{
		return false;
	}
	cJSON* obj1 = cJSON_GetObjectItem(m_json, node1);

	if(obj1 == NULL)
	{
		return false;
	}
	cJSON* obj2 = cJSON_GetObjectItem(obj1, node2);
	if(obj2 == NULL)
	{
		return false;
	}

	result = obj2->valuestring;
	return true;
}

bool JsonHelper::GetJsonItem2(const char* node1, const char* node2, int & result)
{
	result = 0;

	if (!b_parse) 
	{
		_ParseGet();		//没分析 就分析一次
	}

	if(m_json == NULL)
	{
		return false;
	}
	cJSON* obj1 = cJSON_GetObjectItem(m_json, node1);

	if(obj1 == NULL)
	{
		return false;
	}
	cJSON* obj2 = cJSON_GetObjectItem(obj1, node2);
	if(obj2 == NULL)
	{
		return false;
	}

	result = obj2->valueint;
	return true;
}

bool JsonHelper::SetJsonItem(const char* node_name, string& result)
{
	if (!b_parse)
	{
		_ParseSet();
	}
	if (m_json == NULL)
	{
		return false;
	}

	cJSON_AddStringToObject(m_json, node_name, result.c_str());
	return true;
}

bool JsonHelper::SetJsonItem(const char* node_name, char* result)
{
	if (!b_parse)
	{
		_ParseSet();
	}
	if (m_json == NULL)
	{
		return false;
	}

	cJSON_AddStringToObject(m_json, node_name, result);
	return true;
}

bool JsonHelper::SetJsonItem(const char* node_name, int result)
{
	if (!b_parse)
	{
		_ParseSet();
	}
	if (m_json == NULL)
	{
		return false;
	}

	cJSON_AddNumberToObject(m_json, node_name, result);
	return true;
}

const string& JsonHelper::Json2Str()
{	
	if (m_str_json != "")
	{
		return m_str_json;
	}

	if (m_json == NULL)
	{
		return NULL;
	}

	m_str_json = cJSON_PrintUnformatted(m_json);
	return m_str_json;
}

char* JsonHelper::Json2Char()
{
	return cJSON_PrintUnformatted(m_json);
}
