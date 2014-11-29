/* This file requires C99
 * 
 */

#include "rbfs.h"



struct file_system_type rbfs_type = {
	.owner = THIS_MODULE,
	.name = "rbfs",
	.mount = rbfs_mount,
	.kill_sb = kill_block_super,
	.fs_flags = 0	
};

struct super_operations rbfs_super_operations = {
	.put_super = rbfs_put_super,
	.statfs = simple_statfs
};

int rbfs_fill_super(struct super_block *super, void *data, int silent) {
	struct inode *root;

	super->s_magic = MAGIC_NUMBER;
	super->s_op = &rbfs_super_operations;

	root = new_inode(super);
	root->i_ino = 0;
	root->i_sb = super;
	root->i_atime = root->i_mtime = root->i_ctime = CURRENT_TIME;
	inode_init_owner(root, NULL, S_IFDIR);

	super->s_root = d_make_root(root);
	return 0;
}

struct dentry *rbfs_mount(struct file_system_type *type, int flags, const char *dev, void *data) {
	return mount_nodev(type, flags, data, rbfs_fill_super);
}

void rbfs_put_super(struct super_block *sb) {
}



int init_module(void) {
	register_filesystem(&rbfs_type); 
	return 0;
}

void cleanup_module(void) {
	unregister_filesystem(&rbfs_type);
}