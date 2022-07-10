#define _GNU_SOURCE

#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <unistd.h>

noreturn void
errExit(const char *err)
{
	perror(err);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int flags = CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWIPC | CLONE_NEWNET;

	if (unshare(flags) == -1)
		errExit("unshare");
	
	pid_t cpid = fork();
	if (cpid == -1)
		errExit("fork");

	char *root_path = realpath("./alpine-minirootfs", NULL);
	if (!root_path)
		errExit("realpath");

 	// child
	if (cpid == 0) {
		char *args[] = { "/bin/sh", NULL };
		char *hostname = "container";
		
		if (sethostname(hostname, strlen(hostname)) == -1)
			errExit("sethostname");

		if (chroot(root_path) == -1)
			errExit("chroot");

		if (chdir("/") == -1)
			errExit("chdir");

		if (setenv("PATH", "/bin:/usr/bin:/sbin:/usr/sbin", 1) == -1)
			errExit("setenv");

		if (setenv("PS1", "\\u@\\h$ ", 1) == -1)
			errExit("setenv");

		if (mount("proc", "/proc", "proc", 0, "") == -1)
			errExit("mount");

		execv(args[0], args);
		errExit("execv");
	}
	
	// parent
	pid_t wpid = waitpid(cpid, NULL, 0);
	if (wpid == -1)
		errExit("waitpid");

	char *proc_path = malloc(strlen(root_path) + strlen("/proc") + 1);
	if (!proc_path)
		errExit("malloc");
	if (sprintf(proc_path, "%s/proc", root_path) < 0)
		errExit("sprintf");

	if (umount(proc_path) == -1)
		errExit("umount");

	free(proc_path);
	free(root_path);

	exit(EXIT_SUCCESS);
}
