

######
### [required]
### Application name. Used as output file name.
###
APP_NAME = unit_test

######
### [required if $(PLATFORM) is xcore]
### xcore target device
###
TARGET_DEVICE = XCORE-AI-EXPLORER

######
### [optional] List of libraries on which this application depends
###
DEPENDENCIES := lib_xs3_math Unity testing

######
### [currently required]
### Paths to the dependencies, because we don't have a way of searching
### for them at the moment.
###
lib_xs3_math_PATH := ../../lib_xs3_math
Unity_PATH := ../../../Unity
testing_PATH := ../shared/testing

## Uncomment to tell lib_xs3_math not to compile .S files (i.e. test C implementations)
# lib_xs3_math_NO_ASM := 1

###### 
### [optional] Directories, relative to the application project, to look for source files.
###            defaults to nothing.
SOURCE_DIRS := src

###### 
### [optional] Source file extentions. Defaults to: c cc xc cpp S
###
# SOURCE_FILE_EXTS := c cc xc cpp S

######
### [optional] List of source files to compile.
###            
# SOURCE_FILES :=  $(wildcard src/*.c src/*.xc)

######
### [optional] list of static libraries that
### should be linked into the executable
###
# LIBRARIES := foo.a

# If the application makefile sets this to any value other than 1, no static 
# libraries (.a files) will be created for dependencies, and the executable
# will be linked directly against library object files.
#
BUILD_STATIC_LIBRARIES := 0

c_FLAGS += -Wno-unused-function
c_FLAGS += -DUNITY_USE_COMMAND_LINE_ARGS
LDFLAGS += -fcmdline-buffer-bytes=200

ifdef QUICK_TEST
c_FLAGS += -DQUICK_TEST=1
endif

######
### [required]
### Default make target. Use this for a project description?
###
app_help:
	$(info *********************************************************)
	$(info unit_test: Unit tests for the functions in lib_xs3_math  )
	$(info                                                          )
	$(info make targets:                                            )
	$(info |   help:    Display this message (default)              )
	$(info |    all:    Build application                           )
	$(info |  clean:    Remove build files and folders from project )
	$(info |    run:    Run the unit tests in xsim                  )
	$(info *********************************************************)


#####################################
### Application-specific targets
#####################################

run: build
	xsim $(APP_EXE_FILE)