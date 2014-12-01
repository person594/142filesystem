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
	.mknod = rbfs_mknod,
	.create = rbfs_create
};

struct inode_operations rbfs_file_inode_operations = {
	.getattr = rbfs_getattr,
	.setattr = simple_setattr
};

struct file_operations rbfs_file_operations = {
	.read = rbfs_read,
	.aio_read = generic_file_aio_read,
	.write = rbfs_write,
	.aio_write = generic_file_aio_write,
	.fsync = noop_fsync,
	.mmap = generic_file_mmap,
	.splice_read = generic_file_splice_read,
	.splice_write = generic_file_splice_write,
	.llseek	= generic_file_llseek
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
	dget(dentry);
	//do we need dget here?
	dir->i_mtime = dir->i_ctime = CURRENT_TIME;
	return 0;
}

int rbfs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl) {
	printk("making file %s\n", dentry->d_name.name);
	return rbfs_mknod(dir, dentry, mode | S_IFREG, 0);
}


ssize_t rbfs_read(struct file *file, char __user *buf, size_t len, loff_t *pos) {
	struct file_data *fd;
	printk("beginning read\n");
	if (file->f_dentry->d_fsdata == NULL) {
		return 0;
	} else {
		fd = (struct file_data *) file->f_dentry->d_fsdata;
	}
	if (*pos + len > fd->len) len = fd->len - *pos;
	
	memcpy(buf, fd->data + *pos, len);
	*pos += len;
	printk("read %d bytes\n", (int)len);
	return len;
}
ssize_t rbfs_write(struct file *file, const char __user *buf, size_t len, loff_t *pos) {
	struct file_data *fd;
	ssize_t size_needed;
	if (file->f_dentry->d_fsdata == NULL) {
		file->f_dentry->d_fsdata = kmalloc(sizeof(struct file_data), GFP_USER);
		fd = file->f_dentry->d_fsdata;
		fd->len = 0;
		fd->allocated = MIN_FILE_SIZE;
		fd->data = kmalloc(fd->allocated, GFP_USER);
		file->f_dentry->d_fsdata = fd;
	} else {
		fd = (struct file_data *) file->private_data;
	}
	size_needed = *pos + len;
	if (size_needed > fd->allocated) {
		fd->allocated = size_needed;
		fd->data = krealloc(fd->data, size_needed, GFP_USER);
	}
	if (size_needed > fd->len) {
		fd->len = size_needed;
	}
	//at this point, we should have enough space allocated
	memcpy(fd->data + *pos, buf, len);
	*pos += len;
	printk("wrote %d bytes\n", (int)len);
	return len;
}

int rbfs_getattr (struct vfsmount *mount, struct dentry *dentry, struct kstat *kstat) {
	//struct file_data *fd;
	int ret;
	ret = simple_getattr(mount , dentry, kstat);
	kstat->size = 9001;
	return ret;
	/*
	fd = dentry->d_fsdata;
	if (fd == NULL) return ret;
	kstat->size = fd->len;
	return ret;*/
}
int rbfs_setattr(struct dentry *dentry, struct iattr *ia);

int rbfs_fill_super(struct super_block *super, void *data, int silent) {
	struct inode *root;

	printk("filling superblock\n");
	super->s_magic = MAGIC_NUMBER;
	/*super->s_op = &rbfs_super_operations;*/

	root = new_inode(super);
	root->i_ino = inode_num++;
	root->i_sb = super;
	root->i_atime = root->i_mtime = root->i_ctime = CURRENT_TIME;
	root->i_op = &rbfs_dir_inode_operations;
	root->i_fop = &simple_dir_operations;
	inode_init_owner(root, NULL, S_IFDIR);

	super->s_root = d_make_root(root);
	return 0;
}

struct dentry *rbfs_mount(struct file_system_type *type, int flags, const char *dev, void *data) {
	printk("mounting\n");
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
