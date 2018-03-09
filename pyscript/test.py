from common.SaveObj_old import *

################test#########################
@saveclass("player",cols = {
    "name":"vch",
    "sex":"int",
    "data":"bld"})
@bldoperator("data", ["map", "pos"])
class Player(SaveObj):
    def __init__(self, id):
        super(Player, self).__init__()
        self.setId(id)

    def after_load(self, bSuccess):
        pass

p = Player(1)
p.setName("sss")
p.setSex(1)
p.setData({})
print p.getSqlUpdate()
print p.getSqlInsert()
print p.getSqlSelect()
print p.getSqlDelete()
p.setMap(1)
p.setPos((22,33))
print p.map()
print p.pos()
