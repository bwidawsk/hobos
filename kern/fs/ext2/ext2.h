/* Original data came from: http://book.opensourceproject.org.cn/kernel/kernel3rd/opensource/0596005652/understandlk-chp-18-sect-2.html#understandlk-chp-18-table-1 */
/* http://uranus.chrysocome.net/explore2fs/es2fs.htm */
/* http://web.mit.edu/tytso/www/linux/ext2intro.html */

#ifndef __le32
	#define __le32 unsigned int
	#define __le16 unsigned short
#endif

#ifndef __u8
	#define __u8  unsigned char
	#define __u16 unsigned short
	#define __u32 unsigned int
#endif

/* I copied this from Linux, remove because I don't want to GPL */
/*
 *  * Constants relative to the data blocks
 *   */
#define EXT2_NDIR_BLOCKS        12
#define EXT2_IND_BLOCK          EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK         (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK         (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS           (EXT2_TIND_BLOCK + 1)
/*
 *  * Structure of a directory entry
 *   */
#define EXT2_NAME_LEN 255

/* end linux copy/GPL removal */



/* bw: I got this from a hexdump, where is the documentation for it! */
#define EXT2_MAGIC	0xEF53

#define S_IFMT  	0xF000 	/* format mask */
#define S_IFSOCK 	0xA000 	/* socket */
#define S_IFLNK 	0xC000 	/* symbolic link */
#define S_IFREG 	0x8000 	/* regular file */
#define S_IFBLK 	0x6000 	/* block device */
#define S_IFDIR 	0x4000 	/* directory */
#define S_IFCHR 	0x2000 	/* character device */
#define S_IFIFO 	0x1000 	/* fifo */
  	  	 
#define S_ISUID 	0x0800 	/* SUID */
#define S_ISGID 	0x0400 	/* SGID */
#define S_ISVTX 	0x0200 	/* sticky bit */
  	  	 
#define S_IRWXU 	0x01C0 	/* user mask */
#define S_IRUSR 	0x0100 	/* read */
#define S_IWUSR 	0x0080 	/* write */
#define S_IXUSR 	0x0040 	/* execute */
  	  	 
#define S_IRWXG 	0x0038 	/* group mask */
#define S_IRGRP 	0x0020 	/* read */
#define S_IWGRP 	0x0010 	/* write */
#define S_IXGRP 	0x0008 	/* execute */
  	  	 
#define S_IRWXO 	0x0007 	/* other mask */
#define S_IROTH 	0x0004 	/* read */
#define S_IWOTH 	0x0002 	/* write */
#define S_IXOTH 	0x0001 	/* execute */

#define EXT2_BAD_INO  			1  	/* Bad blocks inode */
#define EXT2_ROOT_INO 			2 	/* Root inode */
#define EXT2_ACL_IDX_INO 		3 	/* ACL inode */
#define EXT2_ACL_DATA_INO 		4 	/* ACL inode */
#define EXT2_BOOT_LOADER_INO 	5 	/* Boot loader inode */
#define EXT2_UNDEL_DIR_INO 		6 	/* Undelete directory inode */
#define EXT2_FIRST_INO 			11 	/* First non reserved inode */

#define EXT2_FT_UNKNOWN			0
#define EXT2_FT_REG_FILE		1
#define EXT2_FT_DIR				2
#define EXT2_FT_CHRDEV			3
#define EXT2_FT_BLKDEV			4
#define EXT2_FT_FIFO			5
#define EXT2_FT_SOCK			6
#define EXT2_FT_SYMLINK			7

