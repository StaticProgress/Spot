SUBDIRS := mem
OBJECTS := graphics.o kprint.o print.o string.o
.PHONY: $(SUBDIRS)

all: $(OBJECTS) $(SUBDIRS)

$(SUBDIRS):
	cd $@; $(MAKE)

%.o: %.c $(OBJDIR)
	$(CC) $(INCDIR) $(CFLAGS) $(CPPFLAGS) -c $< -o $(OBJDIR)/$@

%.o: %.s $(OBJDIR)
	$(AS) $(ASMFLAGS) -c $< -o $(OBJDIR)/$@
