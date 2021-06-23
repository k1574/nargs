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
int wflag = 0, uflag = 0;
int memalloced;

char **bufs;
int narg = 1, realnarg;
int ncmd; 
int bufsiz = StdBufSiz ;

void
usage(void)
{
	fprintf(stderr, "usage: %s [-n narg] [-u] [-w] <cmd>\n", argv0);
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
	int i, arglen = ncmd+realnarg ;
	char **arg = malloc(sizeof(char*)*(arglen+1)) ;	

	for(i=0 ; i<ncmd ; ++i)
		arg[i] = cmd[i] ;
	for(; i<arglen ; ++i)
		arg[i] = bufs[i-ncmd] ;
	arg[i] = 0 ;

	/*for(i = 0 ; i<arglen ; ++i)
		printf("%x = '%s' %d\n", arg[i], arg[i], realnarg);*/
	execvp(arg[0], arg);

	/* Not reachable? */
	free(arg);
}

int
main(int argc, char *argv[])
{
	int i, pid, run, status;
	
	argv0 = argv[0] ;
	ARGBEGIN {
	case 'n' :
		narg = atoi(EARGF(usage())) ;
	break;
	case 'w' :
		wflag = 1 ;
	case 'u' :
		uflag = 1 ;
	break;
	default:
		usage();
	} ARGEND ;
	if(!argc)
		usage();

	cmd =  argv ;
	ncmd = argc ;

	bufs = malloc(sizeof(char *)*narg) ;
	for(i=0 ; i<narg ; ++i)
		bufs[i] = malloc(bufsiz) ;

	run = 1 ;
	while(run){
		for(i=0 ; i<narg ; ++i){
			if(!fgets(bufs[i], bufsiz, stdin)){
				run = 0 ;
				break ;
			}
			strchp(bufs[i], '\n');
		}
		realnarg = run ? narg : i ;
		if(!(pid = fork()))
			exe();
		else if(!wflag)
				waitpid(pid, &status, 0);
	}

	
	if(wflag) while(wait(&status) > 0) ;

	return 0 ;
}
