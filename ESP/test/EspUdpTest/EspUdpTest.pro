QT += testlib network
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_espudptest.cpp



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/EspUdp/release/ -lEspUdp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/EspUdp/debug/ -lEspUdp
else:unix: LIBS += -L$$OUT_PWD/../../lib/EspUdp/ -lEspUdp

INCLUDEPATH += $$PWD/../../lib/EspUdp
DEPENDPATH += $$PWD/../../lib/EspUdp

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/EspUdp/release/libEspUdp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/EspUdp/debug/libEspUdp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/EspUdp/release/EspUdp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/EspUdp/debug/EspUdp.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../lib/EspUdp/libEspUdp.a
