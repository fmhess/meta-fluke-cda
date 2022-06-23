/*

Interactive rescue program for fluke-cda
	
Copyright (C) 2019 Fluke Corporation

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

const char * const mount_cmd = "mount -o ro /dev/mmcblk0p1 /mnt/target_boot";

//#define RESCUE_FLUKE_CDA_CONSOLE_BOOTARGS "console=ttyS0,115200"
// #define RESCUE_FLUKE_CDA_KEXEC_ERROR_OUT "/dev/console"
#define RESCUE_FLUKE_CDA_CONSOLE_BOOTARGS "console=null"
#define RESCUE_FLUKE_CDA_KEXEC_ERROR_OUT "/dev/null"
const char * const kexec_load_cmd = "kexec --load /mnt/target_boot/zImage "
	"--append \"" RESCUE_FLUKE_CDA_CONSOLE_BOOTARGS " vt.global_cursor_default=0 vt.cur_default=1 coherent_pool=256K isolcpus=1 root=/dev/mmcblk0p2 rw rootwait rootfstype=ext4\" "
	"2>" RESCUE_FLUKE_CDA_KEXEC_ERROR_OUT ;
const char * const kexec_exec_cmd = "kexec --exec 2>" RESCUE_FLUKE_CDA_KEXEC_ERROR_OUT ;
const char * const dd_boot_cmd = "dd if=/dev/mmcblk0p3 of=/dev/mmcblk0p1 bs=1M conv=fsync";
const char * const dd_root_cmd = "dd if=/dev/mmcblk0p4 of=/dev/mmcblk0p2 bs=1M conv=fsync";
const char * const dd_mtd_to_tmp_cmd = "dd if=/dev/mtd%d of=/tmp/mtdX bs=1M";
const char * const flashcp_tmp_to_mtd_cmd = "flashcp /tmp/mtdX /dev/mtd%d";
const int uboot_partition = 0;
const int uboot_environment_partition = 1;
const int device_tree_partition = 2;
const int kernel_partition = 3;
const int fpga_partition = 4;
const int initrd_partition = 5;
const int uboot_backup_partition = 6;
const int uboot_environment_backup_partition = 7;
const int device_tree_backup_partition = 8;
const int kernel_backup_partition = 9;
const int fpga_backup_partition = 10;
const int initrd_backup_partition = 11;
const char * const main_menu_options[] = 
{
	"Exit rescue program and continue normal boot process",
	"Restore filesystems to factory state",
	"Restore flattened device tree to factory state",
	"Restore FPGA RBF image to factory state",
	"Restore U-Boot with SPL to factory state",
	"Restore U-Boot environment to factory state",
	"Restore rescue kernel and initrd to factory state",
	NULL
};

int system_wrapper(const char *command, int verbose, int dry_run)
{
	int result;
	
	if(verbose)
	{
		printf("%s\n", command);
	}
	if(dry_run == 0)
	{
		result = system(command);
		if(result)
		{
			fprintf(stderr, "Error code = %d\n", result);
		}
	}else
	{
		result = 0;
	}
	return result;
}

int system_dd_mtd_to_tmp(int source_mtd_partition, int verbose, int dry_run)
{
	const size_t command_size = 0x1000;
	char * const command = malloc(command_size);
	int result;
	
	if (command == NULL) return -1;
	
	snprintf(command, command_size, dd_mtd_to_tmp_cmd, source_mtd_partition);
	result = system_wrapper(command, verbose, dry_run);
	free(command);
	return result;
}

int system_flashcp_tmp_to_mtd(int destination_mtd_partition, int verbose, int dry_run)
{
	const size_t command_size = 0x1000;
	char * const command = malloc(command_size);
	int result;
	
	if (command == NULL) return -1;
	
	snprintf(command, command_size, flashcp_tmp_to_mtd_cmd, destination_mtd_partition);
	result = system_wrapper(command, verbose, dry_run);
	free(command);
	return result;
}

int do_boot(int verbose, int dry_run)
{
	int result;
	
	result = system_wrapper(mount_cmd, verbose, dry_run);
	if(result)
	{
		return result;
	}
	
	result = system_wrapper(kexec_load_cmd, verbose, dry_run);
	if(result)
	{
		return result;
	}

	result = system_wrapper(kexec_exec_cmd, verbose, dry_run);
	if(result)
	{
		return result;
	}
	
	return 0;
}

void print_success()
{
	printf("Operation completed successfully.\n");
}

int do_restore_filesystems(int verbose, int dry_run)
{
	int result;
	
	result = system_wrapper(dd_boot_cmd, verbose, dry_run);
	if(result)
	{
		return result;
	}
	
	result = system_wrapper(dd_root_cmd, verbose, dry_run);
	if(result)
	{
		return result;
	}

	print_success();
	return 0;
}

int do_restore_mtd_partition(int backup_mtd_partition, int destination_mtd_partition, int verbose, int dry_run)
{
	int result;
	
	result = system_dd_mtd_to_tmp(backup_mtd_partition, verbose, dry_run);
	if(result)
	{
		return result;
	}

	result = system_flashcp_tmp_to_mtd(destination_mtd_partition, verbose, dry_run);
	if(result)
	{
		return result;
	}
	
	print_success();
	return 0;
}

/* Wait for user to hit magic key to go into interactive recovery mode.
 */
