#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO  */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/fs.h>           /* libfs stuff           */
#include <asm/uaccess.h>        /* copy_to_user          */
#include <linux/buffer_head.h>  /* buffer_head           */
#include <linux/slab.h>         /* kmem_cache            */
#include "assoofs.h"		

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eduardo Juárez Robles");

int assoofs_fill_super(struct super_block *sb, void *data, int silent);
static struct dentry *assoofs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data);
struct assoofs_inode_info *assoofs_get_inode_info(struct super_block *sb, ,uint64_t inode_no);
ssize_t assoofs_read(struct file * filp, char __user * buf, size_t len, loff_t * ppos); ·
int assoofs_sb_get_a_freeblock(struct super_block *vsb, uint64_t * block);
void assoofs_save_sb_info(struct super_block *vsb);
struct dentry *assoofs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags);
static int assoofs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl);
static int assoofs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode);
static int assoofs_create_fs_object(struct inode *dir, struct dentry *dentry, umode_t mode);
struct assoofs_inode_info *assoofs_search_inode_info(struct super_block *sb, struct assoofs_inode_info *start, struct assoofs_inode_info *s
int assoofs_inode_save(struct super_block *sb, struct assoofs_inode_info *inode_info);
void assoofs_add_inode_info(struct super_block *sb, struct assoofs_inode_info *inode);
static struct inode *assoofs_get_inode(struct super_block *sb, int ino);
static int assoofs_iterate(struct file *filp, struct dir_context *ctx);
ssize_t assoofs_write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos);







static int __init init_assoofs(void){
	int ret;
	ret=register_filesystem(assoofs_type);

    if(likely(ret==0)){
 	printk(KERN_INFO "iniciando el sistema de archivos\n");
	
    }
    else
    	printk(KERN_ERR "no se ha podido Iniciar\n");
    	return ret;
}

static void __exit exit_assoofs(void){
	int ret;
	ret=unregister_filesystem(assoofs_type);

	if(likely(ret==0)){
 	printk(KERN_INFO "finalizando el sistema de archivos\n");
	
    }
    else
    	printk(KERN_ERR "error al finalizar el sistema de archivos\n");
 
}

struct dentry *assoofs_lookup (struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags){
	
	struct assoofs_inode_info *parent=parent_inode->i_private;
	struct super_block *sb= parent_inode-> i_sb;
	struct assoofs_dir_record_entry *record;
	struct buffer_head *bh;
	struct assofs_dir_record_entry *record;
	int i;

	bh= sb_bread(sb, parent_info->data_block_number);
	printk(KERN_INFO "lookup in: ino=%ilu, b=%ilu\n", parent_info-> inode_no, parent_info->data_block_number);

	aparent = parent_inode->i_private;
	sb = parent_inode->i_sb;
	

	b = sb_bread(sb, aparent->data_block_number);
	//ahora crearemos nuestra estructura en forma de arbol de directorios

	record = (struct assoofs_dir_record_entry*) bh->b_data;
	for(i=0; i<parent_info->dir_children_count; i++){
		printk(KERN_INFO "have a files: '%a'(ino=%illu)\n", record->filename, record->inode_no);
		if(!strcmp(record->filename, child_dentry-> d_name.name)){
			struct inode *inode=assofs_get_inode(sb,record->inode_no);
		
			
			inode_init_owner(inode, parent_inode, ((struct assoofs_inode_info *)ainode->i_private)->mode);
			//en los apuntes ponia d_add(child_dentry, parent_inode);
			d_add(child_dentry, inode);

			printk(KERN_INFO "El inodo ha sido encontrado\n");
			brelse(b);
			return NULL;
		}
		record++;

	}
	printk(KERN_INFO "El inodo no ha sido encontrado\n", child_dentry->d_name.name);
	return NULL;

}

struct inode *assoofs_get_inode(struct super_block *sb, int ino){
	struct inode *inode;
	struct assoofs_inode_info *inode_info;
	
	inode_info = assoofs_get_inode_info(sb, ino);

	imode = new_inode(sb);
	inode->i_ino = ino;
	inode->i_sb = sb;
	inode->i_op = &assoofs_inode_ops;
	
	if(S_ISDIR(inode_info->mode)){
		printk(KERN_INFO "GETINODE: S_ISDIR\n");
		inode->i_fop=&assoofs_dir_operations;
	}else if (S_ISREG(inode_info->mode)) {
		printk(KERN_INFO "GETINODE: S_ISREG\n");
		inode->i_fop=&assoofs_file_operations;
	}
	else{
		printk(KERN_ERR "unknown inode type. neither a directory not a file\n");
		inode->i_atime =inode->i_mtime=i->inode_ctime =CURRENT_TIME;
		inode->i_private = inode_info;
	return inode;

}

static int assoofs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl){
	printk(KERN_INFO "Entrando en create...\n");
	return assoofs_create_object(dir, dentry, mode);
}



