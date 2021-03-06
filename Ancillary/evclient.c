#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <errno.h>

#include "ancillary.h"


// X will be replaced by '\0' post-snprintf
char *name = "Xeventfd_socket";

int main(void)
{
    struct sockaddr_un address;
    int  socket_fd;
    int evfd;

    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0)	{
		fprintf(stderr,"socket() failed: %s\n", strerror(errno));
		return 1;
    }

    memset(&address, 0, sizeof(struct sockaddr_un));

    address.sun_family = AF_UNIX;
    snprintf(address.sun_path,sizeof(address.sun_path), "%s", name);
    address.sun_path[0] = '\0';

    if (connect(socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) != 0) {
		fprintf(stderr,"connect() failed: %s\n", strerror(errno));
		return 1;
    }

    if (ancil_recv_fd(socket_fd, &evfd)) {
		perror("ancil_recv_fd");
		exit(1);
    } else {
		printf("Received eventfd on: %d\n", evfd);
    }

    if (evfd < 0) {
	printf("bad event fd\n");
	exit(1);
    }

	uint64_t u;
	ssize_t s;

    while (1) {

		printf("About to read\n");
		s = read(evfd, &u, sizeof(uint64_t));
		if (s != sizeof(uint64_t)){
			perror("read");
		}
		printf("Read %llu (0x%llx) from efd %d\n", (unsigned long long) u, (unsigned long long) u, evfd);
		

    }
    close(socket_fd);

    return 0;
}
