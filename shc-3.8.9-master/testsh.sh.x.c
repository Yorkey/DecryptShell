#if 0
	shc Version 3.8.9, Generic Script Compiler
	Copyright (c) 1994-2012 Francisco Rosales <frosal@fi.upm.es>

	./shc -r -v -f testsh.sh 
#endif

static  char data [] = 
#define      tst1_z	22
#define      tst1	((&data[5]))
	"\352\074\277\172\125\215\252\160\377\330\100\301\071\262\154\212"
	"\340\056\273\262\340\217\245\006\013\007\255\145\112\377"
#define      pswd_z	256
#define      pswd	((&data[57]))
	"\072\356\100\042\237\273\056\250\244\310\364\056\230\172\031\343"
	"\231\227\317\061\202\014\360\374\141\126\107\207\137\343\214\170"
	"\112\000\353\157\123\100\106\346\227\323\016\335\343\340\335\367"
	"\054\304\216\107\145\350\023\214\133\306\024\273\252\241\063\364"
	"\241\036\144\364\137\252\332\366\176\351\324\141\311\261\130\366"
	"\166\346\076\333\317\122\150\053\030\174\346\302\035\031\267\276"
	"\070\033\263\230\305\216\217\104\167\144\245\101\026\376\067\214"
	"\345\166\150\264\310\320\340\340\115\307\243\152\340\132\051\031"
	"\166\334\262\074\152\101\200\342\245\045\043\274\044\133\110\011"
	"\321\261\276\232\202\237\172\317\146\035\071\107\170\143\140\356"
	"\100\023\052\253\124\252\215\372\320\261\266\364\015\377\376\337"
	"\261\275\171\063\134\363\002\302\021\074\011\211\237\152\167\337"
	"\175\242\212\322\114\027\315\034\311\204\021\326\204\017\265\065"
	"\314\056\150\051\042\152\354\063\246\365\275\106\140\065\045\335"
	"\327\257\260\043\307\175\100\221\001\121\147\205\141\035\273\056"
	"\113\043\127\156\216\103\241\065\071\137\173\231\224\240\167\153"
	"\120\050\217\030\245\320\252\247\041\021\055\202\056\350\260\172"
	"\013\010\350\232\114\212\317\205\352\112\037\141\143\262\162"
#define      msg2_z	19
#define      msg2	((&data[320]))
	"\300\101\156\061\315\234\315\141\345\065\203\312\221\006\113\277"
	"\026\301\204\310\362\061\356\351\022"
#define      rlax_z	1
#define      rlax	((&data[342]))
	"\331"
#define      inlo_z	3
#define      inlo	((&data[343]))
	"\304\040\155"
#define      date_z	1
#define      date	((&data[346]))
	"\126"
#define      msg1_z	42
#define      msg1	((&data[348]))
	"\016\253\352\246\312\344\313\275\333\076\270\275\155\062\116\013"
	"\340\007\173\225\250\370\163\215\111\237\312\126\023\366\235\203"
	"\234\141\337\165\333\235\036\331\350\046\216\023\161\160\151\270"
#define      text_z	29
#define      text	((&data[401]))
	"\104\152\015\367\052\116\075\104\054\032\153\137\320\155\230\353"
	"\213\006\174\060\317\163\061\240\074\302\312\262\117\000\364\135"
	"\162\147\216\146\031\070"
#define      chk1_z	22
#define      chk1	((&data[437]))
	"\272\040\156\025\303\264\356\222\243\376\203\073\216\053\006\025"
	"\113\021\021\221\210\210\136\251\226\153"
#define      xecc_z	15
#define      xecc	((&data[460]))
	"\125\135\001\013\240\250\351\222\267\236\372\230\141\021\110\350"
#define      opts_z	1
#define      opts	((&data[475]))
	"\127"
#define      tst2_z	19
#define      tst2	((&data[480]))
	"\000\246\245\315\125\377\066\061\036\174\346\325\367\327\071\123"
	"\205\257\371\233\303\056\337\022\352\070\037"
#define      chk2_z	19
#define      chk2	((&data[504]))
	"\110\212\003\221\365\270\145\114\301\221\344\275\247\117\206\047"
	"\142\002\155\211\173\246"
