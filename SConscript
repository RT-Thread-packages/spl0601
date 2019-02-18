from building import *
Import('rtconfig')

cwd     = GetCurrentDir()
src	= Glob('*.c')
path = [cwd]

group = DefineGroup('spl0601', src, depend = ['PKG_USING_SPL0601'], CPPPATH = path)

Return('group')
