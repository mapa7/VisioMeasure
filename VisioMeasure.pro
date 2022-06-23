QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Additional libraries
INCLUDEPATH += /usr/local/include/opencv4
LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_imgcodecs -lopencv_videoio
LIBS += -lsnap7

SOURCES += \
    plcsiemens.cpp \
    dialoghelpdbread.cpp \
    dialoghelpdbwrite.cpp \
    main.cpp \
    mainwindow.cpp \
    detectionsystem.cpp \
    snap7.cpp

HEADERS += \
    plcsiemens.h \
    dialoghelpdbread.h \
    dialoghelpdbwrite.h \
    mainwindow.h \
    detectionsystem.h \
    snap7.h

FORMS += \
    mainwindow.ui \
    dialoghelpdbread.ui \
    dialoghelpdbwrite.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