int wait_for_keypress_timeout(int *timed_out)
{
	int retval;
	fd_set read_descriptors;
	struct timeval one_second_timeval;
	const int timeout_seconds = 4;
	int i;
	char buffer[2];
	const char magic_key = '\n';
	
	*timed_out = 0;

	for (i = timeout_seconds; i > 0; --i)
	{
		// print a countdown
		printf("\r%i ", i);
		fflush(stdout);
		
		one_second_timeval.tv_sec = 1;
		one_second_timeval.tv_usec = 0;	
		do
		{
			FD_ZERO(&read_descriptors);
			FD_SET(STDIN_FILENO, &read_descriptors);

			retval = select(1, &read_descriptors, NULL, NULL, &one_second_timeval);
			if(retval < 0)
			{
				fprintf(stderr, "select error, errno=%d\n", errno);
				return retval;
			}else if(retval == 0)
			{
				break;
			}else
			{
				ssize_t read_retval;
				read_retval = read(STDIN_FILENO, buffer, 1);
				if(read_retval < 0)
				{
					fprintf(stderr, "read error, errno=%d\n", errno);
					return read_retval;
				}else if(buffer[0] == magic_key)
				{
					return 0;
				}
			}
		}while (retval);
	}

	*timed_out = 1;
	
	return 0;
}

int confirm_selection(int *confirmed)
{
	char *buffer;
	size_t n;
	ssize_t retval;
	
	*confirmed = 0;

	printf("Enter \"y\" or \"1\" to confirm and execute commands > ");
	buffer = NULL;
	n = 0;
	retval = getline(&buffer, &n, stdin);
	if(retval < 0)
	{
		fprintf(stderr, "getline returned error, errno=%d\n", errno);
		return -1;
	}else if(retval >= 1)
	{
		switch(buffer[0])
		{
		case '1':
			// fall-through
		case 'y':
			// fall-through
		case 'Y':
			*confirmed = 1;
			break;
		default:
			printf("Execution NOT confirmed.\n");
			break;
		}
	}else
	{
		printf("Execution NOT confirmed.\n");
	}
	free(buffer);
	
	return 0;
}

int do_interactive_boot()
{
	int confirmed;
	int retval;
	
	retval = confirm_selection( &confirmed );
	if(retval < 0)
	{
		return retval;
	} else if (confirmed) 
	{
		return do_boot(0, 0);
	} else 
	{
		return 0;
	}
}

