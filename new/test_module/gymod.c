#include <linux/init.h>
#include <linu/module.h>

MODULE_LICENCE("Dual BSD/GPL");
static int hello_init(void) {
    printk(KERN_ALERT "Heloo");
    return 0;
}
static void hello_exit(void) {
    printk(KERN_ALERT "Goodboy");
}
module_init(hello_init);
module_exit(hellp_exit);