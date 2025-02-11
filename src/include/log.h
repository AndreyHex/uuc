#ifndef uuc_log_h
#define uuc_log_h

#include <stdio.h>

#if defined(UUC_LOG_DEBUG) || defined(UUC_LOG_TRACE)
#define LOG_INFO(str, ...) do { printf("[INFO] [%s:%d] ", __FILE__, __LINE__);printf(str, ##__VA_ARGS__); } while(0);
#define LOG_ERROR(str, ...) do { printf("[\033[31mERROR\033[m] [%s:%d] ", __FILE__, __LINE__);printf(str, ##__VA_ARGS__); } while(0);
#else
#define LOG_INFO(str, ...) do { printf("[INFO] ");printf(str, ##__VA_ARGS__); } while(0);
#define LOG_ERROR(str, ...) do { printf("[\033[31mERROR\033[m] ");printf(str, ##__VA_ARGS__); } while(0);
#endif

#if defined(UUC_LOG_DEBUG)
#define LOG_DEBUG(str, ...) do { printf("[DEBUG] [%s:%d] ", __FILE__, __LINE__);printf(str, ##__VA_ARGS__); } while(0);
#else
#define LOG_DEBUG(str, ...)
#endif

#if defined(UUC_LOG_TRACE)
#define LOG_TRACE(str, ...) do { printf("[TRACE] [%s:%d] ", __FILE__, __LINE__);printf(str, ##__VA_ARGS__); } while(0);
#else
#define LOG_TRACE(str, ...)
#endif

#endif
