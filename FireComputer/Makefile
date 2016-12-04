GOALNAME=Release
DEBUG=0

#default goal is Release
ifeq ($(MAKECMDGOALS),)
    GOALNAME=Release
    DEBUG=0
endif

ifeq (Release,$(filter Release,$(MAKECMDGOALS)))
    GOALNAME=Release
    DEBUG=0
endif

ifeq (Debug,$(filter Debug,$(MAKECMDGOALS)))
    GOALNAME=Debug
    DEBUG=1
endif

INCLUDEDIRS+=-I./

LIBSDIR +=
LIBS += -lpthread

VPATH +=
	
LDFLAGS += -Wl,-Map=mymap.map

ifeq ($(DEBUG),0)
    LDFLAGS += -rdynamic
    CXXFLAGS += -ggdb -g -O0
else
    CXXFLAGS += -O3 -g -ggdb -rdynamic
endif

CXXFLAGS+=-D__STDC_FORMAT_MACROS

TARGET = testfire

all: Release Debug
Release: $(TARGET)
Debug: $(TARGET)

OBJ += FireComputer.o  ManagerSlat.o  SerialCommand.o  utils.o
DEPS = $(OBJ:.o=.d)

-include $(DEPS)

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ) $(LIBSDIR) ${LIBS} $(LDFLAGS)
	@if [ -d /tftp ]; then cp $(TARGET) /tftp/$(TARGET); fi

#-MM - do not add system header files
#-MG - assume missing header files will be generated
#-MP - add phony target for each dependency
#-MF - specify the file to write the dependency to
#-MT - specify the target in the dependency file
%.o: %.cpp
	$(CXX) -MMD -MT '$@ $*.d' $(CXXFLAGS) -I. $(INCLUDEDIRS) -Wall -O3 -c $< -o $@
	@cp $*.d $*.tmp
	@sed -e 's;#.*;;' -e 's;^[^:]*: *;;' -e 's; *\\$$;;' \
	    -e '/^$$/d' -e 's;$$; :;' < $*.tmp >> $*.d
	@rm $*.tmp


.PHONY: clean
clean: 
	rm -f $(TARGET) $(OBJ) $(DEPS)


