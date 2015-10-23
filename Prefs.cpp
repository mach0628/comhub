#include "stdafx.h"
#include "Prefs.h"

#include "getopt.h"
#include "Utils.h"
#include "Serial.h"

#ifdef WIN32
#include <shlobj.h> /* SHGetFolderPath() */
#endif

/* ******************************************* */
Prefs * gPrefs=NULL;
#define CONST_DEFAULT_DATA_DIR      "data"

#define DEFAULT_PID_PATH        "/var/tmp/"
#define CONST_DEFAULT_WRITABLE_DIR  "/var/tmp"
#define CONST_DEFAULT_INSTALL_DIR   "/usr/local/share/comhub"

/* SVN Release */
#define COMHUB_SVN_RELEASE "r328"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Architecture of this host */
#define PACKAGE_MACHINE "x86_64"
/* SVN release of this package */
#define PACKAGE_RELEASE "r328"
/* Define to the version of this package. */
#define PACKAGE_VERSION "3.2"


 const int Prefs::mRateList[11]={
CBR_2400,CBR_4800  ,CBR_9600  ,CBR_14400 ,CBR_19200 ,CBR_38400 ,
CBR_56000 ,CBR_57600 ,CBR_115200, CBR_128000,CBR_256000
};

Prefs::Prefs( char * app) {
	char * cp;
	if ((cp = strrchr(app, CONST_PATH_SEP)) != NULL)
		strcpy(program_name ,cp + 1 );
	else strcpy(program_name ,app);
	if ((cp = strrchr(program_name, '.')) != NULL)	cp[0]=0;
	mBaudRate = 9600;
	mParity = 0;
	mStopBits = 1;
	mDataBtis = 8;
	mListenPort = 0;
	mKbEnable = 1;
	mKaTime1 = 2000;
	mKaTime2 = 1000;
#ifdef WIN32
  if(::SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
		     SHGFP_TYPE_CURRENT, working_dir) != S_OK) {
    strcpy(working_dir, "C:\\Windows\\Temp" ); // Fallback: it should never happen
  }


  // Get the full path and filename of this program
  if(GetModuleFileName(NULL, startup_dir, sizeof(startup_dir)) == 0) {
    startup_dir[0] = '\0';	
  } else {
    for(size_t i=strlen(startup_dir)-1; i>0; i--)
      if(startup_dir[i] == '\\') {
			startup_dir[i] = '\0';
			break;
      }
  }
  strcpy(install_dir, startup_dir);
  //mNoESC = false;
#else
  struct stat statbuf;

  snprintf(working_dir, sizeof(working_dir), "%s/%s", CONST_DEFAULT_WRITABLE_DIR,program_name);

  umask (0);
  mkdir(working_dir, 0777);

  if(getcwd(startup_dir, sizeof(startup_dir)) == NULL)
	  Trace::traceEvent(TRACE_WARNING,"Occurred while checking the current directory (errno=%d)", errno);

  if(stat(CONST_DEFAULT_INSTALL_DIR, &statbuf) == 0)
    strcpy(install_dir, CONST_DEFAULT_INSTALL_DIR);
  else {
    if(getcwd(install_dir, sizeof(install_dir)) == NULL)
      strcpy(install_dir, startup_dir);
  }
  mNoESC = true;
#endif

  data_dir = strdup(CONST_DEFAULT_DATA_DIR);

  config_file_path = NULL;
	script_file = NULL;
	daemonize = false;
  logFd = NULL;
  pid_path = new char[strlen(DEFAULT_PID_PATH)+strlen(program_name)+6];
  sprintf(pid_path,"%s%s.pid",DEFAULT_PID_PATH,program_name);

  for(int i=0;i<MAX_COMM;i++)  mComms[i]= NULL;

}

/* ******************************************* */

Prefs::~Prefs() {
  printf("free 1 ");
  if(logFd) fclose(logFd);
  printf("2 ");
  if(data_dir) free(data_dir);
  printf("4 ");
  if(config_file_path) free(config_file_path);
  printf("5 ");
  if(pid_path) free(pid_path);
  if( script_file ) free(script_file);
  for(int i=0;i<MAX_COMM;i++) if( mComms[i] ) delete mComms[i];
}


