# -*- coding: utf-8 -*-
__author = "majianfei"


def join_get_sql(tbl, key, value):
	sql = "SELECT id FROM %s WHERE %s = \'%s\'" % (tbl, key, value)
	return sql
	
def join_insert_sql(tbl, data):
	_keys = []
	_values = []
	
	for _key, _value in data.items():
		_keys.append(_key)
		_values.append(_value)
	
	_keys = "(%s)" % ",".join(_keys)
	_values = tuple(_values)
	sql = "INSERT INTO %s %s VALUES %s" % (tbl, _keys, _values)
	return sql
