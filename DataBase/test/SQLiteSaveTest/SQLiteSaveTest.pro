QT += testlib sql
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_sqlitesavetest.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/SQLiteSave/release/ -lSQLiteSave
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/SQLiteSave/debug/ -lSQLiteSave
else:unix: LIBS += -L$$OUT_PWD/../../lib/SQLiteSave/ -lSQLiteSave

INCLUDEPATH += $$PWD/../../lib/SQLiteSave
DEPENDPATH += $$PWD/../../lib/SQLiteSave

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/SQLiteSave/release/libSQLiteSave.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/SQLiteSave/debug/libSQLiteSave.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/SQLiteSave/release/SQLiteSave.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/SQLiteSave/debug/SQLiteSave.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../lib/SQLiteSave/libSQLiteSave.a
