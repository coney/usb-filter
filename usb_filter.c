#include <linux/module.h>
#include <linux/sched.h>
#include <linux/usb.h>
#include <linux/kallsyms.h>

#include "klog.h"
#include "usbf_proc.h"

MODULE_AUTHOR("Coney Wu <kunwu@thoughtworks.com>");
MODULE_LICENSE("GPL");

static struct bus_type *usb_bus_type_ptr = NULL;

//static int access_usb_device(struct usb_device *udev, void *data) {
//    loginfo("access usb dev %p", udev);
//    return 0;
//}

#define is_interface_driver(drv) (!((struct usbdrv_wrap*)(drv))->for_devices)

static int access_usb_driver(struct usb_driver *drv, void *unused)
{
    loginfo("access usb drv %p:%s fops:%p", drv, drv->name, );
    if (!strcmp(drv->name, "dummy_driver"))
    {
        drv->probe(NULL, NULL);
    }
    return 0;
}

static int access_usb_device_driver(struct usb_device_driver *drv, void *unused)
{
    loginfo("access usb dev drv %p:%s", drv, drv->name);
    return 0;
}

static int access_device_driver(struct device_driver *drv, void *unused)
{
    loginfo("access dev drv %p:%s", drv, drv->name);
    return is_interface_driver(drv) ? access_usb_driver(to_usb_driver(drv), unused) 
        : access_usb_device_driver(to_usb_device_driver(drv), unused);
}

int usb_for_each_drv(void *data, int(*fn)(struct device_driver *, void *))
{
    return bus_for_each_drv(usb_bus_type_ptr, NULL, data, fn);
}

static int dummy_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    loginfo("dummy_probe  plugged\n");
    return 0;
}

static void dummy_disconnect(struct usb_interface *interface)
{
    loginfo("dummy drive removed\n");
}

static struct usb_device_id dummy_table[] =
{
    { USB_DEVICE(0x058F, 0x6387) },
{} /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, dummy_table);

static struct usb_driver dummy_driver =
{
    .name = "dummy_driver",
    .id_table = dummy_table,
    .probe = dummy_probe,
    .disconnect = dummy_disconnect,
};

int __init usb_filter_init(void)
{
    int ret = 0;
    loginfo("load usbfilter driver from %s:%d!\n", current->comm, task_pid_nr(current));

    usb_bus_type_ptr = (struct bus_type *)kallsyms_lookup_name("usb_bus_type");
    loginfo("usb_bus_type address:%p", usb_bus_type_ptr);

    usb_for_each_drv(NULL, access_device_driver);

    if ((ret = usbf_proc_init()) != 0) {
        logerror("failed to create proc entry!\n");
        goto error;
    }

    if ((ret = usb_register(&dummy_driver)) != 0) {
        logerror("failed to register dummy usb driver!\n");
        goto usb_error;
    }

    usb_for_each_drv(NULL, access_device_driver);

    loginfo("usbfilter driver loaded!\n");
    return 0;

usb_error:
    usb_deregister(&dummy_driver);
error:
    usbf_proc_clear();

    return ret;
}

void __exit usb_filter_exit(void)
{

    loginfo("unload usbfilter driver!\n");

    usb_deregister(&dummy_driver);

    usbf_proc_clear();

    return;
}



module_init(usb_filter_init);
module_exit(usb_filter_exit);