OBJECTS := minqueue.o util.o

all: $(OBJECTS)

%.o: %.c
	$(CC) $(INCDIR) $(CFLAGS) $(CPPFLAGS) -c $< -o $(OBJDIR)/$@

%.o: %.s
	$(AS) $(ASMFLAGS) -c $< -o $(OBJDIR)/$@
