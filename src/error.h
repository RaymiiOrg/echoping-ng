#pragma once

#include "popt.h"
#include "bool.h"

void usage (poptContext context);
void err_sys (char *str, ...);
void err_ret (char *str, ...);
void err_quit (char *str, ...);
char *sys_err_str ();