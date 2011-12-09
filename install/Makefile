#
# Create installation files for pRSSreader
# Copyright (c) 2008, David Andrs
#

PRODUCT=pRSSreader
VERSION=1.4.7
INF=pRSSreader.inf

# On linux
#CABWIZ=wine c:\\Program\ Files\\Windows\ CE\ Tools\\wce420\\POCKET\ PC\ 2003\\Tools\\Cabwiz.exe
#ZIP=zip

# On windows
CABWIZ="c:\program files\windows ce tools\wce420\pocket pc 2003\tools\cabwiz.exe"
ZIP="c:\program files\unxutils\usr\local\wbin\zip.exe"

all: cab

cab: $(PRODUCT)-$(VERSION).cab

$(PRODUCT)-$(VERSION).cab: $(PRODUCT).ARM.CAB
	ren $(PRODUCT).ARM.CAB $(PRODUCT)-$(VERSION).cab

$(PRODUCT).ARM.CAB: $(INF)
	$(CABWIZ) $(INF) /cpu ARM || sleep 1

zip: $(PRODUCT)-$(VERSION).zip

$(PRODUCT)-$(VERSION).zip:
	$(ZIP) $(PRODUCT)-$(VERSION).zip $(PRODUCT)-$(VERSION).cab

dist:
	$(PRODUCT)-$(VERSION).cab

# TODO: localiztion


clean:
	rm -f *.DAT
	rm -f *.CAB
	rm -f *.zip