static int assoofs_create_object(struct inode *dir, struct dentry *dentry,
umode_t mode){//mirar desde aqui

	struct inode *inode;
	struct assoofs_inode_info *inode_info;
	struct super_block *sb;
	struct assoofs_inode_info *parent_inode_info;
	struct buffer_head *bh;
	struct assoofs_dir_record_entry *dir_contents;
	uint64_t count;
	int ret;
	
	sb = dir->i_sb;
	count=((struct assoofs_super_block_info *)sb->s_fs_info)->inodes_count;
	if(unlikely(count>=ASSOOFS_MAX_FILESYSTEM_OBJECTS_SUPPORTED)){
		printk(KERN_ERR "maximo numero de objetos soportados por assoofs");
		return -ENOSPC;
	}

	if(!S_ISREG(mode)&& !S_ISDIR(mode)){
		printk(KERN_ERR "creation request but for neither a file nor a directory");
		return -EINVAL;
	}
	inode = new_inode(sb);
	if(!inode){
		return -ENOMEM;
	}

	inode->i_sb = sb;
	inode->i_ino = (count + ASSOOFS_START_INO - ASSOOFS_RESERVED_INODES + 1);
	inode->i_op = &assoofs_inode_ops;
	inode->i_atime = CURRENT_TIME;
	inode->i_mtime = CURRENT_TIME;
	inode->i_ctime = CURRENT_TIME;
	inode_info = kmalloc(sizeof(struct assoofs_inode_info), GFP_KERNEL);
	inode_info->inode_no = inode->i_ino;
	inode_info->mode = mode;

	ret=assoofs_sb_get_a_freeblock(sb, &inode_info->data_block_number);
	if(ret<0){
		printk(KERN_ERR"asoofs could not get a freeblocks")
		return ret;
	}

	if(S_ISDIR(mode)){
		inode->i_fop=&assoofs_dir_operations;
		inode_info->dir_children_count = 0;
	}else if (S_ISREG(mode)){
		inode->i_fop=&assoofs_file_operations;
		inode->file_size = 0;
		
	}
	inode->i_private=inode_info;

	assoofs_add_inode_info(sb, inode_info);

	parent_inode_info=dir->i_private;
	bh=sb_bread(sb, parent_inode_info->data_block_number);
	dir_contents+=parent_inode_info->dir_children_count;
	dir_contents->inode_no=inode_info->inode_no;
	strcpy(dir_contents->filename, dentry->d_name.name);
	mark_buffer_dirty(bh);
	brelse(bh);

	parent_inode_info->dir_children_count++;
	ret= assoofs_save_inode_info(sb, parent_inode_info);
	if(ret){
		return ret;
	}
	inode_init_owner(inode, dir, mode);
	d_add(dentry, inode);
	return 0;

}

//faltan todas las del punto 2.3.4


/********************************************************************************
*******************************iterate, read, write
*********************************************************************************/

static int assoofs_iterate (struct file *filp, struct dir_context *ctx){
	
	struct inode *inode;
	struct seper_block *sb;
	struct buffer_head *bh;
	struct assofs_inode_info *inode_info;
	struct assoofs_dir_record_entry *record;
	int i;

	//para sacar el directorio al que pertenece
	inode = filp->f_path.dentry->d_inode;
	sb= inode->i_sb;
	if(ctx->pos){
		return 0;
	}
	inode_info= inode-> i_private;
	if(unlikely(!S_ISDIR(inode_info->mode))){
		printk(KERN_INFO "Entrando en iterate...\n" ,inode_info->inode_no, inode->i_ino, filp->f_path.dentry->d_name.name);
		return -ENOTDIR;
	}

	bh=sb_bread(sb, inode_info->data_block_number);
	record= (struct assoofs_dir_record_entry *)bh->b_data;
	for(i=0;i<inode_info->dir_children_count; i++){
		dir_emit(ctx, record->filename, ASSOOFS_FILENAME_MAXLEN, record->inode_no, DT_UNKNOWN);
		ctx->pos += sizeof(struct assoofs_dir_record_entry);
		record++;
	}
	brelse(bh);
	return 0;
}


ssize_t assoofs_read (struct file *filp, char __user *buf, size_t len, loff_t *ppos){
	struct assofs_inode_info *inode=filp->f_path.dentry->d_inode->i_private;
	struct buffer_head *bh;
	char *buffer;
	int nbytes;

	if(*ppos >= inode->file_size){
		return 0;
	}
	bh= sb_bread(filp->f_path.dentry->d_inode->i_sb, inode->data_block_number);
	if(!bh){
		printf(KERN_ERR "reading the block number %llu failed", inode->data_block_number);
		return 0;
	}
	buffer= (char *)bh->b_data;
	nbytes = min((size_t) inode->file_size, len);
	if(copy_to_user(buf, buffer, nbytes)){
		brelse(bh);
		printk(KERN_ERR "error copyning file content to the userpace bufer\n");
		return -EFAULT;
	}
	brelse(bh);
	*ppos += nbytes;
	return nbytes;
}


ssize_t assoofs_write (struct file *filp, const char __user *buf, size_t len, loff_t *ppos){
	struct inode *inode;
	struct assoofs_inode_info *inode_info;
	struct buffer_head *bh;
	struct super_block *sb;
	char *buffer;
	int retval;

	sb= filp->i_path.dentry->d_inode->i_sb;
	inode = filp->f_path.dentry->d_inode;
	inode_info=inode->i_private;

	bh=sb_bread(sb, inode_info->data_block_number);
	if(!bh){
		printk(KERN_ERR "reading the block number %llu failed", inode_info->data_block_number);
		return 0;
	}
	buffer= (char *)bh->b_data;
	buffer+=*ppos;
	if (copy_from_user(buffer, buf, len)) {
		brelse(bh);
		printk(KERN_ERR "Fallo en la copia del archivo");
		return -EFAULT;
	}
	*ppos += len;
	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);
	brelse(bh);
	inode_info->file_size=*ppos;
	retval=assoofs_save_inode_info(sb, inode_info);
	return len;
}





	

	
	