char* Prefs::GetValidPath(char *__path) {
  char _path[MAX_PATH];
  struct stat buf;
#ifdef WIN32
  //const char *install_dir = (const char *)install_dir;
#endif
  const char* dirs[] = {
	  startup_dir,
#ifndef WIN32
    CONST_DEFAULT_INSTALL_DIR,
#else
    install_dir,
#endif
    NULL
  };

  if(strncmp(__path, "./", 2) == 0) {
    snprintf(_path, MAX_PATH, "%s/%s", startup_dir, &__path[2]);
	Utils::fixPath(_path);

    if(stat(_path, &buf) == 0)
      return(strdup(_path));
  }

  if((__path[0] == '/') || (__path[0] == '\\')) {
    /* Absolute paths */

    if(stat(__path, &buf) == 0) {
      return(strdup(__path));
    }
  } else
    snprintf(_path, MAX_PATH, "%s", __path);

  /* relative paths */
  for(int i=0; dirs[i] != NULL; i++) {
    char path[MAX_PATH];

    snprintf(path, sizeof(path), "%s/%s", dirs[i], _path);
    Utils::fixPath(path);

    if(stat(path, &buf) == 0) {
      return(strdup(path));
    }
  }

  return(strdup(""));
}

void Prefs::SetWorkingDir(char *dir) 
{
  snprintf(working_dir, sizeof(working_dir), "%s", dir);
  Utils::RemoveTrailingSlash(working_dir);
}

/* ******************************************* */

/* C-binding needed by Win32 service call */
void Prefs::Usage() {


  printf("%s %s v.%s (%s) - (C) 2014-06 atdm.liao\n\n"
	 "Usage:\n"
	 "  %s [-f <defaults file>]  [-d <data dir>] [-i <serial name]\n"
	 "         [-r baudrate] [-p <parity>] [-B <databits>] [-b <stopsbits>] \n"
	 "         [-R] [-v] [-h ] [-k] [-e]\n"
	 "\n"
	 "Options:\n"
	 "[--interface|-i] <serial name>   | Input serial name \n"
	 "[--bits|-B] <databits>         | serial databits,1-8,default is 8\n"
	 "[--baudrate|-r] <baudrate>     | serial  baudrate,2400|4800|9600|14400|19200\n"
	 "[--parity|-p] <parity>         | serial  parity|, N(NO)|O(ODD)|E(EVEN)|MARK(M)|S(SPACE) ,default is N\n"
	 "[--stopsbits|-b] <stops bits>  | serial stops bits , 0 or 1,default is 1\n"
	 "[--listen|-l] <listen port>    | controll port, zero is no control,default is 0\n"
	 "[--keyboard|-a] <number>       | set keyboard support, zero is no,others is true\n"
	 "[--script|-s] <file name>      | command to be execute\n"
	 "[--katime1|-x] number          | keep io alive check time, if zero , do not check,unit is ms\n"
	 "[--katime2|-y] number          | io idle time, unit is ms\n"
#ifndef WIN32
	 "[--noesc|-R]  <number>         | program run on remote,keyborad will disable control code under vt100\n"
#endif
	 "[--defaults-file|-f] <file name>    | Use the specified defaults file\n"
	 "[--data-dir|-d] <path>              | Data directory (must be writable).\n"
	 "                                    | Default: %s\n"
	 "[--pid|-G] <path>                   | Pid file path\n"
	 "[--daemon|-e]                       | Daemonize comhub\n"
	 "[--kill|-k]                         | Kill Daemon\n"
	 "[--verbose|-v]                      | Verbose tracing\n"
	 "[--help|-h]                         | Help\n"
	 ,program_name, PACKAGE_MACHINE, PACKAGE_VERSION, COMHUB_SVN_RELEASE,program_name,CONST_DEFAULT_DATA_DIR
	 );

  printf("\n");

  exit(0);
}

/* ******************************************* */

