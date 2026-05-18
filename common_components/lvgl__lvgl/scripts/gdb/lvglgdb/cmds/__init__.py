import gdb

from .core import DumpObj
from .display import DumpDisplayBuf
from .draw import InfoDrawUnit
from .misc import InfoStyle, DumpCache
from .debugger import Debugger
from .drivers import Lvglobal

__all__ = []

# Отключить страницы нумерации и заполнить стек печати Python
gdb.execute("set pagination off")
gdb.write("set pagination off\n")
gdb.execute("set python print-stack full")
gdb.write("set python print-stack full\n")


# Отладчик
Debugger()

# Свалки
DumpObj()
DumpDisplayBuf()
DumpCache()

# Информация
InfoStyle()
InfoDrawUnit()

# Драйверы
Lvglobal()
