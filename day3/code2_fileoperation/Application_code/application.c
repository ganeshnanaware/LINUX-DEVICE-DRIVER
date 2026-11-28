/*
Question : Write a user space application to test the character device driver
           /dev/beginner using open, read, write, lseek, ioctl and close.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/beginner"
#define CAPACITY 64

/* ioctl commands, must be same as in the driver */
#define BEGINNER_MAGIC		'B'
#define BEGINNER_CLEAR		_IO(BEGINNER_MAGIC, 0)
#define BEGINNER_GET_LENGTH	_IOR(BEGINNER_MAGIC, 1, unsigned int)
#define BEGINNER_SET_LENGTH	_IOW(BEGINNER_MAGIC, 2, unsigned int)
#define BEGINNER_GET_CAPACITY	_IOR(BEGINNER_MAGIC, 3, unsigned int)

int main()
{
	int fd;
	int choice;
	int whence;
	int ret;
	unsigned int value;
	off_t offset;
	off_t pos;
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
		printf("\n1. Read\n2. Write\n3. Seek\n4. Show position\n");
		printf("5. Close and open again\n");
		printf("6. ioctl : Clear buffer\n7. ioctl : Get length\n");
		printf("8. ioctl : Set length\n9. ioctl : Get capacity\n");
		printf("10. Exit\n");
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
			printf("1. From start\n2. From current position\n3. From end of data\n");
			printf("Enter whence : ");

			if(fgets(line, sizeof(line), stdin) == NULL)
				continue;

			ret = atoi(line);

			if(ret == 1)
				whence = SEEK_SET;
			else if(ret == 2)
				whence = SEEK_CUR;
			else if(ret == 3)
				whence = SEEK_END;
			else
			{
				printf("invalid whence\n");
				continue;
			}

			printf("Enter offset : ");

			if(fgets(line, sizeof(line), stdin) == NULL)
				continue;

			offset = atol(line);

			pos = lseek(fd, offset, whence);

			if(pos < 0)
				perror("lseek");
			else
				printf("position moved to %ld\n", (long)pos);
		}
		else if(choice == 4)
		{
			pos = lseek(fd, 0, SEEK_CUR);

			if(pos < 0)
				perror("lseek");
			else
				printf("current position = %ld\n", (long)pos);
		}
		else if(choice == 5)
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
		else if(choice == 6)
		{
			ret = ioctl(fd, BEGINNER_CLEAR);

			if(ret < 0)
				perror("ioctl CLEAR");
			else
				printf("buffer cleared\n");
		}
		else if(choice == 7)
		{
			ret = ioctl(fd, BEGINNER_GET_LENGTH, &value);

			if(ret < 0)
				perror("ioctl GET_LENGTH");
			else
				printf("valid data length = %u\n", value);
		}
		else if(choice == 8)
		{
			printf("Enter new length (0 to %d) : ", CAPACITY);

			if(fgets(line, sizeof(line), stdin) == NULL)
				continue;

			value = (unsigned int)atoi(line);

			ret = ioctl(fd, BEGINNER_SET_LENGTH, &value);

			if(ret < 0)
				perror("ioctl SET_LENGTH");
			else
				printf("length set to %u\n", value);
		}
		else if(choice == 9)
		{
			ret = ioctl(fd, BEGINNER_GET_CAPACITY, &value);

			if(ret < 0)
				perror("ioctl GET_CAPACITY");
			else
				printf("buffer capacity = %u\n", value);
		}
		else if(choice == 10)
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
