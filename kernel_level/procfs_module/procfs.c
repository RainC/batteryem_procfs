#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/init.h> 
#include <linux/signal.h> 
#include <linux/rcupdate.h>
#include <linux/string.h>
  
#include <linux/sched.h>    //find_task_by_pid_type
   
#include <asm/siginfo.h>  	
#include <linux/debugfs.h> 


 
MODULE_LICENSE("GPL");
 
 
#define PROCFS_MAX_SIZE         4096
#define PROCFS_TESTLEVEL        "battery_test"
#define PROCFS_NOTIFYPID        "battery_notify"
#define PROCFS_THRESHOLD        "battery_threshold"
#define PROCFS_PIDTH            "pid_th"
#define PROCFS_LENGTH 8
#define SIGNAL_USR1  1
#define SIGNAL_USR2  2
#define SIG_TEST 44

#define CHR_DEV_NAME "chr_dev"
#define CHR_DEV_MAJOR 240

 
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
static int send_signal_logic(pid_t pid, int sig) {
        int error = -ESRCH;              /* default return value */
        struct task_struct* p;
        struct task_struct* t = NULL; 
        struct pid* pspid;
        // safe kill 
        rcu_read_lock(); 
        p = &init_task;                  /* start at init */
        do {
            if (p->pid == pid) {         /* does the pid (not tgid) match? */
                t = p;    
            break;
        }
        
        p = next_task(p);    /* "this isn't the task you're looking for" */
        } while (p != &init_task);   /* stop when we get back to init */
        if (t != NULL) {
                pspid = t->pids[PIDTYPE_PID].pid;
                if (pspid != NULL) error = kill_pid(pspid,sig,1);
        }
        rcu_read_unlock();

        printk("Error : %d", error);
        return error;
}


 
 
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
                printk(KERN_INFO "Invalid battery level. - test level\n");
                return -ENOMEM;
        } else {
                printk(KERN_INFO "request value success\n");
        }
        // accept value.
        test_level = requested;
        if (test_level < threshold ) { 
                send_signal_logic(notify_pid, SIGUSR1);
        } else {
                send_signal_logic(notify_pid, SIGUSR2);
        }
        
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


// Device driver test
int chr_open (struct inode *inode, struct file *filep) {
        int number = MINOR (inode->i_rdev);
        printk("Virtual Character Device open: Minor %d", number);
        return 0;
}

ssize_t chr_write(struct file *filep, const char *buf, size_t count ,loff_t *f_pos) {
        // int status;
        kstrtoint(buf, 10, &test_level);
        printk("current test_level : %d", test_level);
        return count;
}

ssize_t chr_read(struct file *filep, const char *buf, size_t count ,loff_t *f_pos) {
        // int status;
        // kstrtoint(buf, 10, &test_level);
        // printk("read_value : %d", status);
        printk("current test_level : %d", test_level);
        return count;
}
int chr_ioctl(struct inode *inode , struct file *filep, unsigned int cmd , unsigned long arg) {
        switch (cmd) {
                case 0: printk("Cmd value is %d\n" , cmd); break;
                case 4: printk("Cmd value is %d\n" , cmd ); break;
                default:
                        printk("Default : %d, arg : %lu", cmd, arg); break;
        }
        return 0;
}

int chr_release(struct inode *inode, struct file *filep) {
        printk("Virtual Character device Release\n");
        return 0;
}

struct file_operations chr_fops = {
        owner: THIS_MODULE,
        unlocked_ioctl: chr_ioctl,
        write: chr_write,
        read: chr_read,
        open: chr_open,
        release: chr_release
};
 
/*
        This function will be called on initialization of  kernel module
*/
int init_process(void)
{
        int ret = 0;
        char *msg;
        int regist;
        msg="123 123";

        proc_entry = proc_create(PROCFS_TESTLEVEL, 0666, NULL, &my_proc_fops);
        pidnum_entry = proc_create("pidnum" ,0666, NULL,&pidnum_ops);
        threshold_entry = proc_create("threshold", 0666, NULL, &threshold_ops);
        regist = register_chrdev(CHR_DEV_MAJOR, CHR_DEV_NAME, &chr_fops);
        printk("Major number : %d", regist);
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
        remove_proc_entry("threshold", threshold_entry);
        unregister_chrdev(CHR_DEV_MAJOR, CHR_DEV_NAME);
}

module_init(init_process);
module_exit(process_exit);