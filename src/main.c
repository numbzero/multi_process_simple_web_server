#include "web_server.h"

void	proc_exit()
{
	int 	child_status;
	pid_t	pid;
	while ((pid = waitpid(-1, &child_status, WNOHANG)) > 0) {}
}

int main(void)
{
	int 	 			sockfd;
	int 				client_sockfd;
	int 		 		opt;
	struct sockaddr_in 	host_addr;
	struct sockaddr_in 	client_addr;
	socklen_t			sin_size;
	pid_t				pid;

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("[ERROR] socket()\n");
		exit(1);
	}

	opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
	{
		printf("[ERROR] setsockopt()\n");
		exit(1);
	}

	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(PORT);
	host_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(host_addr.sin_zero), '\0', 8);

	if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
	{
		printf("[ERROR] bind()\n");
		exit(1);
	}

	if (listen(sockfd, 100) == -1)
	{
		printf("[ERROR] listen()\n");
		exit(1);
	}

	signal(SIGCHLD, proc_exit);
	
	printf("Listening at %s:%d\n\n", inet_ntoa(host_addr.sin_addr), PORT);
	
	while (1)
	{
		sin_size = sizeof(struct sockaddr_in);
		client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
		pid = fork();
		if (pid == 0)
		{
			if (client_sockfd == -1)
			{
				printf("[ERROR] accept()\n");
				printf("%s\n", strerror(errno));
				break;
			}
			handle_requests(client_sockfd, &client_addr);
			shutdown(client_sockfd, SHUT_RDWR);
			close(client_sockfd);
			break;
		}
		else close(client_sockfd);
	}
	return (0);
}