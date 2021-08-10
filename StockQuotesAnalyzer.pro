QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        atrdialog.cpp \
        candlebar.cpp \
        customproxymodel.cpp \
        dealline.cpp \
        graphicslinepolygon.cpp \
        main.cpp \
        mainwindow.cpp \
        mymodel.cpp \
        newscene.cpp \
        resultsmodel.cpp

HEADERS += \
        atrdialog.h \
        candlebar.h \
        customproxymodel.h \
        dealline.h \
        graphicslinepolygon.h \
        mainwindow.h \
        mymodel.h \
        newscene.h \
        resultsmodel.h

FORMS += \
        atrdialog.ui \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    gbp_usd.qrc \
    pics.qrc
