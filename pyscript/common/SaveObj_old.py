# -*- coding: utf-8 -*-
from common.Sql import getSqlPatternUp, getSqlPatternInst, getSqlPatternSel, getSqlPatternDel
from common import SqlExec
from common import Logger
import json
from server import GameServerRepo

def dumps(d):
    return json.dumps(d)

def loads(s):
    return json.loads(s)

class SaveObj(object):
    def __init__(self):
        self._dbdata = {}
        self._dirty = False
        self.tick_num = 0

    def isDirty(self):
        return self._dirty
    
    def OnTick(self):
        self.tick_num += 1
        if self.tick_num >= GameServerRepo.persistent_time:
            self.tick_num = 0
            if self._dirty is True:
                self._saveToDb()

    def destroy(self):
        if self._dirty is True:
            self._saveToDb()
    
    @classmethod
    def createSqlStatement(cls):
        cls._sqlPatternUp = getSqlPatternUp(cls._table, cls._keycols, cls._cols)
        cls._sqlPatternInst = getSqlPatternInst(cls._table, cls._keycols, cls._cols)
        cls._sqlPatternSel = getSqlPatternSel(cls._table, cls._keycols, cls._cols)
        cls._sqlPatternDel = getSqlPatternDel(cls._table, cls._keycols)

    def getSqlUpdate(self):
        return self._sqlPatternUp%tuple(self.getColumnsData(self._keycols,self.getColumnsData(self._cols)))

    def getSqlInsert(self):
        return self._sqlPatternInst%tuple(self.getColumnsData(self._cols,self.getColumnsData(self._keycols)))

    def getSqlSelect(self):
        return self._sqlPatternSel%tuple(self.getColumnsData(self._keycols))

    def getSqlDelete(self):
        return self._sqlPatternDel%tuple(self.getColumnsData(self._keycols))

    def getColumnsData(self, cols, data=None):
        data = data or []
        for attr, dbType in cols.iteritems():
            funcname = self._savefuncs.get(attr, None) or attr
            func = getattr(self, funcname)
            val = func()
            if dbType == "bld":
                val = dumps(val)
            data.append(val)
        return data

    def loadFromDb(self, data, callback):
        Logger.LogDebug("saveobj.loadFromDb:%s"%str(data))
        for attr, d in data.iteritems():
            if self._cols[attr] == "bld":
                d = loads(d)
            elif self._cols[attr] == "int":
                d = int(d)
            print attr,self._loadfuncs[attr]
            func = getattr(self, self._loadfuncs[attr])
            func(d)
        
        bSuccess = (True if data else False)
        if bSuccess:
            callback(self.id(), self)
            self.after_load(bSuccess)
        else:
            callback(self.id(), None)
    
    def queryDb(self, callback):
        Logger.LogDebug("saveobj.querydb")
        
        func = lambda data: self.loadFromDb(data, callback)
        
        SqlExec.SqlExec(self.getSqlSelect(), func)

    def _saveToDb(self):
        self._dirty = False
        SqlExec.SqlExec(self.getSqlUpdate())
        print self.getSqlUpdate()
    
    def saveToDb(self):
        if self._dirty:
            self._saveToDb()
        
    def createDb(self):
        SqlExec.SqlExec(self.getSqlInsert())
    
    def after_create(self):
        raise Exception("after create is not implement")
        
    def after_load(self, bSuccess):
        raise Exception("after load is not implement")


def saveclass(tblname, keycols={"id":"int"}, cols = {"data":"vch"}):
    def saveclass_(cls):
        def getAttrCreator(attr):
            def getAttr(self, default=None):
                return self._dbdata.get(attr, default)
            return getAttr
        def setAttrCreator(attr):
            def setAttr(self, val):
                self._dbdata[attr] = val
                self._dirty = True
            return setAttr
        def loadAttrCreator(attr):
            def loadAttr(self, val):
                self._dbdata[attr] = val
            return loadAttr
        loadfuncs = {}
        savefuncs = {}
        setattr(cls, "_table", tblname)
        setattr(cls, "_keycols", keycols)
        setattr(cls, "_cols", cols)
        setattr(cls, "_loadfuncs", loadfuncs)
        setattr(cls, "_savefuncs", savefuncs)
        for attr in keycols.keys():
            getfuncname = attr
            getfunc = getattr(cls, getfuncname, None) or getAttrCreator(attr)
            setattr(cls, getfuncname, getfunc)
            
            setfuncname = "set"+str.title(attr)
            setfunc = getattr(cls, setfuncname, None) or setAttrCreator(attr)
            setattr(cls, setfuncname, setfunc)
        for attr in cols.keys():
            getfuncname = attr
            getfunc = getattr(cls, getfuncname, None) or getAttrCreator(attr)
            setattr(cls, getfuncname, getfunc)
            
            setfuncname = "set"+str.title(attr)
            setfunc = getattr(cls, setfuncname, None) or setAttrCreator(attr)
            setattr(cls, setfuncname, setfunc)
            
            savefuncname = "save"+str.title(attr)
            savefunc = getattr(cls, savefuncname, None) or getattr(cls, getfuncname)
            setattr(cls, savefuncname, savefunc)
            savefuncs[attr] = savefuncname
            
            loadfuncname = "load"+str.title(attr)
            loadfunc = getattr(cls, loadfuncname, None) or loadAttrCreator(attr)
            setattr(cls, loadfuncname, loadfunc)
            loadfuncs[attr] = loadfuncname
            
        cls.createSqlStatement()
        return cls
    return saveclass_

#@bldoperator("data", ["map"])
def bldoperator(bld, attrlist):
    def bldop(cls):
        #def getDataAttr(self, attr, default):
        #    dct = self.data()
        #    return dct.get(attr, default)
        def get(self, attr, default=None):
            dct = getattr(self, bld)()
            return dct.get(attr, default)
        getbldattr = "get"+str.title(bld)+"Attr"
        if not getattr(cls, getbldattr, None):
            setattr(cls, getbldattr, get)

        #def map(self, default):
        #    return self.getDataAttr("map", default)
        def getAttrCreator(attr):
            def getAttr(self, default=None):
                return getattr(self, getbldattr)(attr, default)
            return  getAttr

        #def setDataAttr(self, attr, val):
        #    dct = getattr(self, bld)(self)
        #    dct[attr] = val
        #    return self.setData(dct)
        setbld = "set"+str.title(bld)
        def set(self, attr, val):
            dct = getattr(self, bld)()
            dct[attr] = val
            return getattr(self, setbld)(dct)
        setbldattr = "set"+str.title(bld)+"Attr"
        if not getattr(cls, setbldattr, None):
            setattr(cls, setbldattr, set)

        #def setMap(self, val):
        #    return self.setDataAttr("map", val)
        def setAttrCreator(attr):
            def setAttr(self, val):
                return getattr(self, setbldattr)(attr, val)
            return setAttr

        for attr in attrlist:
            getfuncname = attr
            if not getattr(cls, getfuncname, None):
                setattr(cls, getfuncname, getAttrCreator(attr))
            setfuncname = "set" + str.title(attr)
            if not getattr(cls, setfuncname, None):
                setattr(cls, setfuncname, setAttrCreator(attr))
        return cls
    return bldop

