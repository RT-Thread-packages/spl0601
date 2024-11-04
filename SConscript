from building import *
Import('rtconfig')

cwd     = GetCurrentDir()
src = ["spl06_01.c"]
path = [cwd]

if GetDepend('PKG_SPL0601_USING_SENSOR_V1'):
    src += ["spl0601_sensor_v1.c"]

group = DefineGroup('spl0601', src, depend = ['PKG_USING_SPL0601'], CPPPATH = path)

Return('group')
