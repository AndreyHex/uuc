#ifndef uuc_log_h
#define uuc_log_h

#define LOG_INFO(str, ...) do { printf("[INFO] [%s] ", __FILE__);printf(str, ##__VA_ARGS__); } while(0);
#define LOG_ERROR(str, ...) do { printf("[ERROR] [%s] ", __FILE__);printf(str, ##__VA_ARGS__); } while(0);

#if defined(UUC_LOG_DEBUG)
#define LOG_DEBUG(str, ...) do { printf("[DEBUG] [%s] ", __FILE__);printf(str, ##__VA_ARGS__); } while(0);
#else
#define LOG_DEBUG(str, ...)
#endif

#if defined(UUC_LOG_TRACE)
#define LOG_TRACE(str, ...) do { printf("[TRACE] [%s] ", __FILE__);printf(str, ##__VA_ARGS__); } while(0);
#else
#define LOG_TRACE(str, ...)
#endif

#endif
