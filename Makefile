ifndef LG_RT_DIR
$(error LG_RT_DIR variable is not defined, aborting build)
endif

JAVA_HOME = /usr/lib/jvm/java-openjdk

DEBUG ?= 1
OUTPUT_LEVEL ?= LEVEL_DEBUG
SHARED_LOWLEVEL ?= 1

NATIVE_JAVA_CLASSES = \
	org.legion.Runtime \
	org.legion.LegionObject \
	org.legion.Future \
	org.legion.Context \
	org.legion.TaskLauncher

NATIVE_INCLUDE = ./include

OUTFILE		:= liblegion_jni.so
GEN_SRC		:= native/runtime.cc
GEN_GPU_SRC	:=

CC_FLAGS	:= -fPIC
NVCC_FLAGS	:=
GASNET_FLAGS	:=
INC_FLAGS    := -I. -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
LD_FLAGS := -shared

###########################################################################
#
#   Don't change anything below here
#
###########################################################################

# General shell commands
SHELL	:= /bin/sh
SH	:= sh
RM	:= rm -f
LS	:= ls
MKDIR	:= mkdir
MV	:= mv
CP	:= cp
SED	:= sed
ECHO	:= echo
TOUCH	:= touch
MAKE	:= make
ifndef GCC
GCC	:= g++
endif
ifndef NVCC
NVCC	:= $(CUDA)/bin/nvcc
endif
SSH	:= ssh
SCP	:= scp

common_all : all

.PHONY	: common_all

# All these variables will be filled in by the runtime makefile
LOW_RUNTIME_SRC	:=
HIGH_RUNTIME_SRC:=
GPU_RUNTIME_SRC	:=
MAPPER_SRC	:=

include $(LG_RT_DIR)/runtime.mk

GEN_OBJS	:= $(GEN_SRC:.cc=.o)
LOW_RUNTIME_OBJS:= $(LOW_RUNTIME_SRC:.cc=.o)
HIGH_RUNTIME_OBJS:=$(HIGH_RUNTIME_SRC:.cc=.o)
MAPPER_OBJS	:= $(MAPPER_SRC:.cc=.o)
# Only compile the gpu objects if we need to
ifndef SHARED_LOWLEVEL
GEN_GPU_OBJS	:= $(GEN_GPU_SRC:.cu=.o)
GPU_RUNTIME_OBJS:= $(GPU_RUNTIME_SRC:.cu=.o)
else
GEN_GPU_OBJS	:=
GPU_RUNTIME_OBJS:=
endif

ALL_OBJS	:= $(GEN_OBJS) $(GEN_GPU_OBJS) $(LOW_RUNTIME_OBJS) $(HIGH_RUNTIME_OBJS) $(GPU_RUNTIME_OBJS) $(MAPPER_OBJS)

all: java
	$(MAKE) $(OUTFILE)

# If we're using the general low-level runtime we have to link with nvcc
$(OUTFILE) : $(ALL_OBJS)
	@echo "---> Linking objects into one binary: $(OUTFILE)"
ifdef SHARED_LOWLEVEL
	$(GCC) -o $(OUTFILE) $(ALL_OBJS) $(LD_FLAGS) $(GASNET_FLAGS)
else
	$(NVCC) -o $(OUTFILE) $(ALL_OBJS) $(LD_FLAGS) $(GASNET_FLAGS)
endif

$(GEN_OBJS) : %.o : %.cc
	$(GCC) -o $@ -c $< $(INC_FLAGS) $(CC_FLAGS)

$(LOW_RUNTIME_OBJS) : %.o : %.cc
	$(GCC) -o $@ -c $< $(INC_FLAGS) $(CC_FLAGS)

$(HIGH_RUNTIME_OBJS) : %.o : %.cc
	$(GCC) -o $@ -c $< $(INC_FLAGS) $(CC_FLAGS)

$(MAPPER_OBJS)	: %.o : %.cc
	$(GCC) -o $@ -c $< $(INC_FLAGS) $(CC_FLAGS)

$(GEN_GPU_OBJS) : %.o : %.cu
	$(NVCC) -o $@ -c $< $(INC_FLAGS) $(NVCC_FLAGS)

$(GPU_RUNTIME_OBJS): %.o : %.cu
	$(NVCC) -o $@ -c $< $(INC_FLAGS) $(NVCC_FLAGS)

clean:
	@$(RM) -rf $(ALL_OBJS) $(OUTFILE)

ARCH := $(shell getconf LONG_BIT)
LEGION_JAR = legionjni-linux$(ARCH).jar

java:
	javac org/legion/*.java
	javah -d $(NATIVE_INCLUDE) -jni $(NATIVE_JAVA_CLASSES)

jar: all
	jar -cf $(LEGION_JAR) org/legion/*.class liblegion_jni.so

hello: jar
	javac -cp $(LEGION_JAR) LegionHelloWorld.java
	java -cp $(LEGION_JAR):. -Xcheck:jni LegionHelloWorld