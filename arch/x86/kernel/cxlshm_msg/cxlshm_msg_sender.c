#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <net/sock.h>
#include <linux/inet.h>
#include <linux/types.h>

static char ip_4_addr[16] = {0};
static int port = 0;
static struct socket *client_socket;
static struct sockaddr_in client_sockaddr;

#define MAX_BUFFER_NET			128

static int tcp_client_start_impl(char *ip_4_addr, int port);
static int send_message_impl(char *message);
static int tcp_client_stop_impl(void);

SYSCALL_DEFINE2(tcp_client_start, char __user *, ip_v4_addr, int, open_port) {
	int ret = strncpy_from_user(ip_4_addr, ip_v4_addr, sizeof(ip_4_addr));
	if (ret < 0) return -EFAULT;
	if (ret >= sizeof(ip_4_addr) || ret == 0) return -EINVAL;
	port = open_port;
	return tcp_client_start_impl(ip_4_addr, open_port);
}

SYSCALL_DEFINE1(send_message, char __user *, message) {
	char message_buf[128] = {0};
	int ret = strncpy_from_user(message_buf, message, sizeof(message_buf));
	if (ret < 0) return -EFAULT;
	if (ret >= sizeof(message_buf) || ret == 0) return -EINVAL;
	return send_message_impl(message_buf);
}

SYSCALL_DEFINE0(tcp_client_stop) {
	return tcp_client_stop_impl();
}

static int tcp_client_start_impl(char *ip_4_addr, int port) {
	int ret = 0;
	if (!client_socket) {
		ret = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &client_socket);
		if (ret < 0) return ret;
		memset(&client_sockaddr, 0, sizeof(client_sockaddr));
		//initialise client socket address
		client_sockaddr.sin_family = AF_INET;
		client_sockaddr.sin_port = htons(port);
		int ret_ip = in4_pton(ip_4_addr, INET_ADDRSTRLEN, (u8 *)&client_sockaddr.sin_addr.s_addr, -1, NULL);
		if (ret_ip == 0) return -EINVAL;
		ret = client_socket->ops->connect(client_socket, (struct sockaddr *)&client_sockaddr, sizeof(client_sockaddr), 0);
		if (ret < 0) return ret;
	} else {
		pr_info("There is a client socket\n");
		ret = -1;
	}
	return ret;
}

static int send_message_impl(char *message) {
	char msg[MAX_BUFFER_NET] = {0};
	int len = strscpy(msg, message, sizeof(msg));
	pr_info("Sending message %s length %d\n", msg, len);
	int ret = 0;
	if (client_socket) {
		struct msghdr hdr;
		memset(&hdr, 0, sizeof(hdr));
		struct kvec iov = {
			.iov_base = message,
			.iov_len = sizeof(msg)
		};
		ret = kernel_sendmsg(client_socket, &hdr, &iov, 1, strlen(msg));
	}
	return ret;
}

static int tcp_client_stop_impl(void) {
	if (client_socket) {
		pr_info("Disconnect from server %s port %d\n", ip_4_addr, port);
		sock_release(client_socket);
		client_socket = NULL;
	}

	return 0;
}