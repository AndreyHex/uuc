#ifndef uuc_text_h 
#define uuc_text_h

typedef enum {
    PASS,
    FAIL,
} TestStatus;

typedef struct {
    char *message;
    TestStatus result;
} TestResult;


#endif
