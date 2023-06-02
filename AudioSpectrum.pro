QT       += core gui multimedia widgets opengl openglwidgets 3dcore 3dextras 3dinput 3drender

CONFIG += c++17

#include(3rdparty/fftreal)

SOURCES += \
    3rdparty/fftreal/fftreal_wrapper.cpp \
    3rdparty/fftreal/stopwatch/ClockCycleCounter.cpp \
    3rdparty/fftreal/stopwatch/StopWatch.cpp \
    main.cpp \
    mainwidget.cpp \
    engine.cpp \
    frequencyspectrum.cpp \
    spectrograph.cpp \
    sphere.cpp \
    scene.cpp \
    spectrumanalyser.cpp \
    utils.cpp

HEADERS += \
    3rdparty/fftreal/Array.h \
    3rdparty/fftreal/Array.hpp \
    3rdparty/fftreal/DynArray.h \
    3rdparty/fftreal/DynArray.hpp \
    3rdparty/fftreal/FFTReal.h \
    3rdparty/fftreal/FFTReal.hpp \
    3rdparty/fftreal/FFTRealFixLen.h \
    3rdparty/fftreal/FFTRealFixLen.hpp \
    3rdparty/fftreal/FFTRealFixLenParam.h \
    3rdparty/fftreal/FFTRealPassDirect.h \
    3rdparty/fftreal/FFTRealPassDirect.hpp \
    3rdparty/fftreal/FFTRealPassInverse.h \
    3rdparty/fftreal/FFTRealPassInverse.hpp \
    3rdparty/fftreal/FFTRealSelect.h \
    3rdparty/fftreal/FFTRealSelect.hpp \
    3rdparty/fftreal/FFTRealUseTrigo.h \
    3rdparty/fftreal/FFTRealUseTrigo.hpp \
    3rdparty/fftreal/OscSinCos.h \
    3rdparty/fftreal/OscSinCos.hpp \
    3rdparty/fftreal/def.h \
    3rdparty/fftreal/fftreal_wrapper.h \
    3rdparty/fftreal/stopwatch/ClockCycleCounter.h \
    3rdparty/fftreal/stopwatch/ClockCycleCounter.hpp \
    3rdparty/fftreal/stopwatch/Int64.h \
    3rdparty/fftreal/stopwatch/StopWatch.h \
    3rdparty/fftreal/stopwatch/StopWatch.hpp \
    3rdparty/fftreal/stopwatch/def.h \
    3rdparty/fftreal/stopwatch/fnc.h \
    3rdparty/fftreal/stopwatch/fnc.hpp \
    3rdparty/fftreal/test_fnc.h \
    3rdparty/fftreal/test_fnc.hpp \
    3rdparty/fftreal/test_settings.h \
    mainwidget.h \
    engine.h \
    frequencyspectrum.h \
    spectrograph.h \
    sphere.h \
    scene.h \
    spectrumanalyser.h \
    utils.h

TRANSLATIONS += \
    pro/AudioSpectrum_pl_PL.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SUBDIRS += \
    3rdparty/fftreal/fftreal.pro

DISTFILES += \
    3rdparty/fftreal/CMakeLists.txt \
    3rdparty/fftreal/FFTReal.dsp \
    3rdparty/fftreal/FFTReal.dsw \
    3rdparty/fftreal/bwins/fftrealu.def \
    3rdparty/fftreal/eabi/fftrealu.def \
    3rdparty/fftreal/fftreal.pas \
    3rdparty/fftreal/license.txt \
    3rdparty/fftreal/readme.txt \
    3rdparty/fftreal/testapp.dpr \
    pro/AudioSpectrum_pl_PL.qm

RESOURCES += \
    resources.qrc

