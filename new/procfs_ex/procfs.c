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
 
MODULE_LICENSE("GPL");
 
 
#define PROCFS_MAX_SIZE         4096
#define PROCFS_TESTLEVEL        "battery_test"
#define PROCFS_NOTIFYPID        "battery_notify"
#define PROCFS_THRESHOLD        "battery_threshold"
#define PROCFS_PIDTH            "pid_th"
#define PROCFS_LENGTH 8
 
/* Declaration of variables used in this module */
 
 
static int level = 99;
static int test_level = 0;                      //indicates level of battery remain.
static int notify_pid = -1;
static int threshold = -1;
 
/* End of declaration */
 
struct pid_th_t
{
	char pid[PROCFS_LENGTH + 1];
	char threshold[PROCFS_LENGTH + 1];
};

struct proc_dir_entry *proc_pid_th;

struct pid_th_t foo_data;       
 
/* Declaration of ancillary variables */
 
char procfs_buffer[PROCFS_MAX_SIZE];     
static unsigned long procfs_buffer_size = 0;    //size of receive side buffer

static char pidnum_buffer[PROCFS_MAX_SIZE];     
static unsigned long pidnum_buffer_size = 0;    //size of receive side buffer

static char threshold_buffer[PROCFS_MAX_SIZE];     
static unsigned long threshold_buffer_size = 0;    //size of receive side buffer

static struct proc_dir_entry *proc_entry;       //indicates procfs entry.
static struct proc_dir_entry *pidnum_entry;       //pidnum, threshold
static struct proc_dir_entry *threshold_entry;       //pidnum, threshold


 
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
        printk(KERN_INFO "user_space buffer %c\n", user_space_buffer);
        // printk(KERN_INFO "filep %c\n", filep.name);
        
        if(status < 0)
        {
                printk(KERN_INFO "Error while called kstrtoint(...) - %d", status);
               
               
                return -ENOMEM;
        } else {
                printk(KERN_INFO "status > 0 -> success \n");
        }
        // validate level value.
        printk(KERN_INFO "Requested : %d", requested);
        if(requested< 0 || requested > 100){
                printk(KERN_INFO "Invalid battery level.\n");
                return -ENOMEM;
        } else {
                printk(KERN_INFO "request value success\n");
        }
        // accept value.
        test_level = requested;
 
        // *off += procfs_buffer_size; // not necessary here!
         int i;
        

        return procfs_buffer_size; 
}

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
static int pidnum_write( struct file *filp, const char *user_space_buffer, unsigned long len, loff_t *off )
{
 
        int status = 0;
        int requested;
 
        pidnum_buffer_size = len;
 
        if (pidnum_buffer_size > PROCFS_MAX_SIZE ) {
                pidnum_buffer_size = PROCFS_MAX_SIZE;
        }
 
        /* write data to the buffer */
        if ( copy_from_user(pidnum_buffer, user_space_buffer, pidnum_buffer_size) ) {
                return -EFAULT;
        }
 
        status  = kstrtoint(pidnum_buffer, 10, &requested);
        printk(KERN_INFO "Requested - pidnum : %d", requested);
        if(status < 0)
        {
                printk(KERN_INFO "Error while called kstrtoint(...) - %d", status);
                return -ENOMEM;
        } else {
                printk(KERN_INFO "status > 0 -> success \n");
        }
        // validate pidnum  value
        /// 

        if(requested< 0 || requested > 100){
                printk(KERN_INFO "Invalid battery level.\n");
                return -ENOMEM;
        } else {
                printk(KERN_INFO "request value success\n");
        }
        // accept value.
        // threshold, notify pid detection..

        notify_pid = requested;
        
        int i;
        // *off += pidnum_buffer_size; // not necessary here!

        return pidnum_buffer_size; 
}


