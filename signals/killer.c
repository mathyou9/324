#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
	char *scenario = argv[1];
	int pid = atoi(argv[2]);

	switch (scenario[0]) {
	case '1':
		kill(-pid, 12);
		break;
	case '2':
		kill(pid, SIGHUP);
		sleep(5);
		kill(-pid, 12);
		break;
	case '3':
		kill(pid, SIGHUP);
		sleep(5);
		kill(pid, SIGHUP);
		sleep(5);
		kill(-pid, 12);
		break;
	case '4':
		kill(pid, SIGHUP);
		sleep(2);
		kill(pid, SIGINT);
		sleep(5);
		kill(-pid, 12);
		break;
	case '5':
		kill(pid, SIGHUP);
		sleep(1);
		kill(pid, 10);
		sleep(5);
		kill(pid, 16);
		sleep(5);
		kill(pid, 31);
		kill(-pid, 12);
		break;
	case '6':
		kill(pid, 31);
		sleep(1);
		kill(pid, 10);
		sleep(3);
		kill(pid, 30);
		sleep(1);
		kill(pid, SIGHUP);
		sleep(5);
		kill(pid, 30);
		sleep(1);
		kill(pid, SIGTERM);
		kill(-pid, 12);
		break;
	case '7':
		kill(pid, 31);
		sleep(5);
		kill(pid, SIGQUIT);
		kill(pid, SIGHUP);
		sleep(9);
		kill(-pid, 12);
		break;

	}
}
/*
* SIGHUP = Handler 1
* SIGINT = Handler 1
* SIGQUIT = Handler 2
* SIGTERM = Handler 3 - Prints value of foo
* 30 = Handler 4
* 10 = Handler 5 - forks and exits 7 in child
* 16 = Handler 6
* 31 = Handler 7
* 12 = Handler 8
* SIGCHLD = Handler 9
*/