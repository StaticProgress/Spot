# 10 = EFI application
# 11 = EFI boot service driver
# 12 = EFI runtime driver
SUBSYSTEM = 10
GNUEFI_DIR = gnu-efi/inc
OBJ_DIR = obj
BIN_DIR = bin
BOOT_DIR = EFI/BOOT
MKDIR_P = mkdir -p
OBJS = $(OBJ_DIR)/hello.o $(OBJ_DIR)/data.o

CC = x86_64-w64-mingw32-gcc
CFLAGS = -ffreestanding -Wall
CFLAGS += -I$(GNUEFI_DIR) -I$(GNUEFI_DIR)/x86_64 -I$(GNUEFI_DIR)/protocol
LDFLAGS = -nostdlib -Wl,-dll -shared -Wl,--subsystem,$(SUBSYSTEM) -e efi_main

BOOTX64.EFI: $(OBJS)
	$(MKDIR_P) $(BIN_DIR)/$(BOOT_DIR)
	$(CC) $(LDFLAGS) -o $(BIN_DIR)/$(BOOT_DIR)/$@ $^ -lgcc

$(OBJ_DIR):
	$(MKDIR_P) $(OBJ_DIR)

obj/%.o: src/%.c $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(BIN_DIR)/$(BOOT_DIR)/BOOTX64.EFI $(OBJ_DIR)/*.o
	rm -rf $(OBJ_DIR) $(BIN_DIR)