static int pidnum_read( struct file *filp, char *user_space_buffer, size_t count, loff_t *off )
{
        int ret = 0;
        int flag = 0;
 
        if(*off < 0) *off = 0;
 
        snprintf(pidnum_buffer, 16, "%d\n", notify_pid);
        pidnum_buffer_size = strlen(pidnum_buffer);
 
        if(*off > pidnum_buffer_size){
                return -EFAULT;
        }else if(*off == pidnum_buffer_size){
                return 0;
        }
 
        if(pidnum_buffer_size - *off > count)
                ret = count;
        else
                ret = pidnum_buffer_size - *off;

        flag = copy_to_user(user_space_buffer, pidnum_buffer + (*off), ret);
 
        if(flag < 0)
                return -EFAULT;
        
        *off += ret;
 
        return ret;
 
}

static int threshold_write( struct file *filp, const char *user_space_buffer, unsigned long len, loff_t *off )
{
 
        int status = 0;
        int requested;
 
        threshold_buffer_size = len;
 
        if (threshold_buffer_size > PROCFS_MAX_SIZE ) {
                threshold_buffer_size = PROCFS_MAX_SIZE;
        }
 
        /* write data to the buffer */
        if ( copy_from_user(threshold_buffer, user_space_buffer, threshold_buffer_size) ) {
                return -EFAULT;
        }
 
        status  = kstrtoint(threshold_buffer, 10, &requested);
        
        if(status < 0)
        {
                printk(KERN_INFO "Error while called kstrtoint(...)\n");
                return -ENOMEM;
        } else {
                printk(KERN_INFO "status > 0 -> success \n");
        }
        // validate pid_th  value
        /// 
        printk(KERN_INFO "Requested- threshold : %d", requested);
        if(requested< 0 || requested > 100){
                printk(KERN_INFO "Invalid battery level.\n");
                return -ENOMEM;
        } else {
                printk(KERN_INFO "request value success\n");
        }
        // accept value.
        // threshold, notify pid detection..

        threshold = requested;
        

        // *off += threshold_buffer_size; // not necessary here!
 
        return threshold_buffer_size; 
}


static int threshold_read( struct file *filp, char *user_space_buffer, size_t count, loff_t *off )
{
        int ret = 0;
        int flag = 0;
 
        if(*off < 0) *off = 0;
 
        snprintf(threshold_buffer, 16, "%d\n", threshold);
        threshold_buffer_size = strlen(threshold_buffer);
 
        if(*off > threshold_buffer_size){
                return -EFAULT;
        }else if(*off == threshold_buffer_size){
                return 0;
        }
 
        if(threshold_buffer_size - *off > count)
                ret = count;
        else
                ret = threshold_buffer_size - *off;
        flag = copy_to_user(user_space_buffer, threshold_buffer + (*off), ret);
 
        if(flag < 0)
                return -EFAULT;
        
        *off += ret;
 
        return ret;
 
}





 
/*
        Implementation of procfs read function
*/


 
 
/*
        Configuration of file_operations
 
        This structure indicate functions when read or write operation occured.
*/
static const struct file_operations my_proc_fops = {
        .write = test_level_write,
        .read = test_level_read,
};

static const struct file_operations pidnum_ops = {
        .write = pidnum_write,
        .read = pidnum_read,
};

static const struct file_operations threshold_ops = {
        .write = threshold_write,
        .read = threshold_read,
};

 

 
 
/*
        This function will be called on initialization of  kernel module
*/
int init_process(void)
{
        int ret = 0;
        char *msg;
        msg="123 123";

        proc_entry = proc_create(PROCFS_TESTLEVEL, 0666, NULL, &my_proc_fops);
        pidnum_entry = proc_create("pidnum" ,0666, NULL,&pidnum_ops);
        threshold_entry = proc_create("threshold", 0666, NULL, &threshold_ops);

        printk(KERN_ALERT "[init] init!!");

        if(proc_entry == NULL && pidnum_entry == NULL && threshold_entry == NULL)
        {
                printk(KERN_ALERT "[error] pid_th_entry&other is failed");
                return -ENOMEM;
        }
        return ret;
 
}
 
/*
        This function will be called on cleaning up of kernel module
*/
void process_exit(void)
{
        printk(KERN_ALERT "[exit]Exit");
        remove_proc_entry(PROCFS_TESTLEVEL, proc_entry);
        remove_proc_entry("pidnum", pidnum_entry);
}

module_init(init_process);
module_exit(process_exit);