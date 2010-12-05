ext2_tool: ext2_tool.c
	$(CC) -D_GNU_SOURCE -I ../kern/fs/ext2/ -o $@ $<


lba: lba.o

pt_walk : pt_walk.o

md5: md5.c
	$(CC) -I../common/include -o $@ $<

clean :
	-rm lba
	-rm pt_walk
	-rm ext2_tool
	-rm md5
	-rm *.o
