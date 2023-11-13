#pragma once

#include <sys/time.h>
#include "bool.h"

struct result
{
    boolean valid;
    struct timeval timevalue;
};

char *random_string ();
void tvsub (struct timeval *out, struct timeval *in);
void tvadd (struct timeval *out, struct timeval *in);
void tvavg (struct timeval *out, int number);
void tvmin (struct timeval *champion, struct timeval *challenger);
void tvmax (struct timeval *champion, struct timeval *challenger);
int tvcmp (struct timeval *left, struct timeval *right);
void tvstddev (struct timeval *out, int number, struct timeval average, struct result* results);
void tvstddevavg (struct timeval *out, int number, struct timeval average, struct result* results, double n_stddev);
double tv2double (struct timeval tv);
struct timeval double2tv (double x);