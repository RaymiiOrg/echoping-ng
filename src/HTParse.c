/*              Parse HyperText Document Address                HTParse.c
   **           ================================
 */

#include "HTParse.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define TRACE 0

#define FREE(x) if (x) {free(x); x = NULL;}

struct struct_parts {
    char           *access;
    char           *host;
    char           *absolute;
    char           *relative;
/*      char * search;          no - treated as part of path */
    char           *anchor;
};

/*      Strings of any length
   **      ---------------------
 */
PUBLIC int      strcasecomp
ARGS2(CONST char *, a, CONST char *, b)
{
    CONST char     *p = a;
    CONST char     *q = b;

    for (p = a, q = b; *p && *q; p++, q++) {
        int             diff = TOLOWER(*p) - TOLOWER(*q);
        if (diff)
            return diff;
    }
    if (*p)
        return 1;               /* p was longer than q */
    if (*q)
        return -1;              /* p was shorter than q */
    return 0;                   /* Exact match */
}

/*      With count limit
   **      ----------------
 */
PUBLIC int      strncasecomp
ARGS3(CONST char *, a, CONST char *, b, int, n)
{
    CONST char     *p = a;
    CONST char     *q = b;

    for (p = a, q = b;; p++, q++) {
        int             diff;
        if (p == (a + n))
            return 0;           /* Match up to n characters */
        if (!(*p && *q))
            return (*p - *q);
        diff = TOLOWER(*p) - TOLOWER(*q);
        if (diff)
            return diff;
    }
    /* NOTREACHED */
}

/*      Allocate a new copy of a string, and returns it
 */
PUBLIC char    *HTSACopy
ARGS2(char **, dest, CONST char *, src)
{
    FREE(*dest);
    if (src) {
        *dest = (char *) malloc(strlen(src) + 1);
        if (*dest == NULL)
            outofmem(__FILE__, "HTSACopy");
        strcpy(*dest, src);
    }
    return *dest;
}
/*      String Allocate and Concatenate
 */
PUBLIC char    *HTSACat
ARGS2(char **, dest, CONST char *, src)
{
    if (src && *src) {
        if (*dest) {
            int             length = strlen(*dest);
            *dest = (char *) realloc(*dest, length + strlen(src) + 1);
            if (*dest == NULL)
                outofmem(__FILE__, "HTSACat");
            strcpy(*dest + length, src);
        } else {
            *dest = (char *) malloc(strlen(src) + 1);
            if (*dest == NULL)
                outofmem(__FILE__, "HTSACat");
            strcpy(*dest, src);
        }
    }
    return *dest;
}



/*      Strip white space off a string.                         HTStrip()
   **   -------------------------------
   **
   ** On exit,
   **   Return value points to first non-white character, or to 0 if none.
   **   All trailing white space is OVERWRITTEN with zero.
 */
PUBLIC char    *HTStrip
ARGS1(char *, s)
{
#define SPACE(c) ((c == ' ') || (c == '\t') || (c == '\n'))
    char           *p = s;
    for (p = s; *p; p++);       /* Find end of string */
    for (p--; p >= s; p--) {
        if (SPACE(*p))
            *p = '\0';          /* Zap trailing blanks */
        else
            break;
    }
    while (SPACE(*s))
        s++;                    /* Strip leading blanks */
    return s;
}

/*      Scan a filename for its consituents.                    scan()
   **   ------------------------------------
   **
   ** On entry,
   **   name    points to a document name which may be incomplete.
   ** On exit,
   **      absolute or relative may be nonzero (but not both).
   **   host, anchor and access may be nonzero if they were specified.
   **   Any which are nonzero point to zero terminated strings.
 */
PRIVATE void    scan
ARGS2(char *, name, struct struct_parts *, parts)
{
    char           *after_access;
    char           *p;
    /* int length = strlen (name); */

    parts->access = NULL;
    parts->host = NULL;
    parts->absolute = NULL;
    parts->relative = NULL;
    parts->anchor = NULL;

    /* 
     **  Scan left-to-right for a scheme (access).
     */
    after_access = name;
    for (p = name; *p; p++) {
        if (*p == ':') {
            *p = '\0';
            parts->access = name;       /* Access name has been specified */
            after_access = (p + 1);
            break;
        }
        if (*p == '/' || *p == '#' || *p == ';' || *p == '?')
            break;
    }

#ifdef NOTDEFINED
    for (p = (name + length - 1); p >= name; p--) {
#endif                          /* NOTDEFINED */
        /* 
         **  Scan left-to-right for a fragment (anchor).
         */
        for (p = after_access; *p; p++) {
            if (*p == '#') {
                parts->anchor = (p + 1);
                *p = '\0';      /* terminate the rest */
            }
        }

        /* 
         **  Scan left-to-right for a host or absolute path.
         */
        p = after_access;
        if (*p == '/') {
            if (p[1] == '/') {
                parts->host = (p + 2);  /* host has been specified */
                *p = '\0';      /* Terminate access */
                p = strchr(parts->host, '/');   /* look for end of host name if any */
                if (p != NULL) {
                    *p = '\0';  /* Terminate host */
                    parts->absolute = (p + 1);  /* Root has been found */
                }
            } else {
                parts->absolute = (p + 1);      /* Root found but no host */
            }
        } else {
            parts->relative = (*after_access) ? after_access : NULL;    /* NULL for
                                                                         * "" */
        }

        /* 
         **  Check schemes that commonly have unescaped hashes.
         */
        if (parts->access && parts->anchor) {
            if ((!parts->host && strcasecomp(parts->access, "lynxcgi")) ||
                !strcasecomp(parts->access, "nntp") ||
                !strcasecomp(parts->access, "snews") ||
                !strcasecomp(parts->access, "news") ||
                !strcasecomp(parts->access, "data")) {
                /* 
                 *  Access specified but no host and not a lynxcgi URL, so the
                 *  anchor may not really be one, e.g., news:j462#36487@foo.bar,
                 *  or it's an nntp or snews URL, or news URL with a host.
                 *  Restore the '#' in the address.
                 */
                *(parts->anchor - 1) = '#';
                parts->anchor = NULL;
            }
        }
#ifdef NOT_DEFINED              /* search is just treated as part of path */
        {
            char           *p = (relative ? relative : absolute);
            if (p != NULL) {
                char           *q = strchr(p, '?');     /* Any search string? */
                if (q != NULL) {
                    *q = '\0';  /* If so, chop that off. */
                    parts->search = (q + 1);
                }
            }
        }
#endif                          /* NOT_DEFINED */
    }                           /* scan */


/*      Parse a Name relative to another name.                  HTParse()
   **   --------------------------------------
   **
   **   This returns those parts of a name which are given (and requested)
   **   substituting bits from the related name where necessary.
   **
   ** On entry,
   **   aName           A filename given
   **      relatedName     A name relative to which aName is to be parsed
   **      wanted          A mask for the bits which are wanted.
   **
   ** On exit,
   **   returns         A pointer to a malloc'd string which MUST BE FREED
 */
    PUBLIC char    *HTParse ARGS3(CONST char *, aName,
                                  CONST char *, relatedName, int, wanted) {
        char           *result = NULL;
        char           *return_value = NULL;
        int             len;
        char           *name = NULL;
        char           *rel = NULL;
        char           *p;
        char           *access;
        struct struct_parts given, related;

        if (TRACE)
            fprintf(stderr,
                    "HTParse: aName:%s   relatedName:%s\n", aName, relatedName);

        /* 
         **  Allocate the output string.
         */
        len = strlen(aName) + strlen(relatedName) + 10;
        result = (char *) malloc(len);  /* Lots of space: more than enough */
        if (result == NULL)
            outofmem(__FILE__, "HTParse");
        result[0] = '\0';       /* Clear string */

        /* 
         **  Make working copies of the input strings to cut up.
         */
        StrAllocCopy(name, aName);
        StrAllocCopy(rel, relatedName);

        /* 
         **  Cut up the strings into URL fields.
         */
        scan(name, &given);
        scan(rel, &related);

        /* 
         **  Handle the scheme (access) field.
         */
        if (given.access && given.host && !given.relative && !given.absolute) {
            if (!strcmp(given.access, "http") ||
                !strcmp(given.access, "https") || !strcmp(given.access, "ftp"))
                /* 
                 **  Assume root.
                 */
                given.absolute = "";
        }
        access = given.access ? given.access : related.access;
        if (wanted & PARSE_ACCESS) {
            if (access) {
                strcat(result, access);
                if (wanted & PARSE_PUNCTUATION)
                    strcat(result, ":");
            }
        }

        /* 
         **  If different schemes, inherit nothing.
         **
         **  We'll try complying with RFC 1808 and
         **  the Fielding draft, and inherit nothing
         **  if both schemes are given, rather than
         **  only when they differ, except for
         **  file URLs - FM
         **
         **  After trying it for a while, it's still
         **  premature, IHMO, to go along with it, so
         **  this is back to inheriting for identical
         **  schemes whether or not they are "file".
         **  If you want to try it again yourself,
         **  uncomment the strncasecomp() below. - FM
         */
        if ((given.access && related.access) && (       /* strcasecomp(given.access, 
                                                         * "file") || */
                                                    strcmp(given.access,
                                                           related.access))) {
            related.host = NULL;
            related.absolute = NULL;
            related.relative = NULL;
            related.anchor = NULL;
        }

        /* 
         **  Handle the host field.
         */
        if (wanted & PARSE_HOST)
            if (given.host || related.host) {
                char           *tail = result + strlen(result);
                if (wanted & PARSE_PUNCTUATION)
                    strcat(result, "//");
                strcat(result, given.host ? given.host : related.host);
#define CLEAN_URLS
#ifdef CLEAN_URLS
                /* 
                 **  Ignore default port numbers, and trailing dots on FQDNs,
                 **  which will only cause identical addresses to look different.
                 */
                {
                    char           *p, *h;
                    p = strchr(tail, ':');
                    if (p != NULL && !isdigit((unsigned char) p[1]))
                        /* 
                         **  Colon not followed by a port number.
                         */
                        *p = '\0';
                    if (p != NULL && p != '\0' && access != NULL) {
                        /* 
                         **  Port specified.
                         */
                        if ((!strcmp(access, "http") && !strcmp(p, ":80")) ||
                            (!strcmp(access, "gopher") && !strcmp(p, ":70")) ||
                            (!strcmp(access, "ftp") && !strcmp(p, ":21")) ||
                            (!strcmp(access, "wais") && !strcmp(p, ":210")) ||
                            (!strcmp(access, "nntp") && !strcmp(p, ":119")) ||
                            (!strcmp(access, "news") && !strcmp(p, ":119")) ||
                            (!strcmp(access, "snews") && !strcmp(p, ":563")) ||
                            (!strcmp(access, "finger") && !strcmp(p, ":79")) ||
                            (!strcmp(access, "cso") && !strcmp(p, ":105")))
                            *p = '\0';  /* It is the default: ignore it */
                    }
                    if (p == NULL) {
                        int             len = strlen(tail);

                        if (len > 0) {
                            h = tail + len - 1; /* last char of hostname */
                            if (*h == '.')
                                *h = '\0';      /* chop final . */
                        }
                    } else {
                        h = p;
                        h--;    /* End of hostname */
                        if (*h == '.') {
                            /* 
                             **  Slide p over h.
                             */
                            while (*p != '\0')
                                *h++ = *p++;
                            *h = '\0';  /* terminate */
                        }
                    }
                }
#endif                          /* CLEAN_URLS */
            }

        /* 
         **  If different hosts, inherit no path.
         */
        if (given.host && related.host)
            if (strcmp(given.host, related.host) != 0) {
                related.absolute = NULL;
                related.relative = NULL;
                related.anchor = NULL;
            }

        /* 
         **  Handle the path.
         */
        if (wanted & PARSE_PATH) {
            if (access && !given.absolute && given.relative) {
                if (!strcasecomp(access, "nntp") ||
                    !strcasecomp(access, "snews") ||
                    (!strcasecomp(access, "news") &&
                     !strncasecomp(result, "news://", 7))) {
                    /* 
                     *  Treat all given nntp or snews paths,
                     *  or given paths for news URLs with a host,
                     *  as absolute.
                     */
                    given.absolute = given.relative;
                    given.relative = NULL;
                }
            }
            if (given.absolute) {       /* All is given */
                if (wanted & PARSE_PUNCTUATION)
                    strcat(result, "/");
                strcat(result, given.absolute);
                if (TRACE)
                    fprintf(stderr, "1\n");
            } else if (related.absolute) {      /* Adopt path not name */
                strcat(result, "/");
                strcat(result, related.absolute);
                if (given.relative) {
                    p = strchr(result, '?');    /* Search part? */
                    if (p == NULL)
                        p = (result + strlen(result) - 1);
                    for (; *p != '/'; p--);     /* last / */
                    p[1] = '\0';        /* Remove filename */
                    strcat(result, given.relative);     /* Add given one */
                    HTSimplify(result);
                }
                if (TRACE)
                    fprintf(stderr, "2\n");
            } else if (given.relative) {
                strcat(result, given.relative); /* what we've got */
                if (TRACE)
                    fprintf(stderr, "3\n");
            } else if (related.relative) {
                strcat(result, related.relative);
                if (TRACE)
                    fprintf(stderr, "4\n");
            } else {            /* No inheritance */
                if (strncasecomp(aName, "lynxcgi:", 8) &&
                    strncasecomp(aName, "lynxexec:", 9) &&
                    strncasecomp(aName, "lynxprog:", 9)) {
                    strcat(result, "/");
                }
                if (!strcmp(result, "news:/"))
                    result[5] = '*';
                if (TRACE)
                    fprintf(stderr, "5\n");
            }
        }

        /* 
         **  Handle the fragment (anchor).
         */
        if (wanted & PARSE_ANCHOR)
            if ((given.anchor && *given.anchor) || (!given.anchor && related.anchor)) {
                if (wanted & PARSE_PUNCTUATION)
                    strcat(result, "#");
                strcat(result, (given.anchor) ? given.anchor : related.anchor);
            }
        if (TRACE)
            fprintf(stderr, "HTParse: result:%s\n", result);
        FREE(rel);
        FREE(name);

        StrAllocCopy(return_value, result);
        FREE(result);

        return return_value;    /* exactly the right length */
    }

/*      Simplify a filename.                            HTSimplify()
   **   --------------------
   **
   **  A unix-style file is allowed to contain the seqeunce xxx/../ which may
   **  be replaced by "" , and the seqeunce "/./" which may be replaced by "/".
   **  Simplification helps us recognize duplicate filenames.
   **
   **   Thus,   /etc/junk/../fred       becomes /etc/fred
   **           /etc/junk/./fred        becomes /etc/junk/fred
   **
   **      but we should NOT change
   **           http://fred.xxx.edu/../..
   **
   **   or      ../../albert.html
 */
    PUBLIC void HTSimplify ARGS1(char *, filename) {
        char           *p;
        char           *q, *q1;

        if (filename == NULL)
            return;

        if ((filename[0] && filename[1]) && strchr(filename, '/') != NULL) {
            for (p = (filename + 2); *p; p++) {
                if (*p == '/') {
                    if ((p[1] == '.') && (p[2] == '.') &&
                        (p[3] == '/' || p[3] == '\0')) {
                        /* 
                         **  Handle "/../" or "/..".
                         */
                        for (q = (p - 1); (q >= filename) && (*q != '/'); q--)
                            /* 
                             **  Back up to previous slash or beginning of string.
                             */
                            ;
                        if ((q[0] == '/') && strncmp(q, "/../", 4) &&
                            !((q - 1) > filename && q[-1] == '/')) {
                            /* 
                             **  Not at beginning of string or in a
                             **  host field, so remove the "/xxx/..".
                             */
                            q1 = (p + 3);
                            p = q;
                            while (*q1 != '\0')
                                *p++ = *q1++;
                            *p = '\0';  /* terminate */
#ifdef NOTDEFINED
                            /* 
                             **  Make sure filename has at least one slash.
                             */
                            if (*filename == '\0') {
                                *filename = '/';
                                *(filename + 1) = '\0';
                            }
#endif                          /* NOTDEFINED */
                            /* 
                             **  Start again with previous slash.
                             */
                            p = (q - 1);
                        }
                    } else if (p[1] == '.' && p[2] == '/') {
                        /* 
                         **  Handle "./" by removing the characters.
                         */
                        q = p;
                        q1 = (p + 2);
                        while (*q1 != '\0')
                            *q++ = *q1++;
                        *q = '\0';      /* terminate */
                        p--;
                    } else if (p[1] == '.' && p[2] == '\0') {
                        /* 
                         **  Handle terminal "." by removing the character.
                         */
                        p[1] = '\0';
                    }
                }
            }
        }
    }

/*      Make Relative Name.                                     HTRelative()
   **   -------------------
   **
   ** This function creates and returns a string which gives an expression of
   ** one address as related to another. Where there is no relation, an absolute
   ** address is retured.
   **
   **  On entry,
   **   Both names must be absolute, fully qualified names of nodes
   **   (no anchor bits)
   **
   **  On exit,
   **   The return result points to a newly allocated name which, if
   **   parsed by HTParse relative to relatedName, will yield aName.
   **   The caller is responsible for freeing the resulting name later.
   **
 */
    PUBLIC char    *HTRelative ARGS2(CONST char *, aName, CONST char *, relatedName) {
        char           *result = NULL;
        CONST char     *p = aName;
        CONST char     *q = relatedName;
        CONST char     *after_access = NULL;
        CONST char     *path = NULL;
        CONST char     *last_slash = NULL;
        int             slashes = 0;

        for (; *p; p++, q++) {  /* Find extent of match */
            if (*p != *q)
                break;
            if (*p == ':')
                after_access = p + 1;
            if (*p == '/') {
                last_slash = p;
                slashes++;
                if (slashes == 3)
                    path = p;
            }
        }

        /* q, p point to the first non-matching character or zero */

        if (!after_access) {    /* Different access */
            StrAllocCopy(result, aName);
        } else if (slashes < 3) {       /* Different nodes */
            StrAllocCopy(result, after_access);
        } else if (slashes == 3) {      /* Same node, different path */
            StrAllocCopy(result, path);
        } else {                /* Some path in common */
            int             levels = 0;
            for (; *q && (*q != '#'); q++)
                if (*q == '/')
                    levels++;
            result = (char *) malloc(3 * levels + strlen(last_slash) + 1);
            if (result == NULL)
                outofmem(__FILE__, "HTRelative");
            result[0] = '\0';
            for (; levels; levels--)
                strcat(result, "../");
            strcat(result, last_slash + 1);
        }
        if (TRACE)
            fprintf(stderr, "HT: `%s' expressed relative to\n    `%s' is\n   `%s'.",
                    aName, relatedName, result);
        return result;
    }
