# -*- coding: utf-8 -*-
DbType2Fmt = {
    "int" : "%d",
    "vch" : "\'%s\'",
    "bld" : "\'%s\'",
    "raw" : "%s",
}

def getSqlPatternUp(tbl, keys, cols):
    sql = "UPDATE %s SET %s WHERE %s"
    cl = map(lambda k:"%s=%s"%(k,DbType2Fmt[cols[k]]), cols)
    kl = map(lambda k:"%s=%s"%(k,DbType2Fmt[keys[k]]), keys)
    return sql%(tbl,",".join(cl), " AND ".join(kl))


def getSqlPatternInst(tbl, keys, cols):
    sql = "INSERT INTO %s (%s) VALUES (%s)"
    nl = map(None,keys)
    map(lambda k:nl.append(k), cols)
    fl = map(lambda k:DbType2Fmt[keys[k]], keys)
    map(lambda k:fl.append(DbType2Fmt[cols[k]]), cols)
    return sql%(tbl, ",".join(nl), ",".join(fl))

def getSqlPatternSel(tbl, keys, cols):
    sql = "SELECT %s FROM %s"
    s = ""
    if keys:
        kl = map(lambda k:"%s=%s"%(k,DbType2Fmt[keys[k]]), keys)
        s = " WHERE %s"%(" AND ".join(kl))
    cl = map(None, cols)
    return sql%(",".join(cl), tbl)+s

def getSqlPatternDel(tbl, keys):
    sql = "DELETE FROM %s WHERE %s"
    kl = map(lambda k:"%s=%s"%(k,DbType2Fmt[keys[k]]), keys)
    return sql%(tbl, " AND ".join(kl))

