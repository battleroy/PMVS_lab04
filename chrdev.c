#include <linux/module.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>

#define MY_MAJOR  200
#define MY_MINOR  0
#define MY_DEV_COUNT 4
#define SIZE_OF_MSG 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BSU");
MODULE_DESCRIPTION("calc");

static int     my_open( struct inode *, struct file * );
static ssize_t my_read( struct file * ,        char *  , size_t, loff_t *);
static ssize_t my_write(struct file * , const  char *  , size_t, loff_t *);
static int     my_close(struct inode *, struct file * );
struct file_operations my_fops = {
        read    :       my_read,
        write   :       my_write,
        open    :       my_open,
        release :       my_close,
        owner   :       THIS_MODULE
};
struct cdev my_cdev;
static char *msg = NULL;

static long num1, num2, result;
static int bad_operands[2];
static char sign;
static int already_sent;


int init_module(void)
{
	dev_t devno;
	unsigned int count = MY_DEV_COUNT;
	int err;
	devno = MKDEV(MY_MAJOR, MY_MINOR);
	register_chrdev_region(devno, count , "calcdev");

	cdev_init(&my_cdev, &my_fops);
	my_cdev.owner = THIS_MODULE;
	err = cdev_add(&my_cdev, devno, count);

	if(err < 0){
		printk("Device Add Error\n");
		return -1;
	}

	printk("Hello World. This is my first char dev.\n");
	printk("'mknod /dev/calcdev_0 c %d 0'.\n", MY_MAJOR);
	printk("'mknod /dev/calcdev_1 c %d 1'.\n", MY_MAJOR);
	printk("'mknod /dev/calcdev_2 c %d 2'.\n", MY_MAJOR);
	printk("'mknod /dev/calcdev_3 c %d 3'.\n", MY_MAJOR);

	msg = (char *)kmalloc(SIZE_OF_MSG, GFP_KERNEL);
	if(msg != NULL)
		printk("malloc allocator address: 0x%p\n", msg);

	num1 = num2 = result = sign = 0;
	bad_operands[0] = bad_operands[1] = 0;
	already_sent = 0;

	return 0;
}


void cleanup_module(void)
{
	dev_t devno;
	printk("Goodbye\n");

	if(msg)
		kfree(msg);

	devno = MKDEV(MY_MAJOR, MY_MINOR);
	unregister_chrdev_region(devno, MY_DEV_COUNT);
	cdev_del(&my_cdev);
}


static int my_open(struct inode *inod, struct file *fil)
{
	int major;
	int minor;
	major = imajor(inod);
	minor = iminor(inod);
	printk("\n*****Some body is opening me at major %d  minor %d*****\n",major, minor);
	return 0;
}

static int check_bad_operands(void)
{
	int i;
	for(i = 0; i < 2; ++i) {
		if(bad_operands[i])
			return 1;
	}
	return 0;
}


static int calc_result(void)
{
	int err_code = 0;
	switch(sign) {
	case '+':
		result = num1 + num2;
		break;
	case '-':
		result = num1 - num2;
		break;
	case '*':
		result = num1 * num2;
		break;
	case '/':
		if(num2 == 0) {
			err_code = 1;
		} else {
			result = num1 / num2;
		}
		break;
	default:
		err_code = 2;
		break;
	}
	return err_code;
}


static ssize_t my_read(struct file *filp, char *buff, size_t len, loff_t *off)
{
	int major, minor;
	int err_code = 0;
	short count;

	major = MAJOR(file_inode(filp)->i_rdev);
	minor = MINOR(file_inode(filp)->i_rdev);
	printk("FILE OPERATION READ:%d:%d\n", major, minor);
	memset(msg, 0, 32);

	switch(minor) {
		case 0:
			snprintf(msg, SIZE_OF_MSG, "num1: %ld\n", num1);
			break;
		case 1:
			snprintf(msg, SIZE_OF_MSG, "num2: %ld\n", num2);
			break;
		case 2:
			snprintf(msg, SIZE_OF_MSG, "sign: %c\n", sign);
			break;
		case 3:	
			if(!check_bad_operands()) {
				err_code = calc_result();
				if(err_code == 1) {
					snprintf(msg, SIZE_OF_MSG, "Division by zero!\n");
				} else if(err_code == 2) {
					snprintf(msg, SIZE_OF_MSG, "Bad sign!\n");
				} else {
					snprintf(msg, SIZE_OF_MSG, "result: %ld\n", result);
				}
			}
			else {
				snprintf(msg, SIZE_OF_MSG, "Bad operands!\n");
			}
			break;
		default:
			len = 0;
			break;
	}
	count = copy_to_user(buff, msg, len);

	if(already_sent) {
		already_sent = 0;
		return 0;
	}
	already_sent = 1;
	return SIZE_OF_MSG;
}


static ssize_t my_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
	int major,minor;
	short count;

	memset(msg, 0, 32);
	major = MAJOR(file_inode(filp)->i_rdev);
	minor = MINOR(file_inode(filp)->i_rdev);

	count = copy_from_user( msg, buff, len );

	printk("FILE OPERATION WRITE:%d:%d\n",major,minor);
	printk("msg: %s", msg);

	switch(minor) {
		case 0:
			if(kstrtol(msg, 10, &num1)) {
				bad_operands[0] = 1;
				printk("Bad operand in /dev/calcdev_0\n");
			} else {
				bad_operands[0] = 0;
				printk("Num1: %ld\n", num1);
			}
			break;
		case 1:
			if(kstrtol(msg, 10, &num2)) {
				bad_operands[1] = 1;
				printk("Bad operand in /dev/calcdev_1\n");
			} else {
				bad_operands[1] = 0;
				printk("Num2: %ld\n", num2);
			}
			break;
		case 2:
			sign = msg[0];
			printk("Sign: %d\n", (int)sign);
			break;
		default:
			break;
	}

	memset(msg, 0, 32);
	return len;
}


static int my_close(struct inode *inod, struct file *fil)
{
	int minor;
	minor = MINOR(file_inode(fil)->i_rdev);
	printk("*****Some body is closing me at major %d*****\n",minor);
	return 0;
}