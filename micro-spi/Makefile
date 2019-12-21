EXEC=test.elf

C_SRCS   = adc.c
CPP_SRCS = main.cpp
ASM_SRCS = 

SYSTEM_SRCS += ../sys/startup_ARMCM4.c
SYSTEM_SRCS += ../sys/startup_clock.c

C_SRCS += $(SYSTEM_SRCS)

#particular config:
OPTIM=0			#set to 1 to enable C compiler optimizations
USE_TFT=1		#set to 1 to compile tft related files

CFLAGS=			#flags to the C compiler
CPPFLAGS=		#flags to the C++ compiler
COMMONFLAGS=	#common flags to C (CFLAGS) and C++ (CPPFLAGS)
LDFLAGS = 		#flags to the linker

EXTRA_FILES_CLEAN= $(EXEC) $(EXEC).bin $(EXEC).map

########################################################
#system files
########################################################
#C++ cross compiler
CXX=arm-none-eabi-g++
#C cross compiler
CC=arm-none-eabi-gcc

########################################################
#TFT lib
########################################################
TFT_LIB_DIR = ../lib/tft/
TFT_CPP_SRCS = Print.cpp Adafruit_GFX.cpp  Adafruit_SPITFT.cpp Adafruit_ST7735.cpp Adafruit_ST77xx.cpp 
TFT_INC += -I$(TFT_LIB_DIR)/Adafruit-GFX-Library
TFT_INC += -I$(TFT_LIB_DIR)/Adafruit-GFX-Library/Fonts
TFT_INC += -I$(TFT_LIB_DIR)/Adafruit-ST7735-Library
TFT_INC += -I$(TFT_LIB_DIR)/arduinoCore
#also requires lib dir for spi.c/h
TFT_C_SRCS += spi.c

ifeq ($(strip $(USE_TFT)),1)
	C_SRCS += $(TFT_C_SRCS)
	CPP_SRCS += $(TFT_CPP_SRCS)
	COMMONFLAGS += $(TFT_INC)
endif

########################################################
# STM32F303 specific flags
########################################################

CMSIS_DIR = ../sys/CMSIS
LIB_DIR = ../lib
COMMONFLAGS += -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F303x8 -DARMCM4
COMMONFLAGS += -I$(CMSIS_DIR)/Device/ST/STM32F3xx/Include 
COMMONFLAGS += -I$(CMSIS_DIR)/Include
COMMONFLAGS += -I$(LIB_DIR)

CFLAGS += -fstack-usage -ffunction-sections #TODO: a regarder plus en detail.
CFLAGS += -std=gnu99
CFLAGS += $(COMMONFLAGS)

CPPFLAGS += $(COMMONFLAGS)

LDFLAGS += -mthumb -mcpu=cortex-m4 -T../sys/stm32f303K8.ld -mfloat-abi=hard -mfpu=fpv4-sp-d16 --specs=nosys.specs -lm
LDFLAGS += -Wl,--start-group -lm  -Wl,--end-group -Wl,--gc-sections
LDFLAGS += -Lsys
LDFLAGS += -lstdc++_nano
LDFLAGS += -Xlinker -Map=$(EXEC).map


ALL: OBJ_DIR_CREATE $(EXEC)
	

#########################################################
## flash 
#########################################################
##extra rules (flash)

$(EXEC).bin: $(EXEC)
	arm-none-eabi-objcopy -O binary $(EXEC) $(EXEC).bin

flash: OBJ_DIR_CREATE $(EXEC).bin
	@st-flash --reset write $(EXEC).bin 0x8000000

########################################################
# compiler optimisation
########################################################
COMMONFLAGS_DEBUG = -O0 -g -Wall -fno-exceptions -Wno-deprecated -DDEBUG
COMMONFLAGS_OPTIM = -O3 -funroll-loops -fomit-frame-pointer -fno-strict-aliasing -pipe -ffast-math -fexceptions


ifeq ($(strip $(OPTIM)),1)
	CFLAGS   += $(COMMONFLAGS_OPTIM)
	CPPFLAGS += $(COMMONFLAGS_OPTIM)
	msg += "with optimizations "
else
	CFLAGS   += $(COMMONFLAGS_DEBUG)
	CPPFLAGS += $(COMMONFLAGS_DEBUG)
	msg += "without optimizations "
endif
########################################################

OBJ_DIR = build
C_SRCS_NODIR   = $(notdir $(C_SRCS))
CPP_SRCS_NODIR = $(notdir $(CPP_SRCS))
ASM_SRCS_NODIR = $(notdir $(ASM_SRCS))
OBJ  = $(addprefix $(OBJ_DIR)/,$(CPP_SRCS_NODIR:.cpp=.o))
OBJ += $(addprefix $(OBJ_DIR)/,$(C_SRCS_NODIR:.c=.o))
OBJ += $(addprefix $(OBJ_DIR)/,$(ASM_SRCS_NODIR:.S=.o))
DEPFILES  = $(addprefix $(OBJ_DIR)/,$(CPP_SRCS_NODIR:.cpp=.dep))
DEPFILES += $(addprefix $(OBJ_DIR)/,$(C_SRCS_NODIR:.c=.dep))

.PHONY: ALL
#ALL: OBJ_DIR_CREATE $(EXEC)

########################################################

.PHONY: clean OBJ_DIR_CREATE
clean:
	@rm -rf $(OBJ_DIR) *~  $(EXTRA_FILES_CLEAN)

$(EXEC): $(OBJ) 
	@echo linking.. $(EXEC)
	@$(CXX) -o $@ $(OBJ) $(LDFLAGS)

$(OBJ_DIR)/%.o: ../sys/%.c
	@echo c compiling $(msg)$< ...
	@$(CC) -MD -MF $(OBJ_DIR)/$(notdir $<).dep -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(TFT_LIB_DIR)/Adafruit-ST7735-Library/%.cpp
	@echo c++ compiling $(msg)$< ...
	@$(CXX) -MD -MF $(OBJ_DIR)/$(notdir $<).dep -c $(CPPFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(TFT_LIB_DIR)/Adafruit-GFX-Library/%.cpp
	@echo c++ compiling $(msg)$< ...
	@$(CXX) -MD -MF $(OBJ_DIR)/$(notdir $<).dep -c $(CPPFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(TFT_LIB_DIR)/arduinoCore/%.cpp
	@echo c++ compiling $(msg)$< ...
	@$(CXX) -MD -MF $(OBJ_DIR)/$(notdir $<).dep -c $(CPPFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c
	@echo c compiling $(msg)$< ...
	@$(CC) -MD -MF $(OBJ_DIR)/$(notdir $<).dep -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c
	@echo c compiling $(msg)$< ...
	@$(CC) -MD -MF $(OBJ_DIR)/$<.dep -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.cpp 
	@echo c++ compiling $(msg)$< ...
	@$(CXX) -MD -MF $(OBJ_DIR)/$<.dep -c $(CPPFLAGS) $< -o $@

OBJ_DIR_CREATE: 
	@if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi;

#include dependancy files.
$(foreach source,$(SRC_NODIR),$(eval -include $(OBJ_DIR)/${source}.dep))

# vim:ft=make
