Import('rtconfig')
from building import *

cwd = GetCurrentDir()
src	= Glob('*.c')

if rtconfig.PLATFORM == 'armcc':
    src += Glob('*_rvds.S')

if rtconfig.PLATFORM == 'gcc':
    src += Glob('*_gcc.S')

if rtconfig.PLATFORM == 'iar':
    src += Glob('*_iar.S')

CPPPATH = [cwd]

group = DefineGroup('Qfplib-M0-tiny', src, depend = ['PKG_USING_QFPLIB_M0_TINY'], CPPPATH = CPPPATH)

Return('group')
