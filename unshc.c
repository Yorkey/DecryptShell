/* rtc.c */

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "unshc.h"

#define      hide_z     4096

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

void rmarg(char ** argv, char * arg)
{
        for (; argv && *argv && *argv != arg; argv++);
        for (; argv && *argv; argv++)
                *argv = argv[1];
}

int chkenv(int argc)
{
        char buff[512];
        unsigned mask, m;
        int l, a, c;
        char * string;
        extern char ** environ;

        mask  = (unsigned long)&chkenv;
        mask ^= (unsigned)getpid() * ~mask;
        sprintf(buff, "x%x", mask);
        string = getenv(buff);
        l = strlen(buff);
        if (!string) {
                /* 1st */
                sprintf(&buff[l], "=%u %d", mask, argc);
                putenv(strdup(buff));
                return 0;
        }
        c = sscanf(string, "%u %d%c", &m, &a, buff);
        if (c == 2 && m == mask) {
                /* 3rd */
                rmarg(environ, &string[-l - 1]);
                return 1 + (argc - a);
        }
        return -1;
}

char * xsh(int argc, char ** argv)
{
        char * scrpt;
        int ret, i, j;
        char ** varg;

        stte_0();
         key(pswd, pswd_z);
        arc4(msg1, msg1_z);
        arc4(date, date_z);
        if (date[0] && date[0]<time(NULL))
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
                printf("%s",text);
                return 0;
                arc4(tst2, tst2_z);
                 key(tst2, tst2_z);
                arc4(chk2, chk2_z);
                if ((chk2_z != tst2_z) || memcmp(tst2, chk2, tst2_z))
                        return tst2;
                if (text_z < hide_z) {
                        /* Prepend spaces til a hide_z script size. */
                        scrpt = malloc(hide_z);
                        if (!scrpt)
                                return 0;
                        memset(scrpt, (int) ' ', hide_z);
                        memcpy(&scrpt[hide_z - text_z], text, text_z);
                } else {
                        scrpt = text;   /* Script text */
                }
        } else {                        /* Reexecute */
                if (*xecc) {
                        scrpt = malloc(512);
                        if (!scrpt)
                                return 0;
                        sprintf(scrpt, xecc, argv[0]);
                } else {
                        scrpt = argv[0];
                }
        }
        j = 0;
        varg[j++] = argv[0];            /* My own name at execution */
        if (ret && *opts)
                varg[j++] = opts;       /* Options on 1st line of code */
        if (*inlo)
                varg[j++] = inlo;       /* Option introducing inline code */
        varg[j++] = scrpt;              /* The script itself */
        if (*lsto)
                varg[j++] = lsto;       /* Option meaning last option */
        i = (ret > 1) ? ret : 0;        /* Args numbering correction */
        while (i < argc)
                varg[j++] = argv[i++];  /* Main run-time arguments */
        varg[j] = 0;                    /* NULL terminated array */
        execvp(shll, varg);
        return shll;
}

int main(int argc, char ** argv)
{
        argv[1] = xsh(argc, argv);
        return 1;
}