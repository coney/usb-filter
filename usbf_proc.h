/**
  * @file   usbf_proc.h
  * @brief  usbfilter procfs header
  * @author ConeyWu
  * @date   10/01/2014
  */
#ifndef __USBF_PROC_H__
#define __USBF_PROC_H__

#define USBF_PROC_NAME            "twusbfilter"
#define USBF_PROC_CONFIG          "config"
#define USBF_PROC_TEST            "test"
#define USBF_PROC_BUFSIZE         2048

int usbf_proc_init(void);
void usbf_proc_clear(void);

#endif // __USBF_PROC_H__
