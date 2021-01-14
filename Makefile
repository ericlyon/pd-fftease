# Makefile to build the 'fftease' library for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name
lib.name = fftease

# sources for the shared library
shared.sources = \
	bloscbank.c \
	convert.c \
	fft.c \
	fft4.c \
	fftease_setup.c \
	fftease_utilities.c \
	fold.c \
	leanconvert.c \
	leanunconvert.c \
	limit_fftsize.c \
	limited_oscbank.c \
	makewindows.c \
	oscbank.c \
	overlapadd.c \
	PenroseOscil.c \
	PenroseRand.c \
	power_of_two.c \
	unconvert.c \
	$(empty)

# sources for the objectclasses
class.sources = \
	bthresher~.c \
	burrow~.c \
	cavoc~.c \
	cavoc27~.c \
	centerring~.c \
	codepend~.c \
	cross~.c \
	dentist~.c \
	disarrain~.c \
	disarray~.c \
	drown~.c \
	enrich~.c \
	ether~.c \
	leaker~.c \
	mindwarp~.c \
	morphine~.c \
	multyq~.c \
	pileup~.c \
	pvcompand~.c \
	pvgrain~.c \
	pvharm~.c \
	pvoc~.c \
	pvtuner~.c \
	pvwarp~.c \
	pvwarpb~.c \
	reanimator~.c \
	resent~.c \
	residency~.c \
	residency_buffer~.c \
	schmear~.c \
	scrape~.c \
	shapee~.c \
	swinger~.c \
	taint~.c \
	thresher~.c \
	vacancy~.c \
	xsyn~.c \
	$(empty)

# extra files
datafiles = \
	$(wildcard fftease-helpfiles/*.pd) \
	LICENSE.txt \
	README.txt \
	fftease-meta.pd \
	lyonpotpourri-meta.pd \
	smap.pd \
	collect.pl \
	$(empty)
# extra dirs
datadirs = sound \
	$(empty)

# include the actual build-system
PDLIBBUILDER_DIR=pd-lib-builder
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
