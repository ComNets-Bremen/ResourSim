#
# OMNeT++/OMNEST Makefile for Resoursim
#
# This file was generated with the command:
#  opp_makemake -f --deep
#

# Name of target to be created (-o option)
TARGET = Resoursim$(D)$(EXE_SUFFIX)
TARGET_DIR = .

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(QTENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)
#USERIF_LIBS = $(QTENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH =

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/src/BackgroundEventContainer.o \
    $O/src/BackgroundServiceInjector.o \
    $O/src/BaseResourceMode.o \
    $O/src/BucketBattery.o \
    $O/src/DeviceFsm.o \
    $O/src/EventManager.o \
    $O/src/OnOffDataset.o \
    $O/src/PhoneEventInjector.o \
    $O/src/SimpleAirplaneMode.o \
    $O/src/SimpleBluetooth.o \
    $O/src/SimpleCellular.o \
    $O/src/SimpleHighPowerCpu.o \
    $O/src/SimplePercentageBattery.o \
    $O/src/SimpleScreen.o \
    $O/src/SimpleWiFi.o \
    $O/src/SlidingDataset.o \
    $O/src/StatisticEntry.o \
    $O/src/background_messages/BackgroundEventEndMessage_m.o \
    $O/src/background_messages/BackgroundEventMessage_m.o \
    $O/src/capacity_messages/CapacityEvent_m.o \
    $O/src/event_messages/AirplaneModeEventMessage_m.o \
    $O/src/event_messages/BaseEventMessage_m.o \
    $O/src/event_messages/BatteryEventMessage_m.o \
    $O/src/event_messages/BluetoothEventMessage_m.o \
    $O/src/event_messages/CellularEventMessage_m.o \
    $O/src/event_messages/ScreenEventMessage_m.o \
    $O/src/event_messages/TrafficEventMessage_m.o \
    $O/src/event_messages/WiFiEventMessage_m.o \
    $O/src/warning_messages/BatteryCriticalWarningMessage_m.o

# Message files
MSGFILES = \
    src/background_messages/BackgroundEventEndMessage.msg \
    src/background_messages/BackgroundEventMessage.msg \
    src/capacity_messages/CapacityEvent.msg \
    src/event_messages/AirplaneModeEventMessage.msg \
    src/event_messages/BaseEventMessage.msg \
    src/event_messages/BatteryEventMessage.msg \
    src/event_messages/BluetoothEventMessage.msg \
    src/event_messages/CellularEventMessage.msg \
    src/event_messages/ScreenEventMessage.msg \
    src/event_messages/TrafficEventMessage.msg \
    src/event_messages/WiFiEventMessage.msg \
    src/warning_messages/BatteryCriticalWarningMessage.msg

# SM files
SMFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = $(OPPMAIN_LIB) $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) $(IMPORT_DEFINES)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/% :: $O/%
	@mkdir -p $(TARGET_DIR)
	$(Q)$(LN) $< $@
ifeq ($(TOOLCHAIN_NAME),clangc2)
	$(Q)-$(LN) $(<:%.dll=%.lib) $(@:%.dll=%.lib)
endif

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET) $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE) | msgheaders smheaders
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc -MD -MP -MF $O/$(basename $<)_m.h.d $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning $(TARGET)
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET)
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET:%.dll=%.lib)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall:
	$(Q)$(MAKE) -s clean MODE=release
	$(Q)$(MAKE) -s clean MODE=debug
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

# include all dependencies
-include $(OBJS:%=%.d) $(MSGFILES:%.msg=$O/%_m.h.d)
