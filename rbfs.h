/* This file requires C99
 * 
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/fs.h>

#define MAGIC_NUMBER 0xdeadbeef

#define MIN_FILE_SIZE 1024

struct file_data {
	ssize_t len;
	ssize_t allocated;
	void *data;
};

int rbfs_fill_super(struct super_block *super, void *data, int silent);
struct dentry *rbfs_mount(struct file_system_type *type, int flags, const char *dev, void *data);
void rbfs_put_super(struct super_block *sb);
int rbfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev);

int rbfs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl);

ssize_t rbfs_read(struct file *file, char __user *buf, size_t len, loff_t *ppos);
ssize_t rbfs_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos);

int rbfs_getattr(struct vfsmount *, struct dentry *, struct kstat *);
int rbfs_setattr(struct dentry *dentry, struct iattr *ia);

int init_module(void);

void cleanup_module(void);
