#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <asm/siginfo.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/string.h>

#include <linux/fs.h>

 
MODULE_LICENSE("GPL");
 
 
#define PROCFS_MAX_SIZE         1024
#define PROCFS_TESTLEVEL        "battery_test"
#define PROCFS_NOTIFYPID        "battery_notify"
#define PROCFS_THRESHOLD        "battery_threshold"

#define CHR_DEV_NAME "battery_device"
#define CHR_DEV_MAJOR 240

 
 
/* Declaration of variables used in this module */
 
 
static int level = 99;
static int test_level = 0;                      //indicates level of battery remain.
static int notify_pid = -1;
static int threshold = -1;
 
/* End of declaration */
 
 
 
/* Declaration of ancillary variables */
 
static char procfs_buffer[PROCFS_MAX_SIZE];     
static unsigned long procfs_buffer_size = 0;    //size of receive side buffer
static struct proc_dir_entry *proc_entry;       //indicates procfs entry.
 
/* End of declaration */
 
 
 
/*
        Implementation of procfs write function
*/
static int test_level_write( struct file *filp, const char *user_space_buffer, unsigned long len, loff_t *off )
{
        int status = 0;
        int requested;
 
        procfs_buffer_size = len;
 
        if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
                procfs_buffer_size = PROCFS_MAX_SIZE;
        }
 
        /* write data to the buffer */
        if ( copy_from_user(procfs_buffer, user_space_buffer, procfs_buffer_size) ) {
                return -EFAULT;
        }
 
        status  = kstrtoint(procfs_buffer, 10, &requested);
        if(status < 0)
        {
                printk(KERN_INFO "Error while called kstrtoint(...)\n");
                return -ENOMEM;
        }
        // validate level value.
        if(requested< 0 || requested > 100){
                printk(KERN_INFO "Invalid battery level.\n");
                return -ENOMEM;
        }
        // accept value.
        test_level = requested;
 
        // *off += procfs_buffer_size; // not necessary here!
 
        return procfs_buffer_size;
 
}
 
/*
        Implementation of procfs read function
*/
static int test_level_read( struct file *filp, char *user_space_buffer, size_t count, loff_t *off )
{
        int ret = 0;
        int flag = 0;
 
        if(*off < 0) *off = 0;
 
        snprintf(procfs_buffer, 16, "%d\n", test_level);
        procfs_buffer_size = strlen(procfs_buffer);
 
        if(*off > procfs_buffer_size){
                return -EFAULT;
        }else if(*off == procfs_buffer_size){
                return 0;
        }
 
        if(procfs_buffer_size - *off > count)
                ret = count;
        else
                ret = procfs_buffer_size - *off;
 
 
 
 
        flag = copy_to_user(user_space_buffer, procfs_buffer + (*off), ret);
 
        if(flag < 0)
                return -EFAULT;
 
        *off += ret;
 
        return ret;
 
}
 
 
/*
        Configuration of file_operations
 
        This structure indicate functions when read or write operation occured.
*/
static const struct file_operations my_proc_fops = {
        .write = test_level_write,
        .read = test_level_read,
};

int chr_open(struct inode *inode, struct file *filep) {
        int number = MINOR(inode->i_rdef);
        printk("Virtual Character Device Open: Minor Number is %d\n", number );
        return 0;
}

ssize_t chr_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
        printk("write data: %s\n", buf);
        return count;
}

ssize_t chr_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
        printk("read data: %s\n", buf);
        return count;
}

int chr_iotcl(sturct inode *inode, struct file *filep, unsigned int cmd, unsigned long arg) {
        switch(cmd) {
                case 0: printk("cmd value is %d\n", cmd) ; break;
                case 4: printk("cmd value is %d\n", cmd); break;
        }
        return 0;
}

int chr_release(struct inode *inode , struct file *filep) {
        printk("Virtual Character Device Release\n");
        return 0;
}

struct file_operations chr_fops = {
        owner: THIS_MODULE ,
        unlocked_ioctl: chr_ioctl, 
        write: chr_write,
        open: chr_read, 
        release: chr_release
};




static int init_process(void){
        int ret = 0;
        int regist_driver; 
        proc_entry = proc_create(PROCFS_TESTLEVEL, 0666, NULL, &my_proc_fops);
        register_Dri

        if(proc_entry == NULL)
        {
        return -ENOMEM;
        }


        return ret;
}

static void exit_process(void) {
    printk(KERN_ALERT "[exit] Hello Test.");
}
 
/*
        This function will be called on cleaning up of kernel module
*/


module_init(init_process);
module_exit(exit_process);