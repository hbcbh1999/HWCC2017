#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/timeb.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "lib_io.h"

#define MAX_LINE_LEN 55000

#define INLINE  static __inline
#ifdef _DEBUG
#define PRINT   printf
#else
#define PRINT(...)
#endif


INLINE void write_file(const bool cover, const char * const buff, const char * const filename);


void print_time(const char *head)
{
#ifdef _DEBUG
    struct timeb rawtime;
    struct tm * timeinfo;
    ftime(&rawtime);
    timeinfo = localtime(&rawtime.time);

    static int ms = rawtime.millitm;
    static unsigned long s = rawtime.time;
    int out_ms = rawtime.millitm - ms;
    unsigned long out_s = rawtime.time - s;
    ms = rawtime.millitm;
    s = rawtime.time;

    if (out_ms < 0)
    {
        out_ms += 1000;
        out_s -= 1;
    }
    printf("%s date/time is: %s \tused time is %lu s %d ms.\n", head, asctime(timeinfo), out_s, out_ms);
#endif
}

bool read(FILE *fp, int &x)
{
    // read positive integer
    char c = getc(fp);
    while (!feof(fp) && c != '\n' && c != '\r' && (c < '0' || c > '9'))
        c = getc(fp);
    if (feof(fp) || c == '\n' || c == '\r')
        return 0;
    for (x = 0; c >= '0' && c <= '9'; c = getc(fp))
        x = x * 10 + c - '0';
    if (c == '\n' || c == '\r')
        return 0;
    return 1;
}

vector<vi> read_file(const char * const filename)
{
    vector<vi> ans;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        PRINT("Fail to open %s, %s.\n", filename, strerror(errno));
        return ans;
    }
    PRINT("Open %s.\n", filename);

    while (!feof(fp))
    {
        vi a;
        int x = -1;
        while (read(fp, x) && !feof(fp))
            a.push_back(x);
        if (x >= 0)
            a.push_back(x);
        if (a.size() > 0)
            ans.push_back(a);
    }
    fclose(fp);
    PRINT("%d lines in %s.\n", (int)ans.size(), filename);
    return ans;
}

void write_result(const char * const buff,const char * const filename)
{
	// 以覆盖的方式写入
    write_file(1, buff, filename);

}

void release_buff(char ** const buff, const int valid_item_num)
{
    for (int i = 0; i < valid_item_num; i++)
        free(buff[i]);
}

INLINE void write_file(const bool cover, const char * const buff, const char * const filename)
{
    if (buff == NULL)
        return;

    const char *write_type = cover ? "w" : "a";//1:覆盖写文件，0:追加写文件
    FILE *fp = fopen(filename, write_type);
    if (fp == NULL)
    {
        PRINT("Fail to open file %s, %s.\n", filename, strerror(errno));
        return;
    }
    PRINT("Open file %s OK.\n", filename);
    fputs(buff, fp);
    fclose(fp);
}

