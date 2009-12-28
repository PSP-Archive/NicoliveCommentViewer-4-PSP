TARGET = NCPS

#cpcnv functions(thanks to yreeen!)
OBJS += Libs/cpcnv/liblzr.o
OBJS += Libs/cpcnv/cpcnv.o
OBJS += Libs/cpcnv/sjis/multibytecharcheck.o
OBJS += Libs/cpcnv/sjis/sjisstrlen.o
OBJS += Libs/cpcnv/sjis/sjisfromucs2.o
OBJS += Libs/cpcnv/sjis/sjistoucs2.o
OBJS += Libs/cpcnv/utf8/utf8toucs2.o
OBJS += Libs/cpcnv/utf8/utf8fromucs2.o
OBJS += Libs/cpcnv/utf8/utf8strlen.o
OBJS += Libs/cpcnv/ucs2/ucs2strlen.o
OBJS += Libs/intraFont/intraFont.o 
OBJS += Libs/intraFont/libccc.o

OBJS += Libs/NetworkClass/NetworkClass.o
OBJS += Libs/NicoAPI/NicoAPI.o

OBJS += Libs/GUListView/GUListView.o 
OBJS += Libs/GUDiag/GUDiag.o 
OBJS += Libs/GUDiag/GUDiag_osk.o 
OBJS += Libs/GUMenu/GUMenu.o 
OBJS += Libs/GUDraw/GUDraw.o

OBJS += dialogs.o gu.o
OBJS += net.o
OBJS += main.o


INCDIR =
CFLAGS = -G0 -Wall -O3
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =

LIBS = -lpspgu -lm -lpsputility -lpsprtc -lpspctrl -lstdc++
LIBS += -lssl -lcrypto

PSP_FW_VERSION = 371

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = NCPS

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak