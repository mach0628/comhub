
#include "stdafx.h"
#include "Utils.h"



long Utils::time_offset =  timezone ;//- (daylight * 3600);
/* ****************************************************** */
int Utils::GetWord(const char * src,char * dim,int max)
{
	int len =0;
	max--;
	while( isspace(src[0]) ) src++;
	while( (!isspace(src[0])) && (len<max ) ){
		dim[0]=src[0];
		src++;dim++;
		len++;
	}
	dim[len]=0;
	return len;
}

int Utils::GetCString(const char * src,char * dim,int max)
{
	int len =0;
	max--;
	while( (src[0]!=0) && (src[0]!='"') ) src++;
	if( src[0]!=0 ){
		src++;
		while( (src[0]!=0) && (src[0]!='"') &&( len<max) ){
			if( src[0]=='\\' ){
				if(src[1]!=0){
					src++;
					if( src[0]=='t' ) dim[0]='\t';
					 else if( src[0]=='n' ) dim[0]='\n';
					   else if( src[0]=='r' ) dim[0]='\r';
					   else if( src[0]=='c' ){
						   switch(src[1]){
						   case '@': dim[0]=0;break;
						   case '[': dim[0]=0x1b;break;
						   case '/': dim[0]=0x1c;break;
						   case ']':dim[0]=0x1d;break;
						   case '^':dim[0]=0x1e;break;
						   case '_':dim[0]=0x1f;break;
						   default:
							 dim[0]=src[1]&0x1F;
						   }
						   if( dim[0]>0 ) src++;
					   } else dim[0]=src[0];
				    src++;dim++;len++;
					continue;
				}
			}
			dim[0]=src[0];
			dim++;src++;
			len ++;
		}
	}
	dim[len]=0;
	return len;
}
/* ****************************************************** */

char *Utils::Trim(char *s) {
  char *end;

  while(isspace(s[0])) s++;
  if(s[0] == 0) return s;

  end = &s[strlen(s) - 1];
  while(end > s && isspace(end[0])) end--;
  end[1] = 0;

  return s;
}

/* ****************************************************** */

u_int32_t Utils::HashString(char *key) {
  u_int32_t hash = 0, len = (u_int32_t)strlen(key);

  for(u_int32_t i=0; i<len; i++)
    hash += ((u_int32_t)key[i])*i;

  return(hash);
}

/* ****************************************************** */

float Utils::timeval2ms(struct timeval *tv) {
  return((float)tv->tv_sec*1000+(float)tv->tv_usec/1000);
}

/* ****************************************************** */

bool Utils::mkdir_tree(char *path) {
  int permission = 0777, i, rc;
  struct stat s;

  fixPath(path);

  if(stat(path, &s) != 0) {
    /* Start at 1 to skip the root */
    for(i=1; path[i] != '\0'; i++)
      if(path[i] == CONST_PATH_SEP) {
#ifdef WIN32
	/* Do not create devices directory */
	if((i > 1) && (path[i-1] == ':')) continue;
#endif

	path[i] = '\0';
	rc = mkdir_s(path, permission);
	path[i] = CONST_PATH_SEP;
      }

    rc = mkdir_s(path, permission);

    return(rc == 0 ? true : false);
  } else
    return(true); /* Already existing */
}

void Utils::RemoveTrailingSlash(char *str) {
  int len = (int)strlen(str)-1;

  if((len > 0)
     && ((str[len] == '/') || (str[len] == '\\')))
    str[len] = '\0';
}

void Utils::formattime(char * buff,time_t utc)
{
	struct tm l;
	localtime_r(&utc,&l);
	sprintf(buff,"%d-%02d-%02d %02d:%02d:%02d",l.tm_year+1900,l.tm_mon+1,l.tm_mday,l.tm_hour,l.tm_min,l.tm_sec);
}
static   char *log_month[12]={ "Jan", "Feb", "Mar",
                         "Apr", "May", "Jun",
                         "Jul", "Aug", "Sep",
                         "Oct", "Nov", "Dec"};
void Utils::formattime2(char * buff,time_t utc)
{
	struct tm l;
	localtime_r(&utc,&l);
	//23/Jun/2014:09:41:07 +0800
	sprintf(buff,"%02d/%s/%d:%d:%02d:%02d +0800",l.tm_mday,log_month[l.tm_mon],l.tm_year+1900,l.tm_hour,l.tm_min,l.tm_sec); 
}
/* ******************************************* */

void Utils::fixPath(char *str) {
#ifdef WIN32
  for(int i=0; str[i] != '\0'; i++)
    if(str[i] == '/') str[i] = '\\';
#endif
}

