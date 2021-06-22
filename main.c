#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sl/arg.h>

enum{
	StdBufSiz = 512,
};

char *argv0;
char **cmd;
char **argbufs;

int curnarg;
int narg = 1 ;
int ncmd; 
int bufsiz = StdBufSiz ;

void
usage(void)
{
	fprintf(stderr, "usage: %s [-n narg] <cmd>\n", argv0);
	exit(1);
}

void
strchp(char *s, char c)
{
	int i = strlen(s) - 1 ;
	if(s[i] == c)
		s[i] = 0 ;
}

void
exe(void)
{
	int i, arglen = ncmd+curnarg ;
	char **arg = malloc(sizeof(char*)*(arglen+1)) ;	

	for(i=0 ; i<ncmd ; ++i)
		arg[i] = cmd[i] ;
	for(; i<arglen ; ++i)
		arg[i] = argbufs[i-curnarg] ;
	arg[i] = 0 ;

	execvp(arg[0], arg);

	free(arg);
}

int
main(int argc, char *argv[])
{
	int i, pid;
	int status;
	argv0 = argv[0] ;
	ARGBEGIN {
	case 'n' :
		narg = atoi(EARGF(usage())) ;
	break;
	default:
		usage();
	} ARGEND ;
	if(!argc)
		usage();

	cmd =  argv ;
	ncmd = argc ;
	argbufs = malloc(sizeof(*argbufs)*narg) ;
	for(i=0 ; i<narg ; ++i)
		argbufs[i] = malloc(bufsiz) ;

	while(1){
		for(i=0 ; i<narg ; ++i){
			if(!fgets(argbufs[i], bufsiz, stdin))
				break ;
			curnarg = i+1 ;
			strchp(argbufs[i], '\n');
		}
		pid = fork() ;
		if(!pid){
			exe();
		} else {
			waitpid(pid, &status, 0);
		}
	}

thread_success:
	return 0 ;
}
