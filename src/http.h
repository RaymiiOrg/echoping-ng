#pragma once

#ifdef HTTP

#include <stdio.h>
typedef unsigned int boolean;

typedef union _CHANNEL
{
    FILE *fs;
#ifdef OPENSSL
    SSL *ssl;
#endif
#ifdef GNUTLS
    gnutls_session tls;
#endif
}
CHANNEL;


char *make_http_sendline (char *url, char *host, int port, int nocache);
void find_server_and_port ();
/* This one has prototypes, for a very subtile compiler issue. */
int read_from_server (CHANNEL fs, short ssl, boolean accept_redirects);
#endif