## Process this file with automake to produce Makefile.in
noinst_LIBRARIES = libcommon.a libtui.a

libcommon_a_SOURCES = \
        common.cpp common.h \
	TimeHack.cpp TimeHack.h \
        Job.cpp Job.h \
        Log.cpp Log.h \
        Random.cpp Random.h \
        ReadJob.cpp ReadJob.h \
        WriteJob.cpp WriteJob.h \
        Transfer.cpp Transfer.h \
        TransferFactory.cpp TransferFactory.h \
        GarbageTransfer.cpp GarbageTransfer.h \
        NumbersTransfer.cpp NumbersTransfer.h \
        RandomTransfer.cpp RandomTransfer.h \
        BytePatternTransfer.cpp BytePatternTransfer.h \
	TransferInfo.cpp TransferInfo.h \
	TransferInfoList.cpp TransferInfoList.h \
	TransferInfoListFactory.cpp TransferInfoListFactory.h \
	RandomTransferInfoList.cpp RandomTransferInfoList.h \
	SequentialTransferInfoList.cpp SequentialTransferInfoList.h

libtui_a_SOURCES = \
        Tui.cpp Tui.h

LDADD = libcommon.a libtui.a

bin_PROGRAMS = spew
spew_SOURCES = spew.cpp \
               SpewDisplay.cpp SpewDisplay.h \
               SpewConsole.cpp SpewConsole.h \
               SpewTui.cpp SpewTui.h \
               SpewTuiWindow.cpp SpewTuiWindow.h \
               SpewTuiHelpWindow.cpp SpewTuiHelpWindow.h \
               SpewTuiProgressWindow.cpp SpewTuiProgressWindow.h \
               SpewTuiStatisticsWindow.cpp SpewTuiStatisticsWindow.h \
               SpewTuiStatusWindow.cpp SpewTuiStatusWindow.h

spew_LDFLAGS = $(STATIC_LINK)

install-exec-local:
	(cd $(DESTDIR)$(bindir) && $(LN_S) -f spew gorge)
	(cd $(DESTDIR)$(bindir) && $(LN_S) -f spew regorge)
