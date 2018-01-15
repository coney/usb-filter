#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/export.h>

#include "klog.h"
#include "usbf_proc.h"

static char usbf_proc_buffer[USBF_PROC_BUFSIZE];

static int usbf_proc_test_show(struct seq_file *m, void *v) {
    logdebug("proc read (/proc/%s) called\n", USBF_PROC_TEST);
    seq_printf(m, "Hello Kitty!\n");
    return 0;
}

static int usbf_proc_test_open(struct inode *inode, struct file *file) {
    return single_open(file, usbf_proc_test_show, NULL);
}

static ssize_t usbf_proc_test_write(struct file *file, const char *buffer, size_t count, loff_t *offset) {
    logdebug("write %zu bytes, offset %p\n", count, offset);
    if (count >= USBF_PROC_BUFSIZE) {
        count = USBF_PROC_BUFSIZE - 1;
    }

    if (copy_from_user(usbf_proc_buffer, buffer, count)) {
        return -EFAULT;
    }

    usbf_proc_buffer[count] = 0;

    logdebug("receive command %s\n", usbf_proc_buffer);

    return count;
}

static const struct file_operations usbf_proc_config_fops = {
        .owner = THIS_MODULE,
        .open = usbf_proc_test_open,
        .read = seq_read,
        .write  = usbf_proc_test_write,
        .llseek = seq_lseek,
        .release = single_release,
};



static struct proc_dir_entry *usbf_proc_root = NULL;

int usbf_proc_init(void) {

    if ((usbf_proc_root = proc_mkdir_mode(USBF_PROC_NAME, S_IALLUGO, NULL)) == NULL
        || !proc_create(USBF_PROC_TEST, S_IALLUGO, usbf_proc_root, &usbf_proc_config_fops)) {
            usbf_proc_clear();
            return -ENOMEM;
    }

    return 0;
}

void usbf_proc_clear(void) {
    if (usbf_proc_root) {
        remove_proc_entry(USBF_PROC_TEST, usbf_proc_root);
        remove_proc_entry(USBF_PROC_NAME, NULL);
        usbf_proc_root = 0;
    }
}
