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
const char * const kexec_load_cmd = "kexec --load /mnt/target_boot/zImage "
	"--append \"console=ttyS0,115200 vt.global_cursor_default=0 vt.cur_default=1 coherent_pool=256K isolcpus=1 root=/dev/mmcblk0p2 rw rootwait rootfstype=ext4\"";
const char * const kexec_exec_cmd = "kexec --exec";
const char * const dd_boot_cmd = "dd if=/dev/mmcblk0p3 of=/dev/mmcblk0p1 bs=1M conv=fsync";
const char * const dd_root_cmd = "dd if=/dev/mmcblk0p4 of=/dev/mmcblk0p2 bs=1M conv=fsync";
const char * const dry_run_preamble = "The following commands will be executed in order to continue the boot process:\n\n";

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

	return 0;
}

/* Wait for user to hit magic key to go into interactive recovery mode.
 */
int wait_for_keypress_timeout(int *timed_out)
{
	int retval;
	fd_set read_descriptors;
	struct timeval timeout;
	char buffer[2];
	const char magic_key = '\n';
	
	FD_ZERO(&read_descriptors);
	FD_SET(STDIN_FILENO, &read_descriptors);

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	
	do
	{
		retval = select(1, &read_descriptors, NULL, NULL, &timeout);
		if(retval < 0)
		{
			fprintf(stderr, "select error, errno=%d\n", errno);
			return retval;
		}else if(retval == 0)
		{
			*timed_out = 1;
			return 0;
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
				*timed_out = 0;
				return 0;
			}
		}
	}while (retval);

	return 0;
}

int confirm_selection(int *confirmed)
{
	char *buffer;
	size_t n;
	ssize_t retval;
	
	*confirmed = 0;

	printf("\nEnter \"y\" or \"1\" to confirm and execute commands > ");
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
	
	printf( dry_run_preamble );
	do_boot(1, 1);

	retval = confirm_selection( &confirmed );
	if(retval < 0) return retval;
	else return do_boot(1, 0);
}

int do_interactive_restore_filesystems()
{
	int confirmed;
	int retval;
	
	printf( dry_run_preamble );
	do_restore_filesystems(1, 1);

	retval = confirm_selection( &confirmed );
	if(retval < 0) return retval;
	else return do_restore_filesystems(1, 0);
}

int prompt_interactively()
{
	char *buffer;
	size_t n;
	
	do
	{
		ssize_t retval;
		
		printf("Select one of the following actions by number:\n"
			"1) Exit rescue program and continue normal boot process.\n"
			"2) Restore filesystems to factory state.\n"
			"> "
		);
		buffer = NULL;
		n = 0;
		retval = getline(&buffer, &n, stdin);
		if(retval < 0)
		{
			fprintf(stderr, "getline returned error, errno=%d\n", errno);
		}else
		{
			long selection;
			
			selection = strtol(buffer, NULL, 0);
			switch(selection)
			{
				case 1:
					do_interactive_boot();
					break;
				case 2:
					do_interactive_restore_filesystems();
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
