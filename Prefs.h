#ifndef _PREFS_H_
#define _PREFS_H_



class Prefs {
 private:
   char working_dir[MAX_PATH], install_dir[MAX_PATH], startup_dir[MAX_PATH];
   char program_name[64];
   bool daemonize;
  
    char *data_dir;
  
  char *config_file_path;
  char * script_file;
  FILE *logFd;
  char *pid_path;
  void SetWorkingDir(char *dir); 
  
  int SetOption(int optkey, char *optarg);
  int CheckOptions();
  void KillDaemon();
  int LoadFromFile(const char *path);
  void Usage();
public:

 public:
  Prefs( char * app);
  ~Prefs();
  static const int MAX_COMM=100;
	 char * mComms[100];
	 int mBaudRate;
	 int mParity;
	 int mStopBits;
	 int mDataBtis;
	 int mListenPort;
	 int mKbEnable;
	 int mKaTime1;
	 int mKaTime2;
#ifndef WIN32
	 bool mNoESC;
#endif
	static const int mRateList[11];
	inline char* WorkingDir()                     { return(working_dir);      };
	inline char * ScriptFile() { return script_file; };
	//inline char* InstallDir()                     { return(install_dir);      };
	//inline char* StartupDir()                     { return(startup_dir);      };
	inline char * ProgramName()   { return program_name;};
	char* GetValidPath(char *path);


 
  inline FILE* LogFd()                             { return(logFd);                  };
  inline char* DataDir()                           { return(data_dir);       };
  inline char* PidPath()                           { return(pid_path);       };
  inline bool IsDaemonize()                        { return(daemonize);      };

  int LoadFromCLI(int argc,  char *argv[]);


};
extern Prefs * gPrefs;

#endif /* _PREFS_H_ */
