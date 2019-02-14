/*
 * =====================================================================================
 *
 *       Filename:  do_telnet_format.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2018 04:08:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#define TELNET_BUF_SIZE                  256
#define TELNET_LOGIN_SIZE               (1024*512)

static const char* USER[] = {"Username:","login:","User:",NULL};

static const char * get_login_prefix(char *buf){

    const char *p;
    int i =0;

    while((p=USER[i++])){

        if(strcasestr(buf,p)!=NULL)
            return p;
    }

    return NULL;
}

static int login_parse(char *buf, char *user, char *pswd, int bsize)
{
    const char *lgn_prefix;
    char *tmp, *lgn;
    long size, i;

    lgn_prefix = get_login_prefix(buf);
    if(lgn_prefix == NULL)
        return 0;

    lgn = strcasestr(buf, lgn_prefix);
    if (lgn != NULL && strchr(lgn, '\r') != NULL) {
        /* last login */
        tmp = lgn;
        while (tmp != NULL) {
            tmp = strcasestr(tmp + 1, lgn_prefix);
            if (tmp != NULL && tmp[-1] != ' ' && tmp[-1] != '\n') {
                if (strcasestr(tmp, "Password:") != NULL)
                    lgn = tmp;
                else
                    tmp = NULL; 
            }
        }
        lgn += strlen(lgn_prefix);
        tmp = strchr(lgn, '\r');
        size = tmp - lgn;
        if (bsize < size) {
            size = bsize - 1;
        }
        memcpy(user, lgn, size);
        user[size] = '\0';

        /* password */
        lgn = strcasestr(lgn, "Password:");
        if (lgn != NULL) {
            lgn += 9;
            tmp = strchr(lgn, '\r');
            size = tmp - lgn;
            if (bsize < size) {
                size = bsize - 1;
            }

            if(size>0){
                memcpy(pswd, lgn, size);
                pswd[size] = '\0';
            }

        }

        /* echo remove */
        size = strlen(user);
        if (size) {
            tmp = user;
            if (user[0] == ' ') {
                tmp++;
                size--;
            }
            while (size) {
                if (tmp[0] != tmp[1])
                    break;
                tmp += 2;
                size -=2;
            }
            
            if (size == 0) {
                /* echo remove */
                size = strlen(user)/2;
                tmp = user;
                i = 0;
                while (i != size) {
                    tmp[i] = user[i*2+1];
                    i++;
                }
                tmp[i] = '\0';
            }
        }
    }

    return 0;
}

 static size_t _get_fsize(const char *fname){
 
      struct stat statbuf;
      stat(fname,&statbuf);
 
      return statbuf.st_size;
 }


void do_telnet_session_format(msgpack_packer *pk,void *session_in){

    char user[TELNET_BUF_SIZE] = {0};
    char password[TELNET_BUF_SIZE] = {0};
	char *buf = NULL;
    size_t bsize = 0;

    ch_telnet_session_entry_t * session = (ch_telnet_session_entry_t*)session_in;

	bsize = _get_fsize(session->content_fpath);
	if(bsize>TELNET_LOGIN_SIZE){
		bsize = TELNET_LOGIN_SIZE;
	}
	
	if(bsize>0){

        FILE *fp = fopen(session->content_fpath,"r");
        if(fp){
            
            buf = calloc(1,bsize+1);
            if(buf){
                
                fread(buf,1,bsize,fp);

                login_parse(buf,user,password,TELNET_BUF_SIZE);

                free(buf);

            }

            fclose(fp);

        }
	}

	ch_msgpack_map_start(pk,"telnet",3);
	
	ch_msgpack_write_kv(pk,"user",user);
	
    ch_msgpack_write_kv(pk,"passwd",password);

	ch_msgpack_write_kv(pk,"contentPath",session->content_fpath);

}
