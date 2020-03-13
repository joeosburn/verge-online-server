SHELL = /bin/sh

CC = gcc
LEX = lex
YACC = yacc
# Add -pg -a to CFLAGS and LDFLAGS to enable profiling
# Run gprof on outputting gmon profile file
CFLAGS = -ggdb -Wall -O3
LDFLAGS = -lmysqlclient -lfl -lpthread
LFLAGS = -f
YFLAGS = -d
BINDIR = ../bin

# Object files needed by voworld
VOW_OBJFILES = comm.o config.o config_parser.o log.o map.o npc.o packet.o \
	packet_event.o packet_object.o packet_system.o packet_text.o \
	proc_event.o proc_object.o proc_text.o proc_sys.o player.o \
	player_output.o sql.o svr_cmds.o sys.o vo.o
# Object files needed by vostate
#VOS_OBJFILES = 

# Object files needed by voupdate
#VOU_OBJFILES =

default: bin

all: bin util

bin: voworld vostate voupdate

util: zhrink

clean:
	-rm -f *.o
	-rm -f config_parser.c
	-rm -f config.c
	-rm -f y.tab.h

zhrink:
	(cd util; $(MAKE))

vostate:

voupdate:

voworld: $(VOW_OBJFILES)
	$(CC) -o $(BINDIR)/voworld $(VOW_OBJFILES) $(LDFLAGS)

comm.o : comm.c conf.h vo.h comm.h log.h
config.o : config.y config_parser.c vo.h conf.h log.h
config_parser.o: config_parser.l config.y vo.h conf.h log.h
log.o : log.c conf.h vo.h log.h
map.o : map.c map.h vo.h log.h
npc.o : npc.c vo.h conf.h npc.h log.h
packet.o : packet.c packet.h vo.h conf.h log.h
packet_event.o : packet_event.c vo.h conf.h packet.h log.h
packet_object.o : packet_object.c packet.h vo.h conf.h log.h
packet_system.o : packet_system.c packet.h vo.h conf.h log.h
packet_text.o : packet_text.c packet.h vo.h conf.h log.h
proc_event.o : proc_event.c packet.h vo.h conf.h log.h
proc_object.o : proc_object.c packet.h vo.h conf.h log.h
proc_text.o : proc_text.c packet.h vo.h conf.h log.h
proc_sys.o : proc_sys.c packet.h vo.h conf.h log.h
player.o : player.c vo.h conf.h player.h log.h
player_output.o : player_output.c vo.h conf.h player.h log.h
sql.o : sql.c conf.h vo.h sql.h log.h
svr_cmds.o : svr_cmds.c svr_cmds.h vo.h conf.h log.h
sys.o : sys.c conf.h vo.h sys.h log.h
vo.o : vo.c conf.h vo.h log.h
