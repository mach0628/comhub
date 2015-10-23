
#ifndef _UTILS_H_
#define _UTILS_H_



/* ******************************* */

class Utils {
 private:

 public:
  static int GetCString(const char * src,char * dim,int max);
  static int GetWord(const char * src,char * dim,int max);
  static char* Trim(char *s);
  static u_int32_t HashString(char *s);
  static float timeval2ms(struct timeval *tv);
  static bool mkdir_tree(char *path);
  static void RemoveTrailingSlash(char *str);
  static void fixPath(char *str);
  
  static void formattime(char * buff,time_t utc);
  static void formattime2(char * buff,time_t utc);
  static char * iptos(u_long in);
  static long time_offset;
};


#endif /* _UTILS_H_ */
