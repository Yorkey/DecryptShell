1.拷贝交叉编译中的objdump 
2.objdump -D hotspot.sbin > hotspot.objfile
3.objdump -s genpass.sbin > hotspot.string
4.在hotspot.objfile中找到arc4，key 函数，并分析出msg1等变量的地址及长度
5.将这些信息填充到analysis_shell.c对应结构体中
6.编译analysis_shell.c并运行analysis_shell
7.编译unshc.c并运行unshc > hotspot.sh 得到hotspot.sh源代码
