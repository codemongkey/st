/* 
 * TCP Client sample for wlan in Linux OS
 * Copyright (C) 2015 by Tony Cho
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "arpa/inet.h"
#include "netinet/in.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "unistd.h"

#define BUFF_SIZE 1024

int main(int argc, char *argv[]) 
{
	printf("1\n");
	int c_socket;
	struct sockaddr_in c_addr;

	char sndBuffer[BUFF_SIZE],rcvBuffer[BUFF_SIZE];

	int n;
	int n_left, n_recv;

	if(argc != 3) {
		printf("usage : %s [ServerIP] [Port]\n", argv[0]);
		exit(0);
	}

	if((c_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Clent : Can't open TCP client socket\n");
		exit(0);
	}

	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = inet_addr(argv[1]);
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(atoi(argv[2]));
printf("2\n");
	if(connect(c_socket, (struct sockaddr*) &c_addr, sizeof(c_addr)) == -1) {
		printf("Can not connect\n");
		close(c_socket);
		return -1;
	}
printf("3\n");
	while(1) {
		if((n = read(0, sndBuffer, BUFF_SIZE)) > 0) {
			sndBuffer[n] = '\0';
			if(!strcmp(sndBuffer, "quit\n"))
				break;

			printf("Send Data : %s", sndBuffer);
			if((n = write(c_socket, sndBuffer, strlen(sndBuffer))) < 0) {
				return (-1);
			}

			n_left = n;
			n_recv = 0;

			while(n_left > 0) {
				if((n = read(c_socket, &rcvBuffer[n_recv], n_left)) < 0) {
					return (-1);
				}
				n_left = n_left - n;
				n_recv = n_recv + n;
			}
			rcvBuffer[n_recv] = '\0';
			printf("Echo Data : %s", rcvBuffer);
		}
	}
	close(c_socket);
	return 0;
}
