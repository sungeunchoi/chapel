
MAKEFLAGS = --no-print-directory

CHPL = chpl

TARGETS = \
	hello \
	hello2-module \
	hello3-datapar \
	hello4-datapar-dist \
	hello5-taskpar \
	hello6-taskpar-dist \

REALS = $(TARGETS:%=%_real)

default: all benchmarks primers programs

all: $(TARGETS)

clean: FORCE
	rm -f $(TARGETS) $(REALS)
	cd benchmarks && $(MAKE) clean
	cd primers && $(MAKE) clean
	cd programs && $(MAKE) clean

benchmarks: FORCE
	cd benchmarks && $(MAKE)

primers: FORCE
	cd primers && $(MAKE)

programs: FORCE
	cd programs && $(MAKE)

%: %.chpl
	$(CHPL) -o $@ $<

FORCE:
