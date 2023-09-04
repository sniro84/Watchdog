debug: lib_wd.so wd_exec client_exec_debug
release: lib_wd_release.so wd_exec_release client_exec_release

WD_LIB_SRC = watchdog.c scheduler/scheduler.c scheduler/priority_queue.c scheduler/uid.c scheduler/task.c scheduler/dlist.c scheduler/sorted_list.c

CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall
DEBUG_CFLAGS = -g
RELEASE_CFLAGS = -DNDEBUG -O3


lib_wd.so: $(WD_LIB_SRC)
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -I ./ -I ./scheduler -fPIC -shared $(WD_LIB_SRC) -o lib_wd.so -lpthread

wd_exec: wd_exec.c
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS)  -I ./ -I ./scheduler wd_exec.c -o wd_exec.out -L. -l_wd -Wl,-rpath=.

client_exec_debug: client_exec.c
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -I ./ -I ./scheduler client_exec.c -o client_exec_debug.out -L. -l_wd -Wl,-rpath=.


lib_wd_release.so: $(WD_LIB_SRC)
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS) -I ./ -I ./scheduler -fPIC -shared $(WD_LIB_SRC) -o lib_wd.so -lpthread

wd_exec_release: wd_exec.c
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS)  -I ./ -I ./scheduler wd_exec.c -o wd_exec.out -L. -l_wd -Wl,-rpath=.

client_exec_release: client_exec.c
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS) -I ./ -I ./scheduler client_exec.c -o client_exec_release.out -L. -l_wd -Wl,-rpath=.

clean:
	rm -f lib_wd.so wd_exec.out client_exec_release.out client_exec_debug.out
