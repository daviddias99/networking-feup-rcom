CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic
ODIR = build

SRC_APP = application
SRC_UTIL = util
SRC_LL = linklayer

TARGET = penguin

OBJS = $(ODIR)/tlv.o $(ODIR)/app.o $(ODIR)/app_util.o $(ODIR)/app_send.o $(ODIR)/app_receive.o $(ODIR)/log.o  $(ODIR)/linklayer.o $(ODIR)/receiver_state_machine.o $(ODIR)/transmitter_state_machine.o


.PHONY: all clean

all: build_dir $(TARGET)
	
build_dir:
	mkdir -p build

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


ifdef DEV_INFO
DEV_FLAGS = -save-temps
endif


$(ODIR)/%.o: $(SRC_APP)/%.c
	$(CC) $(CFLAGS) $(DEV_FLAGS) -MMD -c $< -o $@

$(ODIR)/%.o: $(SRC_LL)/%.c
	$(CC) $(CFLAGS) $(DEV_FLAGS) -MMD -c $< -o $@

$(ODIR)/%.o: $(SRC_UTIL)/%.c
	$(CC) $(CFLAGS) $(DEV_FLAGS) -MMD -c $< -o $@


clean:
	rm -f $(TARGET) $(ODIR)/*.o $(ODIR)/*.d $(ODIR)/*.i $(ODIR)/*.s *.txt
	
-include $(TARGET:=.d)
