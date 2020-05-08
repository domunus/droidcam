#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/stat.h>

#include "common.h"
#include "settings.h"

static inline FILE *GetFile(const char* mode) {
    char buf[PATH_MAX];
    struct stat st = {0};

    snprintf(buf, sizeof(buf), "%s/.droidcam", getenv("HOME"));
    if (stat(buf, &st) == -1) {
        mkdir(buf, 0700);
    }

    snprintf(buf, sizeof(buf), "%s/.droidcam/settings", getenv("HOME"));
    return fopen(buf, mode);
}

void LoadSettings(struct settings* settings) {
    char buf[512];
    int version = 0;
    FILE * fp = GetFile("r");

    // set defaults
    settings->audio = 0;
    settings->connection = CB_RADIO_WIFI;
    settings->ip[0] = 0;
    settings->port = 4747;

    if (!fp) {
        MSG_ERROR("Couldnt load settings");
        return;
    }

    if(fgets(buf, sizeof(buf), fp)){
        sscanf(buf, "v%d", &version);
    }

    if (version == 1) {
        if (fgets(buf, sizeof(buf), fp)){
            buf[strlen(buf)-1] = '\0';
            strncpy(settings->ip, buf, sizeof(settings->ip));
        }

        if (fgets(buf, sizeof(buf), fp)) {
            buf[strlen(buf)-1] = '\0';
            settings->port = atoi(buf);
        }
    }
    else if (version == 2) {
        if (fgets(buf, sizeof(buf), fp)){
            buf[strlen(buf)-1] = '\0';
            strncpy(settings->ip, buf, sizeof(settings->ip));
        }
        if (fgets(buf, sizeof(buf), fp)) {
            sscanf(buf, "%d", &settings->port);
        }
        if (fgets(buf, sizeof(buf), fp)) {
            sscanf(buf, "%d", &settings->audio);
        }
        if (fgets(buf, sizeof(buf), fp)) {
            sscanf(buf, "%d", &settings->connection);
        }
    }

    fclose(fp);
    dbgprint(
        "settings: ip=%s\n"
        "settings: port=%d\n"
        "settings: audio=%d\n"
        "settings: connection=%d\n"
        ,
        settings->ip,
        settings->port,
        settings->audio,
        settings->connection);
}

void SaveSettings(struct settings* settings) {
    int version = 2;
    FILE * fp = GetFile("w");
    if (!fp) return;

    fprintf(fp,
        "v%d\n"
        "%s\n"
        "%d\n"
        "%d\n"
        "%d\n"
        ,
        version,
        settings->ip,
        settings->port,
        settings->audio,
        settings->connection);
    fclose(fp);
}