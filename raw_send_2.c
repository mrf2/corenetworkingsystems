/*
 * Raw Ethernet Frame Sender without libc
 * Goal: Send a handcrafted Ethernet frame using syscall() directly
 * Platform: Linux (x86_64)
 * Build: gcc -nostdlib -static -o raw_send_2 raw_send_2.c
 * Run as root: ./raw_send_2
 */

#define _GNU_SOURCE
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/if.h>
#include <linux/net.h>
#include <linux/sockios.h>
#include <stdint.h>
#include <unistd.h>

// Declare syscall numbers manually (if needed)
#define SYS_socket 41
#define SYS_sendto 44
#define SYS_close 3
#define SYS_exit 60

// Minimal helpers (no libc!)
static inline long my_syscall(long number, long a1, long a2, long a3, long a4,
		long a5, long a6)
{
	long ret;
	__asm__ volatile (
		"mov %[arg4], %%r10 \n\t"
		"mov %[arg5], %%r8 \n\t"
		"mov %[arg6], %%r9 \n\t"
		"syscall"
		: "=a" (ret)
		: "a" (number), "D" (a1), "S"(a2), "d" (a3),
	          [arg4] "r" (a4), [arg5] "r" (a5), [arg6] "r" (a6)
		: "rcx", "r11", "memory"
	);
	
	return ret;
}

long sys_socket (int domain, int type, int protocol)
{
	return my_syscall(SYS_socket, domain, type, protocol, 0, 0, 0);
}

long sys_sendto(int sockfd, const void *buf, size_t len, int flags,
		const void *dest_addr, size_t addrlen)
{
	return my_syscall(SYS_sendto, sockfd, (long) buf, len, flags, (long)dest_addr, 
			addrlen);
}

long sys_close(int fd)
{
	return my_syscall(SYS_close, fd, 0, 0, 0, 0, 0);
}

long sys_exit(int code)
{
	return my_syscall(SYS_exit, code, 0, 0, 0, 0, 0);
}

// Manual htons
static inline uint16_t htons(uint16_t x)
{
	return (x << 8) | (x >> 8);
}

void _start()
{
	/* Hardcoded MAC addresses (broadcast + fake source) */
	uint8_t frame[60] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff,	// Destination: Broadcast
		0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc,	// Source MAC: fake 
		0x08, 0x06,				// EtherType: ARP (0x0806)A
		// ARP payload (just dummy)
		0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 192, 168, 0, 2,
		0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 192, 168, 0, 1,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};

	int sockfd = sys_socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sockfd < 0) goto end;

	struct sockaddr_ll addr = {0};
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ALL);
	addr.sll_ifindex = 2;	// hardcoded for now: eth0 usually = 2, check `ip link`
	addr.sll_halen = ETH_ALEN;
	addr.sll_addr[0] = 0xff; addr.sll_addr[1] = 0xff; addr.sll_addr[2] = 0xff;
	addr.sll_addr[3] = 0xff; addr.sll_addr[4] = 0xff; addr.sll_addr[5] = 0xff;

	sys_sendto(sockfd, frame, sizeof(frame), 0, (struct sockaddr *)&addr, 
			sizeof(addr));

	sys_close(sockfd);
end:
	sys_exit(0);
}





