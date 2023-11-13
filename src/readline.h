#pragma once
#include <stdio.h>
#include "bool.h"

int readline (FILE *fs,char *ptr, int maxlen,unsigned short  ln);
#ifdef OPENSSL
int SSL_readline ();
#endif
#ifdef GNUTLS
int TLS_readline ();
#endif
