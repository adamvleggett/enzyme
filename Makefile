#
#   Makefile
#
#   Builds the Enzyme Hardware Access Library with gcc
#
#   Adam Leggett
#


srcdir = .
output = bin


vpath %.cpp $(srcdir)
vpath %.cpp $(srcdir)/linux

enzyme_obj += $(output)/enzyme_cpu.o
enzyme_obj += $(output)/enzyme_mem.o
enzyme_obj += $(output)/enzyme_pci.o
enzyme_obj += $(output)/enzyme_system.o

enzyme_obj += $(output)/enzyme_linuxkernel.o
enzyme_obj += $(output)/enzyme_linuxpci.o


CXXFLAGS += -MD
CXXFLAGS += -O3

ARFLAGS := rs


default: $(output) $(output)/libenzyme.a

$(output)/%.o: %.cpp Makefile
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(output)/libenzyme.a: $(enzyme_obj)
	$(AR) $(ARFLAGS) $@ $^

$(output):
	mkdir $(output)

clean:
	rm -rf $(output)
