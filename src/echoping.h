#pragma once

/* $Id$ */

#include <sys/time.h>
#include <netdb.h>
#include "bool.h"

/* Settings you should not change -- see below for changeable ones */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Settings you can change */

#define DEFLINE 256
#define UDPMAX 65535
/* Mostly for HTTP */
#ifdef SMTP
#define MAXSMTP 1024
#define MAXSMTPLINES 30
#endif

/* Probably too many inclusions but this is to keep 'gcc -Wall' happy... */

#include <popt.h>

#ifdef OPENSSL
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#endif /* OpenSSL */

#ifdef GNUTLS
#include <gnutls/gnutls.h>
#endif

#ifdef LIBIDN
#include <stringprep.h>		/* stringprep_locale_to_utf8() */
#include <idna.h>		/* idna_to_ascii_from_utf8() */
#endif

#ifndef SIGALRM			/* Linux... */
#define SIGALRM   14		/* alarm clock timeout */
#endif
#ifndef SIGINT			/* Linux... */
#define SIGINT   2		/* interrupt, generated from terminal special char */
#endif

#ifndef INADDR_NONE		/* SunOS */
#define       INADDR_NONE (-1)
#endif

#ifndef SOL_IP
#define SOL_IP (getprotobyname("ip")->p_proto)
#endif

/* These entities should be in errno.h but some systems do not define
   them. */
#ifdef DECL_SYS_NERR
extern int sys_nerr;
#endif

/* If we have it, use it */
#ifdef HAVE_SIGACTION
#define USE_SIGACTION 1
#endif
#ifdef HAVE_TOS
#define USE_TOS 1
#endif
#ifdef HAVE_SOCKET_PRIORITY
#define USE_PRIORITY 1
#endif



struct echoping_struct
{
  boolean udp;			/* Use the UDP protocol (TCP is the default) */
  boolean ttcp;
  boolean only_ipv4;
  boolean only_ipv6;
  boolean verbose;
};


char *server;
boolean timeout_flag = TRUE;

typedef struct echoping_struct echoping_options;
#ifndef IN_PLUGIN
/* The functions we will find in the plugin */
/* Initializes the plugin with its arguments. Returns the port name or number or NULL if the plugin wants to use the raw interface. */
typedef char *(*init_f) (const int argc, const char **argv,
			 const echoping_options global_options);
typedef void (*start_f) (struct addrinfo *);
typedef void (*start_raw_f) ();
typedef int (*execute_f) ();
typedef void (*terminate_f) ();
#endif

#define	ECHO_TCP_PORT	"echo"
#define	DISCARD_TCP_PORT	"discard"
#define	CHARACTER_GENERATOR_TCP_PORT	"chargen"
#define DEFAULT_HTTP_TCP_PORT "http"
#define DEFAULT_HTTPS_TCP_PORT "https"
#define DEFAULT_ICP_UDP_PORT "icp"

#ifdef HTTP
/* Use the old HTTP 1.0 protocol? If yes, set HTTP10 to 1*/
#undef HTTP10
#endif

#define	USE_ECHO	1
#define	USE_DISCARD	2
#define	USE_CHARGEN	3
#define	USE_HTTP	4
#define USE_ICP		5
#define USE_SMTP        6

#define CHARGENERATED " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefg";

#ifdef LIBIDN
char *locale_server, *ace_server, *utf8_server;
#endif

/* My functions */

/* error.c */
#ifdef ICP
#include "icp.h"
void *make_icp_sendline ();
int recv_icp ();
#ifndef HTTP
int read_from_server ();
#endif

#endif



