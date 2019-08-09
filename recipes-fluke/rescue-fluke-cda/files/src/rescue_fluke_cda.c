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

#include <stdio.h>
#include <stdlib.h>

const char * const mount_cmd = "mount -o ro /dev/mmcblk0p1 /mnt/target_boot";
const char * const kexec_load_cmd = "kexec --load /mnt/target_boot/zImage "
	"--append \"console=ttyS0,115200 vt.global_cursor_default=0 vt.cur_default=1 coherent_pool=256K isolcpus=1 root=/dev/mmcblk0p2 rw rootwait rootfstype=ext4\""
const char * const kexec_exec_cmd = "kexec --exec"

int system_wrapper(const char *command)
{
	int result;
	
	printf("Executing command:\n");
	printf("%s\n", command);
	result = system(command);
	if(result)
	{
		fprintf(stderr, "Error code = %d\n", result);
	}
	return result;
}

int main()
{
	int result;
	
	printf("Press enter.\n");
	getchar();

	result = system_wrapper(mount_cmd);
	if(result)
	{
		return result;
	}
	
	result = system_wrapper(kexec_load_cmd);
	if(result)
	{
		return result;
	}

	result = system_wrapper(kexec_exec_cmd);
	if(result)
	{
		return result;
	}

	return 0;
}
