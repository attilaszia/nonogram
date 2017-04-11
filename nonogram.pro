# C++14
CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++14

LIBS += -lpng

SOURCES += \
    deduction.cc \
    line-solver-dynamic.cc \
    line-solver-fast.cc \
    line-solver.cc \
    line.cc \
    solver.cc \
    table.cc

HEADERS += \
    common.h \
    deduction.h \
    line-solver.h \
    line.h \
    solver.h \
    table.h
