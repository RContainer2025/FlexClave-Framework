#ifndef __MINIOS_API_H
#define __MINIOS_API_H


// 本文件仅用于导出系统OS所需的miniOS函数
void minios_api_call(unsigned int op, ...);
void trigger_syscall(struct nsproxy *ns);
void trigger_syscall_ret(struct nsproxy *ns);

#endif