#
# pRSSreader
#

LANG=ENU

# export variable to be visible in all makefiles
export LANG

SUBDIRS = prssrenc prssr prssrtoday prssrnot res096 res192 setup

all: unsvninfo subdirs

devel: svninfo subdirs


.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)
 
$(SUBDIRS):
	$(MAKE) -C $@

# svn
.PHONY: svninfo
svninfo:
	@echo SVNinfo
	@sed s/%Rev%/`svn info 2> /dev/null | grep Revision | cut -d \  -f 2`/ share/svninfo.h.linux > share/svninfo.h

.PHONY: unsvninfo
unsvninfo:
	@echo UnSVNinfo
	cp -f share/svninfo.h.clean share/svninfo.h

# dependency
prssr: prssrenc

# clean
.PHONY: clean
clean:
	$(MAKE) -C prssrenc clean
	$(MAKE) -C prssr clean
	$(MAKE) -C prssrtoday clean
	$(MAKE) -C prssrnot clean
	$(MAKE) -C res096 clean
	$(MAKE) -C res192 clean
	$(MAKE) -C setup clean

# Make distribution package
dist:
	$(MAKE) -C install