int do_interactive_restore_filesystems()
{
	int confirmed;
	int retval;
	
	retval = confirm_selection( &confirmed );
	if(retval < 0) 
	{
		return retval;
	} else if (confirmed)
	{
		return do_restore_filesystems(0, 0);
	} else 
	{
		return 0;
	}
}

int do_interactive_restore_mtd_partition(int backup_partition, int destination_partition, const char *partition_description)
{
	int confirmed;
	int retval;
	
	retval = confirm_selection( &confirmed );
	if(retval < 0)
	{
		return retval;
	} else if (confirmed) 
	{
		return do_restore_mtd_partition(backup_partition, destination_partition, 0, 0);
	} else 
	{
		return 0;
	}
}

int do_interactive_restore_device_tree()
{
	return do_interactive_restore_mtd_partition(device_tree_backup_partition, device_tree_partition, "flattened device tree");
}

int do_interactive_restore_fpga()
{
	return do_interactive_restore_mtd_partition(fpga_backup_partition, fpga_backup_partition, "FPGA RBF image");
}

int do_interactive_restore_uboot()
{
	return do_interactive_restore_mtd_partition(uboot_backup_partition, uboot_partition, "U-Boot with SPL");
}

int do_interactive_restore_uboot_environment()
{
	return do_interactive_restore_mtd_partition(uboot_environment_backup_partition, uboot_environment_partition, "U-Boot environment");
}

int do_interactive_restore_rescue()
{
	int confirmed;
	int retval;
	
	retval = confirm_selection( &confirmed );
	if(retval < 0)
	{
		return retval;
	} else if (confirmed) 
	{
		retval = do_restore_mtd_partition(kernel_backup_partition, kernel_partition, 0, 0);
		if (retval) return retval;
		return do_restore_mtd_partition(initrd_backup_partition, initrd_partition, 0, 0);
	} else 
	{
		return 0;
	}
}

int prompt_interactively()
{
	char *buffer;
	size_t n;
	do
	{
		ssize_t retval;
		int i;
		int num_main_menu_options; 
		
		printf("\nSelect one of the following actions by number:\n");
		for(i = 0; main_menu_options[i] != NULL; ++i)
		{
			printf("%d) %s.\n", i + 1, main_menu_options[i]);
		}
		num_main_menu_options = i;
		printf("> ");
		
		buffer = NULL;
		n = 0;
		retval = getline(&buffer, &n, stdin);
		if(retval < 0)
		{
			free(buffer);  // according to getline docs, buffer should be freed even on failure
			fprintf(stderr, "getline returned error, errno=%d\n", errno);
		}else
		{
			long selection;
			
			selection = strtol(buffer, NULL, 0);
			if(selection > 0 && selection <= num_main_menu_options)
			{
				printf("\nYou have selected \"%s\".\n", main_menu_options[selection - 1]);
			}
			switch(selection)
			{
				case 1:
					do_interactive_boot();
					break;
				case 2:
					do_interactive_restore_filesystems();
					break;
				case 3:
					do_interactive_restore_device_tree();
					break;
				case 4:
					do_interactive_restore_fpga();
					break;
				case 5:
					do_interactive_restore_uboot();
					break;
				case 6:
					do_interactive_restore_uboot_environment();
					break;
				case 7:
					do_interactive_restore_rescue();
					break;
				default:
					printf("Invalid selection.\n");
					break;
			}
		}
		free(buffer);
		
	} while(1);
		
	return 0;
}

int main()
{
	int result;
	int timed_out;
	
	/* cbreak lets us read each character as the key is pressed, rather
	 * than it buffering until a newline is received. */
	system("stty cbreak -echo");
	
	result = wait_for_keypress_timeout(&timed_out);
	if(result)
	{
		return result;
	}
	if(timed_out)
	{
		return do_boot(0, 0);
	}
	
	/* reset the terminal now that we are past the initial wait for the 
	 * magic keypress which drops us into the interactive rescue program. */
	system("reset");
	
	printf("Fluke CDA Rescue Program\n");

	return prompt_interactively();
}
