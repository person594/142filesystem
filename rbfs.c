/* This file requires C99
 * 
 */

#include "rbfs.h"

int inode_num = 0;

struct file_system_type rbfs_type = {
	.owner = THIS_MODULE,
	.name = "rbfs",
	.mount = rbfs_mount,
	.kill_sb = rbfs_put_super, //this function just does nothing
	.fs_flags = 0	
};

struct super_operations rbfs_super_operations = {
	.put_super = rbfs_put_super,
	.statfs = simple_statfs
};

struct inode_operations rbfs_dir_inode_operations = {
	.lookup = simple_lookup,
	.mknod = rbfs_mknod
};

struct inode_operations rbfs_file_inode_operations = {
	.getattr = simple_getattr, 
	.setattr = simple_setattr
};

struct file_operations rbfs_file_operations = {
	
};

struct inode *rbfs_make_file_inode(struct super_block *super, struct inode *dir) {
	struct inode *inode = new_inode(super);
	inode->i_ino = inode_num++;
	inode_init_owner(inode, dir, S_IFREG);
	
	inode->i_op = &rbfs_file_inode_operations;
	inode->i_fop = &rbfs_file_operations;

	return inode;
}

/*We need a dev_t, even though we are deviceless*/
int rbfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev) {
	struct inode *inode = rbfs_make_file_inode(dir->i_sb, dir);
	d_instantiate(dentry, inode);
	//do we need dget here?
	dir->i_mtime = dir->i_ctime = CURRENT_TIME;
}

int rbfs_fill_super(struct super_block *super, void *data, int silent) {
	struct inode *root;

	super->s_magic = MAGIC_NUMBER;
	/*super->s_op = &rbfs_super_operations;*/

	root = new_inode(super);
	root->i_ino = 0;
	root->i_sb = super;
	root->i_atime = root->i_mtime = root->i_ctime = CURRENT_TIME;
	root->i_op = &rbfs_dir_inode_operations;
	root->i_fop = &simple_dir_operations;
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
