/* This file requires C99
 * 
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

#define MAGIC_NUMBER 0xdeadbeef


int rbfs_fill_super(struct super_block *super, void *data, int silent);
struct dentry *rbfs_mount(struct file_system_type *type, int flags, const char *dev, void *data);
void rbfs_put_super(struct super_block *sb);



int init_module(void);

void cleanup_module(void);
