#include <linux/module.h>

#include "klog.h"
#include "usbf_proc.h"

MODULE_AUTHOR("Coney Wu <kunwu@thoughtworks.com>");
MODULE_LICENSE("GPL");

int __init usb_filter_init(void)
{
    int ret = 0;
    loginfo("load usbfilter driver!\n");

    if ((ret = usbf_proc_init()) != 0) {
        logerror("failed to create proc entry!\n");
        goto error;
    }

    loginfo("usbfilter driver loaded!\n");
    return 0;

error:
    usbf_proc_clear();

    return ret;
}

void __exit usb_filter_exit(void)
{
    loginfo("unload usbfilter driver!\n");

    usbf_proc_clear();

    return;
}



module_init(usb_filter_init);
module_exit(usb_filter_exit);