/******************************************************************************
**
** Main IP Discovery file...
**
** Contains the program entry point
**
******************************************************************************/
#include <iostream>
#include <cstring>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>

extern "C" {
#include <strutils.h>
}

#include "ip_error.h"
#include "currenttime.h"
#include "logger.h"
#include "configmgr.h"
#include "posixthread.h"
#include "threads.h"

extern "C" {
	#include "version.h"
}

using namespace std;

void printUsage(char * pszAppName)
{
	printf("\n Usage: %s [OPTIONS]\n\n", pszAppName);
	printf("  Options:\n");
	printf("   -h/?             Print this help\n");
	printf("   -version         Print the program version\n");
	printf("   -port device     Serial port device\n");
	printf("   -baud baudrate   Serial port baud rate\n");
	printf("   -cfg configfile  Specify the cfg file, default is ./webconfig.cfg\n");
	printf("   -d               Daemonise this application\n");
	printf("   -log  filename   Write logs to the file\n");
	printf("\n");
}

void daemonise()
{
	pid_t			pid;
	pid_t			sid;

	fprintf(stdout, "Starting daemon...\n");
	fflush(stdout);

	do {
		pid = fork();
	}
	while ((pid == -1) && (errno == EAGAIN));

	if (pid < 0) {
		fprintf(stderr, "Forking daemon failed...\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}
	if (pid > 0) {
		fprintf(stdout, "Exiting child process...\n");
		fflush(stdout);
		exit(EXIT_SUCCESS);
	}

	sid = setsid();
	
	if(sid < 0) {
		fprintf(stderr, "Failed calling setsid()...\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);    
	
	umask(0);

	if((chdir("/") == -1)) {
		fprintf(stderr, "Failed changing directory\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}
	
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
}

int main(int argc, char *argv[])
{
	FILE *			fptr_pid;
	char *			pszAppName;
	char *			pszLogFileName = NULL;
	char *			pszConfigFileName = NULL;
	char			szPidFileName[PATH_MAX];
	int				i;
	bool			isDaemonised = false;
	bool			isDumpConfig = false;
	char			cwd[PATH_MAX];
	int				defaultLoggingLevel = LOG_LEVEL_INFO | LOG_LEVEL_ERROR | LOG_LEVEL_FATAL;

	pszAppName = strdup(argv[0]);

	getcwd(cwd, sizeof(cwd));
	
	strcpy(szPidFileName, cwd);
	strcat(szPidFileName, "/ipaddr.pid");

	printf("\nRunning %s from %s\n", pszAppName, cwd);

	if (argc > 1) {
		for (i = 1;i < argc;i++) {
			if (argv[i][0] == '-') {
				if (argv[i][1] == 'd') {
					isDaemonised = true;
				}
				else if (strcmp(&argv[i][1], "log") == 0) {
					pszLogFileName = strdup(&argv[++i][0]);
				}
				else if (strcmp(&argv[i][1], "cfg") == 0) {
					pszConfigFileName = strdup(&argv[++i][0]);
				}
				else if (strcmp(&argv[i][1], "list-config") == 0) {
					isDumpConfig = true;
				}
				else if (argv[i][1] == 'h' || argv[i][1] == '?') {
					printUsage(pszAppName);
					return 0;
				}
				else if (strcmp(&argv[i][1], "version") == 0) {
					printf("%s Version: [%s], Build date: [%s]\n\n", pszAppName, getVersion(), getBuildDate());
					return 0;
				}
				else {
					printf("Unknown argument '%s'", &argv[i][0]);
					printUsage(pszAppName);
					return 0;
				}
			}
		}
	}
	else {
		printUsage(pszAppName);
		return -1;
	}

	if (isDaemonised) {
		daemonise();
	}

	fptr_pid = fopen(szPidFileName, "wt");
	
	if (fptr_pid == NULL) {
		fprintf(stderr, "Failed top open PID file\n");
		fflush(stderr);
	}
	else {
		fprintf(fptr_pid, "%d\n", getpid());
		fclose(fptr_pid);
	}
	
	openlog(pszAppName, LOG_PID|LOG_CONS, LOG_DAEMON);
	syslog(LOG_INFO, "Started %s", pszAppName);

	if (pszConfigFileName == NULL) {
		pszConfigFileName = strdup("ipaddr.cfg");
	}

	ConfigManager & cfg = ConfigManager::getInstance();
	
	try {
		cfg.initialise(pszConfigFileName);
	}
	catch (ip_error & e) {
		fprintf(stderr, "Could not read config file: %s [%s]\n", pszConfigFileName, e.what());
		fprintf(stderr, "Aborting!\n\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	if (pszConfigFileName != NULL) {
		free(pszConfigFileName);
	}

	if (isDumpConfig) {
		cfg.dumpConfig();
	}

	Logger & log = Logger::getInstance();

	if (pszLogFileName != NULL) {
		log.initLogger(pszLogFileName, defaultLoggingLevel);
		free(pszLogFileName);
	}
	else {
		const char * filename = cfg.getValue("log.filename");
		const char * level = cfg.getValue("log.level");

		if (strlen(filename) == 0 && strlen(level) == 0) {
			log.initLogger(defaultLoggingLevel);
		}
		else if (strlen(level) == 0) {
			log.initLogger(filename, defaultLoggingLevel);
		}
		else {
			log.initLogger(filename, level);
		}
	}

	/*
	 * Start threads...
	 */
	ThreadManager & threadMgr = ThreadManager::getInstance();

	threadMgr.startThreads();

	while (1) {
		PosixThread::sleep(PosixThread::seconds, 5L);
	}

	return 0;
}
