ifndef LIGHTTABLE_DIR
LIGHTTABLE_DIR = ../..
endif

SRC_FILES := $(wildcard *.cc)
INCLUDE_DIR = -I$(LIGHTTABLE_DIR)/src -I. 

SOURCE_FOR_CC = $(foreach source_file_1, $(SOURCES),$(filter %.cc, $(source_file_1)))

CXXFLAGS += -Werror -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter \
            -Wno-missing-field-initializers -Wmissing-declarations -fno-strict-aliasing \
            $(INCLUDE_DIR) -Wformat -Wformat-security -D_GNU_SOURCE \
            -fPIC -fno-stack-protector -fno-common -DNDEBUG -U_FORTIFY_SOURCE \
            -D_FORTIFY_SOURCE=2  -std=c++11

CXXFLAGS += -DYCSB_TEST

CXXFLAGS  += -Wl,-z,relro,-z,now -Wl,-z,noexecstack -lpthread -Wl,-z,relro,-z,now \
            -Wl,-z,noexecstack \
            -Wl,--whole-archive  -Wl,--no-whole-archive 
        

include $(KERNEL_ROOT)/compiler.mk
