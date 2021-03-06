######################################################################
# Automatically generated by qmake (3.1) Thu Feb 11 17:32:53 2021
######################################################################

QT += core gui widgets multimedia charts
TEMPLATE = app
TARGET = COP4520_project_spectrograph
INCLUDEPATH += ./include
QMAKE_CXXFLAGS += -g

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += include/AudioFileStream.h \
           include/FFTWorkerThread.h \
           include/DistributedFFTWorkerThread.h \
           include/FFTUtils.h \
           include/SpectrographUI.h \
           include/SettingsDialog.h \
           include/Waveform.h \
           include/Spectrograph.h \
           include/Constants.h \
           include/FTController.h \
           include/DFTWorkerThread.h \
           include/DistributedDFTWorkerThread.h

SOURCES += src/main.cpp \
           src/AudioFileStream.cpp \
           src/FFTWorkerThread.cpp \
           src/DistributedFFTWorkerThread.cpp \
           src/FFTUtils.cpp \
           src/SpectrographUI.cpp \
           src/SettingsDialog.cpp \
           src/Waveform.cpp \
           src/Spectrograph.cpp \
           src/FTController.cpp \
           src/DFTWorkerThread.cpp \
           src/DistributedDFTWorkerThread.cpp

RESOURCES = Resource.qrc
