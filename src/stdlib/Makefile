SUBDIRS := mem
OBJECTS := graphics.o kprint.o print.o string.o
.PHONY: $(SUBDIRS)

all: $(OBJECTS) $(SUBDIRS)

$(SUBDIRS):
	cd $@; $(MAKE)

%.o: %.c
	$(CC) $(INCDIR) $(CFLAGS) $(CPPFLAGS) -c $< -o $(OBJDIR)/$@

%.o: %.s
	$(AS) $(ASMFLAGS) -c $< -o $(OBJDIR)/$@
