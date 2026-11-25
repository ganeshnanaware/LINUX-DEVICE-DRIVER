/*
Question : Write a user space application to test the character device driver
           /dev/beginner (open, read, write, close).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE "/dev/beginner"
#define CAPACITY 64

int main()
{
	int fd;
	int choice;
	int ret;
	char buf[CAPACITY + 1];
	char input[CAPACITY + 1];
	char line[16];

	fd = open(DEVICE, O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return 1;
	}

	printf("device opened, fd = %d\n", fd);

	while(1)
	{
		printf("\n1. Read\n2. Write\n3. Close and open again\n4. Exit\n");
		printf("Enter choice : ");

		if(fgets(line, sizeof(line), stdin) == NULL)
			break;

		choice = atoi(line);

		if(choice == 1)
		{
			ret = read(fd, buf, CAPACITY);

			if(ret < 0)
			{
				perror("read");
			}
			else if(ret == 0)
			{
				printf("end of data, nothing to read\n");
			}
			else
			{
				buf[ret] = '\0';
				printf("read %d bytes : %s\n", ret, buf);
			}
		}
		else if(choice == 2)
		{
			printf("Enter data : ");

			if(fgets(input, sizeof(input), stdin) == NULL)
				continue;

			input[strcspn(input, "\n")] = '\0';

			ret = write(fd, input, strlen(input));

			if(ret < 0)
				perror("write");
			else
				printf("wrote %d bytes\n", ret);
		}
		else if(choice == 3)
		{
			close(fd);

			fd = open(DEVICE, O_RDWR);
			if(fd < 0)
			{
				perror("open");
				return 1;
			}

			printf("device reopened, new session started\n");
		}
		else if(choice == 4)
		{
			break;
		}
		else
		{
			printf("invalid choice\n");
		}
	}

	close(fd);
	return 0;
}
