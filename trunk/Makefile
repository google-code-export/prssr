#
# pRSSreader
#

LANG=ENU

# export variable to be visible in all makefiles
export LANG

SUBDIRS = prssrenc prssr prssrtoday prssrnot res096 res192 setup

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)
 
$(SUBDIRS):
	$(MAKE) -C $@

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

