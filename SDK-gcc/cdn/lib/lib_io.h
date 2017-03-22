#ifndef __LIB_IO_H__
#define __LIB_IO_H__

#include <vector>
using namespace std;
typedef vector<int> vi;

extern vector<vi> read_file(const char * const filename);

//将result缓冲区中的内容写入文件，写入方式为覆盖写入
extern void write_result(const char * const buff,const char * const filename);

#endif

