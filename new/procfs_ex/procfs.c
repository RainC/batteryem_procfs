

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
 



#define PFS_MODULE_VERSION "1.0"
#define MODULE_NAME "procfs_example"

#define FOOBAR_LEN 8

struct fb_data_t {
        char name[FOOBAR_LEN + 1];
        char value[FOOBAR_LEN + 1];
};


static struct proc_dir_entry *example_dir, *foo_file,
        *bar_file, *jiffies_file, *tty_device, *symlink;


struct fb_data_t foo_data, bar_data;


static int proc_read_jiffies(char *page, char **start,
                             off_t off, int count,
                             int *eof, void *data)
{
        int len;

        
        len = sprintf(page, "jiffies = %ld\n",
                      jiffies);

        // MOD_DEC_USE_COUNT;

        return len;
}


static int proc_read_foobar(char *page, char **start,
                            off_t off, int count, 
                            int *eof, void *data)
{
        int len;
        struct fb_data_t *fb_data = (struct fb_data_t *)data;

        
        len = sprintf(page, "%s = '%s'\n", 
                      fb_data->name, fb_data->value);

        // MOD_DEC_USE_COUNT;

        return len;
}


static int proc_write_foobar(struct file *file,
                             const char *buffer,
                             unsigned long count, 
                             void *data)
{
        int len;
        struct fb_data_t *fb_data = (struct fb_data_t *)data;


        if(count > FOOBAR_LEN)
                len = FOOBAR_LEN;
        else
                len = count;

        if(copy_from_user(fb_data->value, buffer, len)) {
                // MOD_DEC_USE_COUNT;
                return -EFAULT;
        }

        fb_data->value[len] = '\0';

        // MOD_DEC_USE_COUNT;

        return len;
}


static int __init init_procfs_example(void)
{
        int rv = 0;

        /* create directory */
        example_dir = proc_mkdir(MODULE_NAME, NULL);
        if(example_dir == NULL) {
                rv = -ENOMEM;
                goto out;
        }
        
        example_dir->owner = THIS_MODULE;
        
        /* create jiffies using convenience function */
        jiffies_file = create_proc_read_entry("jiffies", 
                                              0444, example_dir, 
                                              proc_read_jiffies,
                                              NULL);
        if(jiffies_file == NULL) {
                rv  = -ENOMEM;
                goto no_jiffies;
        }

        jiffies_file->owner = THIS_MODULE;

        /* create foo and bar files using same callback
         * functions 
         */
        foo_file = create_proc_entry("foo", 0644, example_dir);
        if(foo_file == NULL) {
                rv = -ENOMEM;
                goto no_foo;
        }

        strcpy(foo_data.name, "foo");
        strcpy(foo_data.value, "foo");
        foo_file->data = &foo_data;
        foo_file->read_proc = proc_read_foobar;
        foo_file->write_proc = proc_write_foobar;
        foo_file->owner = THIS_MODULE;
                
        bar_file = create_proc_entry("bar", 0644, example_dir);
        if(bar_file == NULL) {
                rv = -ENOMEM;
                goto no_bar;
        }

        strcpy(bar_data.name, "bar");
        strcpy(bar_data.value, "bar");
        bar_file->data = &bar_data;
        bar_file->read_proc = proc_read_foobar;
        bar_file->write_proc = proc_write_foobar;
        bar_file->owner = THIS_MODULE;
                
        /* create tty device */
        tty_device = proc_mknod("tty", S_IFCHR | 0666,
                                example_dir, MKDEV(5, 0));
        if(tty_device == NULL) {
                rv = -ENOMEM;
                goto no_tty;
        }
        
        tty_device->owner = THIS_MODULE;

        /* create symlink */
        symlink = proc_symlink("jiffies_too", example_dir, 
                               "jiffies");
        if(symlink == NULL) {
                rv = -ENOMEM;
                goto no_symlink;
        }

        symlink->owner = THIS_MODULE;

        /* everything OK */
        printk(KERN_INFO "%s %s initialised\n",
               MODULE_NAME, MODULE_VERSION);
        return 0;

no_symlink:
        remove_proc_entry("tty", example_dir);
no_tty:
        remove_proc_entry("bar", example_dir);
no_bar:
        remove_proc_entry("foo", example_dir);
no_foo:
        remove_proc_entry("jiffies", example_dir);
no_jiffies:                           
        remove_proc_entry(MODULE_NAME, NULL);
out:
        return rv;
}


static void __exit cleanup_procfs_example(void)
{
        remove_proc_entry("jiffies_too", example_dir);
        remove_proc_entry("tty", example_dir);
        remove_proc_entry("bar", example_dir);
        remove_proc_entry("foo", example_dir);
        remove_proc_entry("jiffies", example_dir);
        remove_proc_entry(MODULE_NAME, NULL);

        printk(KERN_INFO "%s %s removed\n",
               MODULE_NAME, MODULE_VERSION);
}


module_init(init_procfs_example);
module_exit(cleanup_procfs_example);


MODULE_AUTHOR("Erik Mouw");
MODULE_DESCRIPTION("procfs examples");

EXPORT_NO_SYMBOLS;
