// ffs_handler_ioctl_rw.c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/socket.h>
#include <linux/kthread.h>
#include <linux/delay.h> // untuk msleep()
#include <linux/in.h>
#include <net/sock.h>
#include <linux/inet.h>
#include <linux/types.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/spinlock.h>

#define DEVICE_NAME             "ffs_sync"
#define CLASS_NAME              "ffs_class"
#define DUMMY_FILE_PATH         "undefined file path, pls setup using ioctl"
#define FILE_PATH_LENGTH        128
#define OPEN_TCP_PORT           57580
#define MAX_BUFFER_NET          128
#define DEFAULT_PORT            57580
#define COMMAND_LENGTH          4

#define IOCTL_MAGIC             0xCD
#define IOCTL_SET_FILE_PATH     _IOW(IOCTL_MAGIC, 0x01, struct famfs_sync_control_struct)
#define IOCTL_SETUP_NETWORK     _IOW(IOCTL_MAGIC, 0x02, struct famfs_sync_control_struct)
#define IOCTL_TEST_NETWORK      _IOW(IOCTL_MAGIC, 0x69, struct famfs_sync_control_struct)//temporary


struct famfs_sync_control_struct {
	char path[FILE_PATH_LENGTH + 1];
	int port;
};

static DEFINE_SPINLOCK(ctr_lock);
static char *commands[] = {"SBGN", "REND", "SACK", "SNCK", NULL};
static char ffs_file_path[FILE_PATH_LENGTH + 1];
static int path_length;
static dev_t dev_num;
static struct cdev ffs_cdev;
static struct class *ffs_class;
static struct socket *server_socket;
static struct sockaddr_in sin;
static struct task_struct *my_kthread;
static int port = 57580;
static wait_queue_head_t wq;
static int ready = 0;
char message[MAX_BUFFER_NET] = {0};
int accept_connection(void *socket_in);
int check_commands(char *message);

int check_commands(char *message) {
	int result = -1;
	for (int i = 0; commands[i] != NULL; i++) {
		if (!strncmp(message, commands[i], 4)) {
			result = i;
			break;
		}
	}
	return result;
}

static int tcp_server_start(void) {
	int ret = 0;
	if (!server_socket) {
		pr_info("Start TCP server on port %d\n", port);
		
		//initialise socket address
		memset(&sin, 0, sizeof(sin));
		sin.sin_addr.s_addr = INADDR_ANY;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);

		//create socket, bind, and listen
		ret = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &server_socket);
		if (ret < 0) return ret;
		ret = server_socket->ops->bind(server_socket, (struct sockaddr *)&sin, sizeof(sin));
		if (ret < 0) return ret;
		ret = server_socket->ops->listen(server_socket, 1);
		if (ret < 0) return ret;

		//accept connection inkernel_sendmsg separate thread
		my_kthread = kthread_run(accept_connection, (void *)server_socket, "accept_connection");
	}
	return ret;
}

int accept_connection(void *socket_in) {
	int ret_val = 0;
	struct socket *srv_socket = (struct socket *)socket_in;
	struct socket *new_socket;
	char buf[MAX_BUFFER_NET] = {0};
	pr_info("Waiting for connection\n");
	while(!kthread_should_stop()) {
		kernel_accept(srv_socket, &new_socket, 0);
		if (new_socket) {
			struct sockaddr_in connected_client_addr;
			struct msghdr hdr;
			memset(&hdr, 0, sizeof(hdr));
			struct kvec iov = {
				.iov_base = buf,
				.iov_len = sizeof(buf) - 1
			};
			kernel_getpeername(new_socket, (struct sockaddr *)&connected_client_addr);
			pr_info("Connected! client: %pI4\n", &connected_client_addr.sin_addr);
			int len = -1;
			for(;;) {
				len = kernel_recvmsg(new_socket, &hdr, &iov, 1, sizeof(buf) - 1, 0);
				if (len > 0) {
					spin_lock(&ctr_lock);
					memset(message, 0, sizeof(message));
					strscpy(message, buf, sizeof(buf));
					pr_info("is a command? %d\n", check_commands(message));
					if (check_commands(message) != -1)
						ready = 1;
					spin_unlock(&ctr_lock);
					wake_up_interruptible(&wq);
					pr_info("Data: %s\n", buf);
				} else if (len == 0) {
					pr_info("Client closed connection.\n");
					break;
				} else if (len == -EAGAIN) {
					msleep(10);
					int accept_connection(void *socket_in);
				} else {
					ret_val = len;
					break;
				}
				//overwrite buf data with NULL char
				memset(buf, 0, sizeof(buf));			
			}
			sock_release(new_socket);
			new_socket = NULL;
			pr_info("Done receiving data\n");
		}
	}
	pr_info("Acceptor thread exit. Bye\n");
	return ret_val;
}