#define      shll_z	8
#define      shll	((&data[525]))
	"\056\334\025\356\244\033\375\364\173\154"
#define      lsto_z	1
#define      lsto	((&data[535]))
	"\123"/* End of data[] */;
#define      hide_z	4096
#define DEBUGEXEC	0	/* Define as 1 to debug execvp calls */
#define TRACEABLE	0	/* Define as 1 to enable ptrace the executable */

/* rtc.c */

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* 'Alleged RC4' */

static unsigned char stte[256], indx, jndx, kndx;

/*
 * Reset arc4 stte. 
 */
void stte_0(void)
{
	indx = jndx = kndx = 0;
	do {
		stte[indx] = indx;
	} while (++indx);
}

/*
 * Set key. Can be used more than once. 
 */
void key(void * str, int len)
{
	unsigned char tmp, * ptr = (unsigned char *)str;
	while (len > 0) {
		do {
			tmp = stte[indx];
			kndx += tmp;
			kndx += ptr[(int)indx % len];
			stte[indx] = stte[kndx];
			stte[kndx] = tmp;
		} while (++indx);
		ptr += 256;
		len -= 256;
	}
}

/*
 * Crypt data. 
 */
void arc4(void * str, int len)
{
	unsigned char tmp, * ptr = (unsigned char *)str;
	while (len > 0) {
		indx++;
		tmp = stte[indx];
		jndx += tmp;
		stte[indx] = stte[jndx];
		stte[jndx] = tmp;
		tmp += stte[indx];
		*ptr ^= stte[tmp];
		ptr++;
		len--;
	}
}

/* End of ARC4 */

/*
 * Key with file invariants. 
 */
int key_with_file(char * file)
{
	struct stat statf[1];
	struct stat control[1];

	if (stat(file, statf) < 0)
		return -1;

	/* Turn on stable fields */
	memset(control, 0, sizeof(control));
	control->st_ino = statf->st_ino;
	control->st_dev = statf->st_dev;
	control->st_rdev = statf->st_rdev;
	control->st_uid = statf->st_uid;
	control->st_gid = statf->st_gid;
	control->st_size = statf->st_size;
	control->st_mtime = statf->st_mtime;
	control->st_ctime = statf->st_ctime;
	key(control, sizeof(control));
	return 0;
}

#if DEBUGEXEC
void debugexec(char * sh11, int argc, char ** argv)
{
	int i;
	fprintf(stderr, "shll=%s\n", sh11 ? sh11 : "<null>");
	fprintf(stderr, "argc=%d\n", argc);
	if (!argv) {
		fprintf(stderr, "argv=<null>\n");
	} else { 
		for (i = 0; i <= argc ; i++)
			fprintf(stderr, "argv[%d]=%.60s\n", i, argv[i] ? argv[i] : "<null>");
	}
}
#endif /* DEBUGEXEC */

void rmarg(char ** argv, char * arg)
{
	for (; argv && *argv && *argv != arg; argv++);
	for (; argv && *argv; argv++)
		*argv = argv[1];
}

int chkenv(int argc)
{
	char buff[512];
	unsigned long mask, m;
	int l, a, c;
	char * string;
	extern char ** environ;

	mask  = (unsigned long)&chkenv;
	mask ^= (unsigned long)getpid() * ~mask;
	sprintf(buff, "x%lx", mask);
	string = getenv(buff);
#if DEBUGEXEC
	fprintf(stderr, "getenv(%s)=%s\n", buff, string ? string : "<null>");
#endif
	l = strlen(buff);
	if (!string) {
		/* 1st */
		sprintf(&buff[l], "=%lu %d", mask, argc);
		putenv(strdup(buff));
		return 0;
	}
	c = sscanf(string, "%lu %d%c", &m, &a, buff);
	if (c == 2 && m == mask) {
		/* 3rd */
		rmarg(environ, &string[-l - 1]);
		return 1 + (argc - a);
	}
	return -1;
}

#if !defined(TRACEABLE)

