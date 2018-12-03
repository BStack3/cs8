TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    stokenizer.cpp \
    table_util.cpp \
    token.cpp \
    ftokenizer.cpp \
    sql.cpp

HEADERS += \
    stokenizer.h \
    table_util.h \
    constants.h \
    token.h \
    arrayutil.h \
    bplustree.h \
    map.h \
    multimap.h \
    sql.h \
    ftokenizer.h \
    table.h \
    record.h \
    set.h \
    myqueue.h \
    mylist.h \
    mystack.h \
    vartype.h \
    selection.h
