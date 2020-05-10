QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wrx
TEMPLATE = app
OBJECTS_DIR += obj

!greaterThan(QT_MAJOR_VERSION, 4): QMAKE_CXXFLAGS += -std=c++98

SOURCES += main.cpp\
           Options.cpp \
           ReceiveJob.cpp \
           Radio.cpp \
           Frequency.cpp \
           OpMode.cpp \
           RadioSetup.cpp \
           CaptureThread.cpp \
           SamplesBlock.cpp \
           RadioService.cpp \
           WaterfallWorker.cpp \
           WaveFile.cpp \
           Logger.cpp \
           SoundCard.cpp \
           WaterfallFFT.cpp \
           RGBLine.cpp \
           RGBLineFiFo.cpp \
           Decoder.cpp \
           DecoderThread.cpp \
           SoundDeviceInfo.cpp \
           SamplesFiFo.cpp \
           TextDecoderWorker.cpp \
           TextDecoderWorkerIntern.cpp \
           MainWindow.cpp \
           SchedTableModel.cpp \
           Settings.cpp \
           ComboBoxDelegate.cpp \
           WaterfallWidget.cpp \
           RadioPage.cpp \
           WaterfallHeader.cpp \
           BrowserPage.cpp \
           RadioSetupPage.cpp \
           SoundcardSelector.cpp \
           TableWidgetRS.cpp \
           JobOptions.cpp \
           ReceiveJobMessage.cpp \
           ReceiveJobInfoDlg.cpp \
           ReceiveJobMessagesDlg.cpp \
           ReceiveJobResultDlg.cpp \
    ReceiveJobResultModel.cpp

HEADERS  += Options.h \
            ReceiveJob.h \
            Radio.h \
            Frequency.h \
            OpMode.h \
            RadioSetup.h \
            CaptureThread.h \
            SamplesBlock.h \
            RadioService.h \
            WaterfallWorker.h \
            WaveFile.h \
            Logger.h \
            SoundCard.h \
            WaterfallFFT.h \
            RGBLine.h \
            WindowFunc.h \
            RGBLineFiFo.h \
            WaterfallInfo.h \
            Decoder.h \
            DecoderThread.h \
            SoundDeviceInfo.h \
            SamplesFiFo.h \
            TextDecoderWorker.h \
            TextDecoderWorkerIntern.h \
            MainWindow.h \
            SchedTableModel.h \
            Settings.h \
            ComboBoxDelegate.h \
            WaterfallWidget.h \
            RadioPage.h \
            WaterfallHeader.h \
            BrowserPage.h \
            RadioSetupPage.h \
            SoundcardSelector.h \
            TableWidgetRS.h \
            JobOptions.h \
            ReceiveJobMessage.h \
            ReceiveJobInfoDlg.h \
            ReceiveJobMessagesDlg.h \
            ReceiveJobResultDlg.h \
    ReceiveJobResultModel.h

FORMS    += MainWindow.ui \
            Settings.ui \
            RadioPage.ui \
            BrowserPage.ui \
            RadioSetupPage.ui \
            SoundcardSelector.ui \
            ReceiveJobInfoDlg.ui \
            ReceiveJobMessagesDlg.ui \
            ReceiveJobResultDlg.ui

LIBS += -lasound \
        -lfftw3f \
        -L. -L../wrx -lFSKDecoder \
        -lsamplerate

RESOURCES += resources/wrx.qrc

TRANSLATIONS = wrx_de.ts wrx_en.ts

isEmpty(QMAKE_LRELEASE) {
win32|os2:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
unix {
!exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease-qt5 }
} else {
!exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease }
}
}

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm
