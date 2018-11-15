/*
 *
 *      Filename: ch_strtok.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 16:56:02
 * Last Modified: 2017-02-10 16:42:51
 */

#include "ch_string.h"

char * ch_strtok(char *str, const char *sep, char **last)
{
    char *token;

    if (!str)           /* subsequent call */
        str = *last;    /* start where we left off */

    /* skip characters in sep (will terminate at '\0') */
    while (*str && strchr(sep, *str))
        ++str;

    if (!*str)          /* no more tokens */
        return NULL;

    token = str;

    /* skip valid token characters to terminate token and
     * prepare for the next call (will terminate at '\0) 
     */
    *last = token + 1;
    while (**last && !strchr(sep, **last))
        ++*last;

    if (**last) {
        **last = '\0';
        ++*last;
    }

    return token;
}

/* split string into tokens */                                                                                                  
 int                                                                                                                             
 ch_strsplit(char *string, int stringlen,                                                                                       
          char **tokens, int maxtokens, char delim)                                                                              
 {                                                                                                                               
     int i, tok = 0;                                                                                                             
     int tokstart = 1; /* first token is right at start of string */                                                             
                                                                                                                                 
     if (string == NULL || tokens == NULL)                                                                                       
         goto einval_error;                                                                                                      
                                                                                                                                 
     for (i = 0; i < stringlen; i++) {                                                                                           
         if (string[i] == '\0' || tok >= maxtokens)                                                                              
             break;                                                                                                              
         if (tokstart) {                                                                                                         
             tokstart = 0;                                                                                                       
             tokens[tok++] = &string[i];                                                                                         
         }                                                                                                                       
         if (string[i] == delim) {                                                                                               
             string[i] = '\0';                                                                                                   
             tokstart = 1;                                                                                                       
         }                                                                                                                       
     }                                                                                                                           
     return tok;                                                                                                                 
                                                                                                                                 
 einval_error:                                                                                                                   
     errno = EINVAL;                                                                                                             
     return -1;

}                
