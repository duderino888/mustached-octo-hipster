#ifndef LOG_H
#define LOG_H


#define log_info(...)   log_msg("INFO", __VA_ARGS__)
#define log_warn(...)   log_msg("WARN", __VA_ARGS__)
#define log_error(...)  log_msg("ERROR", __VA_ARGS__)
#define log_debug(...)  do{ if (DEBUG) log_msg("DEBUG", __VA_ARGS__); }while(0)


void log_msg(const char *tag, const char *fmt, ...);


#endif /* LOG_H */