struct ext2_super_block {
	__le32 s_inodes_count; /* Total number of inodes */
	__le32 s_blocks_count; /* Filesystem size in blocks */
	__le32 s_r_blocks_count; /* Number of reserved blocks */
	__le32 s_free_blocks_count; /* Free blocks counter */
	__le32 s_free_inodes_count; /* Free inodes counter */
	__le32 s_first_data_block; /* Number of first useful block (always 1) */
	__le32 s_log_block_size; /* Block size */
	__le32 s_log_frag_size; /* Fragment size */
	__le32 s_blocks_per_group; /* Number of blocks per group */
	__le32 s_frags_per_group; /* Number of fragments per group */
	__le32 s_inodes_per_group; /* Number of inodes per group */
	__le32 s_mtime; /* Time of last mount operation */
	__le32 s_wtime; /* Time of last write operation */
	__le16 s_mnt_count; /* Mount operations counter */
	__le16 s_max_mnt_count; /* Number of mount operations before check */
	__le16 s_magic; /* Magic signature */
	__le16 s_state; /* Status flag */
	__le16 s_errors; /* Behavior when detecting errors */
	__le16 s_minor_rev_level; /* Minor revision level */
	__le32 s_lastcheck; /* Time of last check */
	__le32 s_checkinterval; /* Time between checks */
	__le32 s_creator_os; /* OS where filesystem was created */
	__le32 s_rev_level; /* Revision level of the filesystem */
	__le16 s_def_resuid; /* Default UID for reserved blocks */
	__le16 s_def_resgid; /* Default user group ID for reserved blocks */
	__le32 s_first_ino; /* Number of first nonreserved inode */
	__le16 s_inode_size; /* Size of on-disk inode structure */
	__le16 s_block_group_nr; /* Block group number of this superblock */
	__le32 s_feature_compat; /* Compatible features bitmap */
	__le32 s_feature_incompat; /* Incompatible features bitmap */
	__le32 s_feature_ro_compat; /* Read-only compatible features bitmap */
	__u8 s_uuid[16]; /* 128-bit filesystem identifier */
	char s_volume_name[16]; /* Volume name */
	char s_last_mounted[64]; /* Pathname of last mount point */
	__le32 s_algorithm_usage_bitmap; /* Used for compression */
	__u8 s_prealloc_blocks; /* Number of blocks to preallocate */
	__u8 s_prealloc_dir_blocks; /* Number of blocks to preallocate for directories */
	__u16 s_padding1; /* Alignment to word */
	__u32 s_reserved[204]; /* Nulls to pad out 1,024 bytes */
} __attribute__((packed)) ;

struct ext2_group_desc {
	__le32 bg_block_bitmap; /* Block number of block bitmap */
	__le32 bg_inode_bitmap; /* Block number of inode bitmap */
	__le32 bg_inode_table; /* Block number of first inode table block */
	__le16 bg_free_blocks_count; /* Number of free blocks in the group */
	__le16 bg_free_inodes_count; /* Number of free inodes in the group */
	__le16 bg_used_dirs_count; /* Number of directories in the group */
	__le16 bg_pad; /* Alignment to word */
	__le32 bg_reserved[3]; /* Nulls to pad out 24 bytes */
} __attribute__((packed)) ;


/*
 * Structure of an inode on the disk
 * BAW: taken from fbsd
 */
struct ext2_inode {
	__u16	i_mode;		/* File mode */
	__u16	i_uid;		/* Owner Uid */
	__u32	i_size;		/* Size in bytes */
	__u32	i_atime;	/* Access time */
	__u32	i_ctime;	/* Creation time */
	__u32	i_mtime;	/* Modification time */
	__u32	i_dtime;	/* Deletion Time */
	__u16	i_gid;		/* Group Id */
	__u16	i_links_count;	/* Links count */
	__u32	i_blocks;	/* Blocks count */
	__u32	i_flags;	/* File flags */
	union {
		struct {
			__u32  l_i_reserved1;
		} linux1;
		struct {
			__u32  h_i_translator;
		} hurd1;
		struct {
			__u32  m_i_reserved1;
		} masix1;
	} osd1;				/* OS dependent 1 */
	__u32	i_block[EXT2_N_BLOCKS];/* Pointers to blocks */
	__u32	i_generation;	/* File version (for NFS) */
	__u32	i_file_acl;	/* File ACL */
	__u32	i_dir_acl;	/* Directory ACL */
	__u32	i_faddr;	/* Fragment address */
	union {
		struct {
			__u8	l_i_frag;	/* Fragment number */
			__u8	l_i_fsize;	/* Fragment size */
			__u16	i_pad1;
			__u32	l_i_reserved2[2];
		} linux2;
		struct {
			__u8	h_i_frag;	/* Fragment number */
			__u8	h_i_fsize;	/* Fragment size */
			__u16	h_i_mode_high;
			__u16	h_i_uid_high;
			__u16	h_i_gid_high;
			__u32	h_i_author;
		} hurd2;
		struct {
			__u8	m_i_frag;	/* Fragment number */
			__u8	m_i_fsize;	/* Fragment size */
			__u16	m_pad1;
			__u32	m_i_reserved2[2];
		} masix2;
	} osd2;				/* OS dependent 2 */
} __attribute__((packed));

struct ext2_dir_entry_2 {
	__le32 inode; /* Inode number */
	__le16 rec_len; /* Directory entry length */
	__u8 name_len; /* Filename length */
	__u8 file_type; /* File type */
	char name[EXT2_NAME_LEN]; /* Filename */
} __attribute__((packed)) ;
