/**
 * @file logger.h
 * @author abdllrhmanzedan
 * @brief Defines a self-contained logger interface and implementation in C, compatible with C++.
 */

#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum LOGLEVEL LOGLEVEL;

// logging levels helpers
enum LOGLEVEL
{
    INFO,    // blue
    DEBUG,   // cyan
    SUCCESS, // green
    WARNING, // yellow
    CRITICAL // red
};

const char *log_level_string[] = {"INFO", "DEBUG", "SUCCESS", "WARNING", "CRITICAL", "UNKNOWN"};
const char *log_level_color[] = {"\x1b[34m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m"};
const char *reset_color = "\x1b[0m";

// logging time helpers
time_t log_time;
struct tm *log_time_info;
char timestamp[25];

// logging files helpers
FILE *file = NULL;

// logging functionalities
void getTimeStamp()
{
    // clearing old timestamp
    memset(timestamp, 0, sizeof(timestamp));

    // setting & formatting timestamp
    log_time = time(0);
    log_time_info = localtime(&log_time);
    strftime(timestamp, sizeof(timestamp), "%d-%m-%Y %H:%M:%S", log_time_info);
}

void consoleLog(const char *message, LOGLEVEL level)
{
    // format: [timestamp] LEVEL : message
    getTimeStamp();
    fprintf(stderr, "%s[%s] %-10s : %s %s\n",
            log_level_color[level], timestamp, log_level_string[level], message, reset_color);
}

void openLogFile(const char *path)
{
    file = fopen(path, "w");
}

void fileLog(const char *message, LOGLEVEL level)
{
    if (file == NULL)
    {
        consoleLog("Log file isn't open!", CRITICAL);
        return;
    }
    getTimeStamp();
    fprintf(file, "%s[%s] %-10s : %s %s\n",
            log_level_color[level], timestamp, log_level_string[level], message, reset_color);
}

void meowLog(const char *message, LOGLEVEL level)
{
    consoleLog(message, level);
    fileLog(message, level);
}

#endif