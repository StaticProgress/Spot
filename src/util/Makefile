OBJECTS := minqueue.o util.o

all: $(OBJECTS)

%.o: %.c $(OBJDIR)
	$(CC) $(INCDIR) $(CFLAGS) $(CPPFLAGS) -c $< -o $(OBJDIR)/$@

%.o: %.s $(OBJDIR)
	$(AS) $(ASMFLAGS) -c $< -o $(OBJDIR)/$@
