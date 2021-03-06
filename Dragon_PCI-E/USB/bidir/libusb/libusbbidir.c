/*
 * USB bidir test for KNJN Dragon-E
 *
 * # libusbbidir [# of bytes] (default is 5)
 *
 *   Copyright (C) 2014 Pierre Ficheux (pierre.ficheux@gmail.com)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

// Dragon-E IDs
#define VENDOR_ID       0x04b4
#define PRODUCT_ID 	0x8614

#define USB_STATUS_OK 0

void end1 (libusb_device_handle *dev) 
{
  libusb_close (dev);
  libusb_exit (NULL);
}

void end2 (libusb_device_handle *dev, struct libusb_config_descriptor *desc) 
{
  libusb_free_config_descriptor(desc);
  end1 (dev);
}

int main (int ac, char **av)
{
  libusb_device *device;
  libusb_device_handle *handle;
  struct libusb_config_descriptor *configure_descriptor;
  int err, status, l, n;
  unsigned char buf[5];

  /*==============================*/
  /*======== init libusb =========*/
  /*==============================*/
  if ((status = libusb_init(NULL)) != USB_STATUS_OK) {
    fprintf (stderr, "libusb_init error, status= %d\n", status);
    exit (1);
  }

  /*=================================*/
  /*======== find/open USB device ===*/
  /*=================================*/
  if ((handle = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID)) == NULL)
    {
      fprintf (stderr, "Could not find/open device\n");
      libusb_exit (NULL);
      exit (1);
    }
	
  if ((device = libusb_get_device (handle)) == NULL)
    {
      fprintf(stderr, "Could not get device\n");
      end1 (handle);
      exit (1);		
    }
	
  /*=========================================*/
  /*======== Get the configure desc =========*/
  /*=========================================*/
  if ((status = libusb_get_active_config_descriptor(device, &configure_descriptor)) != USB_STATUS_OK)
    {
      fprintf (stderr, "Failed to get configure descriptor, status= %d\n", status);
      end1 (handle);
      exit (1);
    }
	
  /*==========================================================================*/
  /*======== if Interface is active on the kernel ==> Detach this interface ==*/
  /*==========================================================================*/			
  if (libusb_kernel_driver_active(handle, configure_descriptor->interface[0].altsetting[0].bInterfaceNumber) == 1) 
    {
      if ((status = libusb_detach_kernel_driver(handle, configure_descriptor->interface[0].altsetting[0].bInterfaceNumber)) != USB_STATUS_OK)
	{
	  fprintf(stderr, "Failed to detach kernel driver, status= %d\n", status);
	  end2 (handle, configure_descriptor);
	  exit (1);
	}
    }

  /*=================================================*/
  /*======== Interface claim before any transfert  ==*/
  /*=================================================*/
  if ((status = libusb_claim_interface(handle, configure_descriptor->interface[0].altsetting[0].bInterfaceNumber)) != USB_STATUS_OK)
    {
      fprintf(stderr, "error interface claim, error= %d\n", status);
      end2 (handle, configure_descriptor);
      exit (1);
    }

  if ((status = libusb_set_interface_alt_setting(handle, configure_descriptor->interface[0].altsetting[0].bInterfaceNumber, 1)) != USB_STATUS_OK)
    {
      fprintf(stderr, "error alt setting, error= %d\n", status);
      end2 (handle, configure_descriptor);
      exit (1);
    }


  /*
   * Send n bytes, board replies 1 byte incremented by n each time we use the program
   */
  if (ac >= 2)
	n = atoi(av[1]);
  else 
	n = 5;

  memset (buf, 0, n);

  if ((err = libusb_bulk_transfer(handle, 0x02, buf, n, &l, 5000)))
    fprintf (stderr, "libusb_bulk_transfert error= %d\n", err);
  
  if ((err = libusb_bulk_transfer(handle, 0x86, buf, n, &l, 5000)))
    fprintf (stderr, "libusb_bulk_transfert error= %d\n", err);
  
  printf ("reply = %d\n", buf[0]);

  end2 (handle, configure_descriptor);

  return 0;
}