static const struct option long_options[] = {
  { "bits",                        required_argument, NULL, 'B' },
  { "interface",                         required_argument, NULL, 'i' },
  { "data-dir",                          required_argument, NULL, 'd' },
  { "baudrate",                     required_argument, NULL, 'r' },
  { "parity",				         required_argument, NULL, 'p' },
  { "stopsbits",                     required_argument,       NULL, 'b' },
  { "listen",                     required_argument,       NULL, 'l' },
  { "keyboard",                     required_argument,       NULL, 'a' },
  { "script",                     required_argument,       NULL, 's' },
  { "katime1",                     required_argument,       NULL, 'x' },
  { "katime2",                     required_argument,       NULL, 'y' },
#ifndef WIN32
  { "noesc",                     required_argument,       NULL, 'R' },
#endif
  { "daemonize",                         no_argument,       NULL, 'e' },
  { "defaults-file",					required_argument, NULL,'f' },
  { "verbose",                           no_argument,       NULL, 'v' },
  { "help",                              no_argument,       NULL, 'h' },
  { "pid",                               required_argument, NULL, 'G' },
  { "kill",                               no_argument, NULL, 'k' },
  /* End of options */
  { NULL,                                no_argument,       NULL,  0 }
};

/* ******************************************* */
void Prefs::KillDaemon()
{
	FILE *f;
    int pid = 0; /* pid number from pid file */
	Trace::traceEvent(TRACE_NORMAL,"open pid file[%s]",pid_path);
    if((f = fopen(pid_path, "r")) == 0){
		Trace::traceEvent(TRACE_NORMAL,"Can't open pid file");
		exit(0);
        return ;
    }
    if(fscanf(f, "%d", &pid)!= 1) {
    }
    /* send signal SIGTERM to kill */
    if(pid > 0)  {
#ifndef WIN32
        kill(pid, SIGTERM);
		Trace::traceEvent(TRACE_NORMAL, "process %d killed!",pid);
#endif
    }
    fclose(f);
	exit(0);
}
int Prefs::SetOption(int optkey, char *optarg) {

	//i B r p b   
  switch(optkey) {
  case 'f':
	  if( strlen(optarg)>1 ) return LoadFromFile(optarg);
	  return -1;
	  break;
  case 'v':
    Trace::set_trace_level(MAX_TRACE_LEVEL);
    break;
  case 'G':
	if(pid_path) free(pid_path);
    pid_path = strdup(optarg);
    break;

  case 'k':
	  KillDaemon();
	  break;
#ifndef WIN32
  case 'd':
    SetWorkingDir(optarg);
    break;
#endif

  case 'e':
    daemonize = true;
	mKbEnable = 0;
    break;
  case 'h':
    Usage();
    break;
  case 'R':
#ifndef WIN32
	if(  atoi(optarg)<1 ) mNoESC = false;
	else mNoESC = true;
#endif
	  break;
  case 'l':
	  mListenPort = atoi(optarg);
	  if( mListenPort<0 ) mListenPort =0;
	  break;
  case 'x':
	  mKaTime1 = atoi(optarg);
	  if( mKaTime1<0 ) mKaTime1 =0;
	  break;
  case 'y':
	  mKaTime2 = atoi(optarg);
	  if( mKaTime2<50 ) mKaTime2 =50;
	  break;
  case 'i'://serial name
	  int i;
	  for(i=0;i<MAX_COMM;i++){
		  if( mComms[i]==NULL ){
			  mComms[i] = strdup(optarg);
			  break;
		  }
	  }
	  if( i>=MAX_COMM ) Trace::traceEvent(TRACE_NORMAL,"%s reach serial max %d, ignored",optarg,MAX_COMM);
    break;
  case 'r'://Baudrate
    mBaudRate = atoi(optarg);
	if( mBaudRate<0 ){
		Trace::traceEvent(TRACE_NORMAL,"error baudrate [%s],usage default 9600",optarg);
		mBaudRate = 9600;
	}
    break;
  case 'B'://databits
	mDataBtis = atoi(optarg);
	if( (mDataBtis!=8)&&(mDataBtis!=7) ){
		Trace::traceEvent(TRACE_NORMAL,"error databits [%s],set to default 8",optarg);
		mDataBtis = 8;
	}
    break;
  case 'a':
	     mKbEnable = atoi(optarg);
		 if( mKbEnable<0 ) {
			 Trace::traceEvent(TRACE_NORMAL,"error keyboard enable set[%s],set to false",optarg);
			 mKbEnable = 0;
		 }
		 if( daemonize ) mKbEnable = 0;
		 break;
  case 'p'://Parity
	  switch(optarg[0]){
	  case 'N':
	  case 'n':
		  mParity = NOPARITY;
	  break;
	  case 'E':
	  case 'e':
		  mParity = EVENPARITY;
		  break;
	  case 'o':
	  case 'O':
		  mParity = ODDPARITY;
		  break;
	  default:
		  Trace::traceEvent(TRACE_NORMAL,"not know parity %s,set to no parity",optarg);
	      mParity = NOPARITY;
		  break;
	  }
	  break;
  case 'b'://StopBits
		mStopBits = atoi(optarg);
		if( (mStopBits!=1)&&(mStopBits!=2) ){
			Trace::traceEvent(TRACE_NORMAL,"error stopbits, set to 1",optarg);
			mStopBits = 1;
		}
    break;
  case 's':
	if(script_file) free(script_file);
	if( optarg==NULL || strlen(optarg)<2 ){
		Trace::traceEvent(TRACE_NORMAL,"error script file name '%s'",optarg);
		exit(1);
	}
	if( optarg[0]=='-' ){
		Trace::traceEvent(TRACE_NORMAL,"error script file name '%s'",optarg);
		exit(1);
	}
    script_file = strdup(optarg);
	break;
  default:
    return(-1);
  }

  return(0);
}