#define _LINUX_SOURCE_COMPAT
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#if !defined(PTRACE_ATTACH) && defined(PT_ATTACH)
#	define PTRACE_ATTACH	PT_ATTACH
#endif
void untraceable(char * argv0)
{
	char proc[80];
	int pid, mine;

	switch(pid = fork()) {
	case  0:
		pid = getppid();
		/* For problematic SunOS ptrace */
#if defined(__FreeBSD__)
		sprintf(proc, "/proc/%d/mem", (int)pid);
#else
		sprintf(proc, "/proc/%d/as",  (int)pid);
#endif
		close(0);
		mine = !open(proc, O_RDWR|O_EXCL);
		if (!mine && errno != EBUSY)
			mine = !ptrace(PTRACE_ATTACH, pid, 0, 0);
		if (mine) {
			kill(pid, SIGCONT);
		} else {
			perror(argv0);
			kill(pid, SIGKILL);
		}
		_exit(mine);
	case -1:
		break;
	default:
		if (pid == waitpid(pid, 0, 0))
			return;
	}
	perror(argv0);
	_exit(1);
}
#endif /* !TRACEABLE */

char * xsh(int argc, char ** argv)
{
	char * scrpt;
	int ret, i, j;
	char ** varg;
	char * me = getenv("_");
	if (me == NULL) { me = argv[0]; }

	stte_0();
	 key(pswd, pswd_z);
	arc4(msg1, msg1_z);
	arc4(date, date_z);
	if (date[0] && (atoll(date)<time(NULL)))
		return msg1;
	arc4(shll, shll_z);
	arc4(inlo, inlo_z);
	arc4(xecc, xecc_z);
	arc4(lsto, lsto_z);
	arc4(tst1, tst1_z);
	 key(tst1, tst1_z);
	arc4(chk1, chk1_z);
	if ((chk1_z != tst1_z) || memcmp(tst1, chk1, tst1_z))
		return tst1;
	ret = chkenv(argc);
	arc4(msg2, msg2_z);
	if (ret < 0)
		return msg2;
	varg = (char **)calloc(argc + 10, sizeof(char *));
	if (!varg)
		return 0;
	if (ret) {
		arc4(rlax, rlax_z);
		if (!rlax[0] && key_with_file(shll))
			return shll;
		arc4(opts, opts_z);
		arc4(text, text_z);
		arc4(tst2, tst2_z);
		 key(tst2, tst2_z);
		arc4(chk2, chk2_z);
		if ((chk2_z != tst2_z) || memcmp(tst2, chk2, tst2_z))
			return tst2;
		/* Prepend hide_z spaces to script text to hide it. */
		scrpt = malloc(hide_z + text_z);
		if (!scrpt)
			return 0;
		memset(scrpt, (int) ' ', hide_z);
		memcpy(&scrpt[hide_z], text, text_z);
	} else {			/* Reexecute */
		if (*xecc) {
			scrpt = malloc(512);
			if (!scrpt)
				return 0;
			sprintf(scrpt, xecc, me);
		} else {
			scrpt = me;
		}
	}
	j = 0;
	varg[j++] = argv[0];		/* My own name at execution */
	if (ret && *opts)
		varg[j++] = opts;	/* Options on 1st line of code */
	if (*inlo)
		varg[j++] = inlo;	/* Option introducing inline code */
	varg[j++] = scrpt;		/* The script itself */
	if (*lsto)
		varg[j++] = lsto;	/* Option meaning last option */
	i = (ret > 1) ? ret : 0;	/* Args numbering correction */
	while (i < argc)
		varg[j++] = argv[i++];	/* Main run-time arguments */
	varg[j] = 0;			/* NULL terminated array */
#if DEBUGEXEC
	debugexec(shll, j, varg);
#endif
	execvp(shll, varg);
	return shll;
}

int main(int argc, char ** argv)
{
#if DEBUGEXEC
	debugexec("main", argc, argv);
#endif
#if !defined(TRACEABLE)
	untraceable(argv[0]);
#endif
	argv[1] = xsh(argc, argv);
	fprintf(stderr, "%s%s%s: %s\n", argv[0],
		errno ? ": " : "",
		errno ? strerror(errno) : "",
		argv[1] ? argv[1] : "<null>"
	);
	return 1;
}
