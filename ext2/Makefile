
CFLAGS=-Wall
TARGET: ext2_mkdir ext2_cp readimage ext2_ln ext2_rm ext2_restore ext2_checker
all : $(TARGET)

ext2_mkdir : ext2_mkdir.c ext2.h 
	gcc $(CFLAGS) -o $@ $< ext2_util.c -lm

ext2_cp : ext2_cp.c ext2.h
	gcc $(CFLAGS) -o $@ $< ext2_util.c -lm

ext2_ln : ext2_ln.c ext2.h 
	gcc $(CFLAGS) -o $@ $< ext2_util.c -lm

ext2_restore : ext2_restore.c ext2.h 
	gcc $(CFLAGS) -o $@ $< ext2_util.c -lm

ext2_checker : ext2_checker.c ext2.h 
	gcc $(CFLAGS) -o $@ $< ext2_util.c -lm

ext2_rm : ext2_rm.c ext2.h
	gcc $(CFLAGS) -o $@ $< ext2_util.c -lm

clean : 
	rm -f ext2_cp ext2_mkdir ext2_ln ext2_restore ext2_checker ext2_rm
