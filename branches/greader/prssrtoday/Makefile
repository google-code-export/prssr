#
# pRSSreader today plugin
#

OUTDIR=prj/ARMV4Rel
BIN=prssrtoday.dll
DEF=prssrtoday.def

VPATH=../prssr/:../prssr/ctrls/:../prssr/sha1/:../prssr/www/:../share
SRC=$(wildcard *.cpp)\
	../share/defs.cpp\
	../share/reg.cpp\
	../share/helpers.cpp\
	../share/file.cpp\
	../share/wnd.cpp\
	../share/UIHelper.cpp\
	../prssr/ctrls/CeListCtrl.cpp\
	../prssr/ctrls/CePropertySheet.cpp\
	../prssr/www/url.cpp\
	../prssr/sha1/sha1.cpp\
	../prssr/Site.cpp\
	../prssr/Feed.cpp
RC=prssrtoday.rc
LIB=

# -----------------------------------------------------------------------------

TMP_OBJ=$(addprefix $(OUTDIR)/, $(patsubst %.cpp,%.obj,$(notdir $(SRC))))
RES=$(addprefix $(OUTDIR)/, $(RC:.rc=.res))
OBJ=$(TMP_OBJ) $(RES)
ODIR=$(subst /,\/,$(OUTDIR))


# 300 for eVC 3.0
CEVersion=420
CePlatform=WIN32_PLATFORM_PSPC=400
CESubsystem=windowsce,4.20

CECrt=C
CECrtDebug=C
CECrtMT=C
CECrtMTDebug=C
CENoDefaultLib=libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib
CEx86Corelibc=corelibc.lib

RSC=/opt/wince-arm/bin/rc
CPP=/opt/wince-arm/bin/clarm
LINK=/opt/wince-arm/bin/link

CABWIZ=wine z:\\opt\\wince-arm\\wce400\\tools\\cabwiz.exe

# minimal include
INCLUDE=\
	/I z:\\opt\\wince-arm\\wce400\\include\\armv4 \
	/I z:\\opt\\wince-arm\\wce400\\mfc\\Include

LIB_PATH=\
	/LIBPATH:z:\\opt\\wince-arm\\wce400\\lib\\Armv4 \
	/LIBPATH:z:\\opt\\wince-arm\\wce400\\mfc\\Lib\\armv4

RSC_INCLUDE=\
	/i z:\\opt\\wince-arm\\wce400\\include\\armv4 \
	/i z:\\opt\\wince-arm\\wce400\\mfc\\Include

# debug
#RSC_FLAGS=/l 0x409 /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "DEBUG" /d "UNICODE" /d "_UNICODE" /d "$(CePlatform)" /d "ARM" /d "_ARM_" /d "ARMV4" /d "_AFXDLL" /d "AFX_TARG_$(LANG)" /r 
#CPP_FLAGS=/nologo /W3 /Zi /Od /D "DEBUG" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=$(CEVersion) /D _WIN32_WCE=$(CEVersion) /D "$(CePlatform)" /D "UNICODE" /D "_UNICODE" /D "_AFXDLL" /M$(CECrtMTDebug) /c 
#LINK32_FLAGS=/nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"wWinMainCRTStartup" /incremental:yes /debug /subsystem:$(CESubsystem) /align:"4096" /MACHINE:ARM 

# release
RSC_FLAGS=/l 0x409 /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "NDEBUG" /d "UNICODE" /d "_UNICODE" /d "$(CePlatform)" /d "ARM" /d "_ARM_" /d "ARMV4" /d "_AFXDLL" /d "AFX_TARG_$(LANG)" /r 
CPP_FLAGS=/nologo /W3 /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=$(CEVersion) /D _WIN32_WCE=$(CEVersion) /D "$(CePlatform)" /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "_AFXDLL" /D "_USRDLL" /D "PRSSR_TODAY" /O2 /M$(CECrtMT) /c 
LINK32_FLAGS=/nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /subsystem:$(CESubsystem) /align:"4096" /MACHINE:ARM /DEF:$(DEF)

## --- ##

all: dep $(OUTDIR) $(OUTDIR)/$(BIN)

$(OUTDIR):
	-mkdir "$(OUTDIR)"

# executable
$(OUTDIR)/$(BIN): $(OBJ)
	@echo "LINK  $@"
	@$(LINK) $(LINK32_FLAGS) $(LIB_PATH) /OUT:$@ $(OBJ) $(LIB)

# main files
$(OUTDIR)/%.obj: %.cpp
	@$(CPP) $(CPP_FLAGS) $(INCLUDE) /Fo$@ $<

$(OUTDIR)/%.res: %.rc
	@echo "RC    $@"
	@$(RSC) $(RSC_FLAGS) $(RSC_INCLUDE) /fo$@ $<

.PHONY: dep
dep:
	@$(CC) -MM -DDEPEND -D__AFXWIN_H__ -D_WIN32_WCE=$(CEVersion) $(SRC) | sed 's/^\([^ .]*\)\.o/$(ODIR)\/\1.obj/' > .dep

.PHONY: clean
clean:
	rm -f $(OUTDIR)/*.obj
	rm -f $(OUTDIR)/*.res
	rm -f $(OUTDIR)/$(BIN)

-include .dep