/* ******************************************* */

int Prefs::CheckOptions() {
  if(daemonize
#ifdef WIN32
		|| 1
#endif
	  ) {
    char path[MAX_PATH];

    //mkdir_s(data_dir, 0777);
    snprintf(path, sizeof(path), "%s/%s.log", install_dir,program_name );
	Utils::fixPath(path);
    logFd = fopen(path, "a");
	if(logFd)
		Trace::traceEvent(TRACE_NORMAL, "Logging into %s", path);
	else
		Trace::traceEvent(TRACE_WARNING,"Unable to create log %s", path);
  }

  
  free(data_dir);data_dir = strdup(install_dir);

  
  if(!data_dir)         {Trace::traceEvent(TRACE_WARNING, "Unable to locate data dir");      return(-1); }
  Trace::traceEvent(TRACE_NORMAL, "locate data dir: %s",data_dir); 
  return(0);
}

/* ******************************************* */

int Prefs::LoadFromCLI(int argc, char *argv[]) {
  u_char c;
  char buff[128];
  sprintf(buff,"%s.conf",program_name);
  FILE * fd = fopen(buff, "r");
  if( fd ){
	  fclose(fd);
	  LoadFromFile(buff);
  }
  while((c = getopt_long(argc, argv, "i:B:r:p:b:f:d:G:ekvhl:a:s:R:x:y:",
			 long_options, NULL)) != '?') {
    if(c == 255)   return(CheckOptions());
    if( SetOption(c, (char *)optarg)<0 ) return -1;
  }
  Usage();
  return -1;
}

/* ******************************************* */

int Prefs::LoadFromFile(const char *path) {
  char buffer[512], *line, *key, *value;
  FILE *fd;
  const struct option *opt;

  if( config_file_path ) free(config_file_path);

  config_file_path = strdup(path);

  fd = fopen(config_file_path, "r");

  if(fd == NULL) {
    Trace::traceEvent(TRACE_WARNING,"Config file %s not found", config_file_path);
    return(-1);
  }

  while(fd) {
    if(!(line = fgets(buffer, sizeof(buffer), fd)))
      break;

    line = Utils::Trim(line);

    if(strlen(line) < 1 || line[0] == '#')
      continue;

    key = line;
    key = Utils::Trim(key);
    
    value = strrchr(line, '=');

    /* Fallback to space */
    if(value == NULL) value = strrchr(line, ' ');
      
    if(value == NULL)
      value = &line[strlen(line)]; /* empty */
    else
      value[0] = 0, value = &value[1];
    value = Utils::Trim(value);

    //if(strlen(key) > 2) key = &key[2];
    //else key = &key[1];

    opt = long_options;
    while (opt->name != NULL) {
      if((strcmp(opt->name, key) == 0)
	  || ((key[1] == '\0') && (opt->val == key[0]))) {
        if( SetOption(opt->val, value)<0 ) return -1;
        break;
      }
      opt++;
    }
  }

  fclose(fd);
  return 1;
  //return(checkOptions());
}