static void tcp_server_stop(void) {
	if (server_socket) {
		pr_info("Release server socket on port %d\n", OPEN_TCP_PORT);
		sock_release(server_socket);
		server_socket = NULL;
	}
}

static long ffs_helper_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	struct famfs_sync_control_struct rw;

	if (copy_from_user(&rw, (void __user *)arg, sizeof(rw)))
		return -EFAULT;

	pr_info("Path: %s\n", rw.path);

	switch (cmd) {
		case IOCTL_SET_FILE_PATH:
			path_length = strscpy(ffs_file_path, rw.path, FILE_PATH_LENGTH);
			pr_info("%d char copied to file_path. File path: %s\n", path_length, ffs_file_path);
			break;
		case IOCTL_SETUP_NETWORK:
			if (!rw.port) return -EINVAL;
			port = rw.port;
			pr_info("Server port: %d\n", port);
			tcp_server_stop();
			tcp_server_start();
			break;
		default:
			return -ENOTTY;
	}

	return 0;
}

static unsigned int ffs_helper_poll(struct file *file, poll_table *poll) {
	poll_wait(file, &wq, poll);
	if (ready) {
		return POLLIN | POLLRDNORM; // Data ready to read
	}
	return -EAGAIN;
}

static long int ffs_helper_read(struct file *file, char __user *buf, size_t length, loff_t *offset) {
	if (!ready) 
		return -EAGAIN;
	spin_lock(&ctr_lock);
	ready = !ready;
	int msg_size = sizeof(message);
	if (copy_to_user(buf, message, msg_size) != 0) {
		return -EFAULT;
	}
	memset(message, 0, sizeof(message));
	spin_unlock(&ctr_lock);
	return msg_size;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = ffs_helper_ioctl,
	.poll = ffs_helper_poll,
	.read = ffs_helper_read
};

static int __init ffs_helper_init(void) {	
	//init char device
	alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	cdev_init(&ffs_cdev, &fops);
	cdev_add(&ffs_cdev, dev_num, 1);
	ffs_class = class_create(CLASS_NAME);
	device_create(ffs_class, NULL, dev_num, NULL, DEVICE_NAME);

	//init tcp and poll
	tcp_server_start();
	init_waitqueue_head(&wq);

	//init others
	strscpy(ffs_file_path, DUMMY_FILE_PATH, 64);
	pr_info("famfs_sync_helper: loaded\n");
	pr_info("%s\n", ffs_file_path);

	return 0;
}

static void __exit ffs_helper_exit(void) {
	//stopping tcp connection stuff
	kthread_stop(my_kthread);
	tcp_server_stop();

	//destroying char devices
	device_destroy(ffs_class, dev_num);
	class_destroy(ffs_class);
	cdev_del(&ffs_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("famfs_sync_helper: unloaded\n"); 
}

module_init(ffs_helper_init);
module_exit(ffs_helper_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("FAMFS sync helper for multi-host configuration (r/w for all, not only master)");
