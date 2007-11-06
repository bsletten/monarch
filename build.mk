# Makefile to compile the Digital Bazaar C++ libraries

# Clean does not create a target file so this will
# make sure it always runs when typing "make clean"
.PHONY: clean

# base path
BASE_DIR = /work/src/dbcpp/dbcore/trunk

# Compiler
CC = g++

# Compiler flags:
# -g	include debug information
CFLAGS = -g -Wall $(INCLUDES)
PYTHON_INCLUDE = -I/usr/include/python2.4

# Archive builder
AR = ar
ARFLAGS = cr

# Library path
LIBS_DIR = $(BASE_DIR)/libs
LIBS = -L$(LIBS_DIR)

# Linker flags:
LDFLAGS = $(LIBS)

# H files
FIND_H = $(wildcard $(dir)/*.h)

# CPP files
FIND_CPP = $(wildcard $(dir)/*.cpp)

# Prefix for all MODULES
MODGROUP = db

# All modules
MODULES = rt modest util io crypto net data sql event test #logging

# All executables
EXES = maintest


# Module specific build rules

modules_MODLIBS = rt

modest_CFLAGS = -DMODEST_API_EXPORT

util_MODLIBS = rt

io_MODLIBS = util

crypto_MODLIBS = io
crypto_EXTRADEPS = $(BASE_DIR)/crypto/python/cppwrapper/_dbcrypto.so

net_MODLIBS = modest crypto
net_SUBDIRS = http

util_SUBDIRS = regex

data_SUBDIRS = xml mpeg id3v2 json
data_LIBS = expat

sql_SUBDIRS = sqlite3 mysql util
sql_LIBS = sqlite3

event_MODLIBS = modest

test_MODLIBS = rt sql


# exe rules

maintest_DIR = test
maintest_EXE = test.exe
maintest_MODLIBS = rt modest util io crypto net data sql event test #logging
maintest_SOURCES = main
maintest_LIBS = pthread crypto ssl expat sqlite3 mysqlclient


#
# Generic rules

# Builds all modules and binaries
#all: $(MODULES:%=lib$(MODGROUP)%) $(EXES)
#	@echo Make all finished.
all: all2

# Cleans all build and dist files
clean:
	@echo Cleaning all DB object and library files...
	rm -rf $(BUILD) $(DIST) $(LIBS_DIR) $(CLEANFILES)
	@echo Make clean finished.

define MODULE_template
# Include path
INCLUDES += -I$$(BASE_DIR)/$(1)/cpp

# The DB build directories
BUILD += $$(BASE_DIR)/$(1)/cpp/build

# The DB dist directories
DIST += $$(BASE_DIR)/$(1)/cpp/dist

# Individual DB libraries as make targets
# This will need to be changed for a windows build
$$(MODGROUP)_$(1)_LIB = $$(BASE_DIR)/$(1)/cpp/dist/lib$$(MODGROUP)$(1).a

# H files
$$(MODGROUP)_$(1)_H = $$(foreach dir,$$(BASE_DIR)/$(1)/cpp/$$(MODGROUP)/$(1) $$(foreach subdir,$$($(1)_SUBDIRS),$$(BASE_DIR)/$(1)/cpp/$$(MODGROUP)/$(1)/$$(subdir)),$$(FIND_H))
ALL_H += $$($$(MODGROUP)_$(1)_H)

# CPP files
$$(MODGROUP)_$(1)_CPP = $$(foreach dir,$$(BASE_DIR)/$(1)/cpp/$$(MODGROUP)/$(1) $$(foreach subdir,$$($(1)_SUBDIRS),$$(BASE_DIR)/$(1)/cpp/$$(MODGROUP)/$(1)/$$(subdir)),$$(FIND_CPP))
ALL_CPP += $$($$(MODGROUP)_$(1)_CPP)

# Object files
$$(MODGROUP)_$(1)_OBJS = $$($$(MODGROUP)_$(1)_CPP:$$(BASE_DIR)/$(1)/cpp/$$(MODGROUP)/$(1)/%.cpp=$$(BASE_DIR)/$(1)/cpp/build/%.o)

$$(BASE_DIR)/$(1)/cpp/dist/lib$$(MODGROUP)$(1).a: $$($$(MODGROUP)_$(1)_OBJS)
	$$(AR) $$(ARFLAGS) $$@ $$^ $$($(1)_LIBS:%=/usr/lib/lib%.a)

$$(BASE_DIR)/$(1)/cpp/dist/lib$$(MODGROUP)$(1).so: $$($$(MODGROUP)_$(1)_OBJS)
	$$(CC) $$(LIBS) -shared -o $$@ $$^ $$($(1)_MODLIBS:%=-l$$(MODGROUP)%) $$($(1)_LIBS:%=-l%)

$$(LIBS_DIR)/lib$$(MODGROUP)$(1).so: $$(BASE_DIR)/$(1)/cpp/dist/lib$$(MODGROUP)$(1).so
	@mkdir -p $$(LIBS_DIR)
	@cp $$< $$@

lib$$(MODGROUP)$(1): $$($(1)_MODLIBS:%=lib$$(MODGROUP)%) \
	$$(BASE_DIR)/$(1)/cpp/dist/lib$$(MODGROUP)$(1).a \
	$$(BASE_DIR)/$(1)/cpp/dist/lib$$(MODGROUP)$(1).so \
	$$(LIBS_DIR)/lib$$(MODGROUP)$(1).so \
	$$($(1)_EXTRADEPS)

# Builds object files
$$(BASE_DIR)/$(1)/cpp/build/%.o: $$(BASE_DIR)/$(1)/cpp/$$(MODGROUP)/$(1)/%.cpp $$($$(MODGROUP)_$(1)_H)
	@mkdir -p $$(BASE_DIR)/$(1)/cpp/build \
		$$($(1)_SUBDIRS:%=$$(BASE_DIR)/$(1)/cpp/build/%) \
		$$(BASE_DIR)/$(1)/cpp/dist
	$$(CC) $$(CFLAGS) -fPIC -o $$@ -c $$< $$($(1)_CFLAGS)
endef

$(foreach mod,$(MODULES),$(eval $(call MODULE_template,$(mod))))


define EXE_template
# The DB build directories
BUILD += $$(BASE_DIR)/$$($(1)_DIR)/cpp/build

# The DB dist directories
DIST += $$(BASE_DIR)/$$($(1)_DIR)/cpp/dist

# CPP files
ALL_CPP += $$($(1)_SOURCES:%=$$(BASE_DIR)/$$($(1)_DIR)/cpp/%.cpp)

$(1)_exe: $$(BASE_DIR)/$$($(1)_DIR)/cpp/dist/$$($(1)_EXE)

CHECK_EXES += $$(BASE_DIR)/$$($(1)_DIR)/cpp/dist/$$($(1)_EXE)

# Builds the binary
$$(BASE_DIR)/$$($(1)_DIR)/cpp/dist/$$($(1)_EXE): $$($(1)_SOURCES:%=$$(BASE_DIR)/$$($(1)_DIR)/cpp/build/%.o) $$($(1)_MODLIBS:%=lib$$(MODGROUP)%) $$($(1)_OBJS)
	$$(CC) $$(CFLAGS) -o $$@ $$< $$(foreach mod,$$($(1)_MODLIBS),$$($$(MODGROUP)_$$(mod)_LIB)) $$($(1)_LIBS:%=-l%)

# Builds object files
$$(BASE_DIR)/$$($(1)_DIR)/cpp/build/%.o: $$(BASE_DIR)/$$($(1)_DIR)/cpp/%.cpp $$(ALL_H)
	@mkdir -p $$(BASE_DIR)/$$($(1)_DIR)/cpp/build \
		$$(BASE_DIR)/$$($(1)_DIR)/cpp/dist
	$$(CC) $$(CFLAGS) -o $$@ -c $$< $$($(1)_CFLAGS)
endef

$(foreach exe,$(EXES),$(eval $(call EXE_template,$(exe))))

check: all2
	@for exe in $(CHECK_EXES); do \
		$$exe; \
	done

#
# Extra build rules

# Build DB cryptography wrapper for python
$(BASE_DIR)/crypto/python/cppwrapper/_dbcrypto.so: $(BASE_DIR)/crypto/python/cppwrapper/dbcryptoWrapper.o $(BASE_DIR)/crypto/python/cppwrapper/dbcrypto_wrapper.o $(db_crypto_LIB) $(db_io_LIB) $(db_rt_LIB) $(db_util_LIB)
	$(CC) $(LIBS) -shared -o $@ $^ -lpthread -lcrypto -lssl

$(BASE_DIR)/crypto/python/cppwrapper/dbcryptoWrapper.o: $(BASE_DIR)/crypto/python/cppwrapper/dbcryptoWrapper.cpp
	$(CC) $(CFLAGS) -fPIC -o $@ -c $^

# Build DB cryptography swig wrapper for python
$(BASE_DIR)/crypto/python/cppwrapper/dbcrypto_wrapper.o: $(BASE_DIR)/crypto/python/cppwrapper/dbcrypto.i
	swig -c++ -python -o $(BASE_DIR)/crypto/python/cppwrapper/dbcrypto_wrapper.c $<
	$(CC) $(CFLAGS) $(PYTHON_INCLUDE) -fPIC -o $@ -c $(BASE_DIR)/crypto/python/cppwrapper/dbcrypto_wrapper.c

CLEANFILES += \
	$(BASE_DIR)/crypto/python/cppwrapper/dbcryptoWrapper.o \
	$(BASE_DIR)/crypto/python/cppwrapper/dbcrypto_wrapper.o \
	$(BASE_DIR)/crypto/python/cppwrapper/dbcrypto_wrapper.c \
	$(BASE_DIR)/crypto/python/cppwrapper/_dbcrypto.so \
	$(BASE_DIR)/crypto/python/cppwrapper/dbcrypto.py

$(BASE_DIR)/dbcore.pc: $(BASE_DIR)/dbcore.pc.in
	cat $< | \
		sed -e 's/@@CFLAGS@@/$(subst /,\/,$(INCLUDES))/' | \
		sed -e 's/@@LIBS@@/$(subst /,\/,$(addprefix -L,$(DIST)))/' > $@

CLEANFILES += \
	$(BASE_DIR)/dbcore.pc

TAGS: $(ALL_H) $(ALL_CPP)
	etags $^

all2: $(MODULES:%=lib$(MODGROUP)%) $(EXES:%=%_exe) $(BASE_DIR)/dbcore.pc
	@echo Make all finished.
