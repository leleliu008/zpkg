#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "core/sysinfo.h"
#include "core/util.h"
#include "core/fs.h"
#include "ppkg.h"

#include <git2.h>
#include <yaml.h>
#include <jansson.h>
#include <archive.h>
#include <curl/curlver.h>
#include <openssl/opensslv.h>

//#define PCRE2_CODE_UNIT_WIDTH 8
//#include <pcre2.h>

static int ppkg_list_dirs(const char * installedDir, size_t installedDirLength, const char * sub) {
    DIR           * dir;
    struct dirent * dir_entry;

    dir = opendir(installedDir);

    if (dir == NULL) {
        perror(installedDir);
        return PPKG_ERROR;
    }

    while ((dir_entry = readdir(dir))) {
        //puts(dir_entry->d_name);

        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t  packageInstalledDirLength = installedDirLength + strlen(dir_entry->d_name) + 2;
        char    packageInstalledDir[packageInstalledDirLength];
        memset (packageInstalledDir, 0, packageInstalledDirLength);
        snprintf(packageInstalledDir, packageInstalledDirLength, "%s/%s", installedDir, dir_entry->d_name);

        size_t  receiptFilePathLength = packageInstalledDirLength + 20;
        char    receiptFilePath[receiptFilePathLength];
        memset (receiptFilePath, 0, receiptFilePathLength);
        snprintf(receiptFilePath, receiptFilePathLength, "%s/.ppkg/receipt.yml", packageInstalledDir);

        if (exists_and_is_a_regular_file(receiptFilePath)) {
            if ((sub == NULL) || (strcmp(sub, "") == 0)) {
                puts(packageInstalledDir);
            } else {
                size_t  subDirLength = packageInstalledDirLength + strlen(sub) + 2;
                char    subDir[subDirLength];
                memset (subDir, 0, subDirLength);
                snprintf(subDir, subDirLength, "%s/%s", packageInstalledDir, sub);

                if (exists_and_is_a_directory(subDir)) {
                    puts(subDir);
                }
            }
        }
    }

    closedir(dir);

    return PPKG_OK;
}

int ppkg_env(bool verbose) {
    printf("compiled.time: %s\n\n", PPKG_COMPILED_TIMESTAMP);

    //printf("pcre2   : %d.%d\n", PCRE2_MAJOR, PCRE2_MINOR);
    printf("libyaml : %s\n", yaml_get_version_string());
    printf("libcurl : %s\n", LIBCURL_VERSION);
    printf("libgit2 : %s\n", LIBGIT2_VERSION);

//https://www.openssl.org/docs/man3.0/man3/OPENSSL_VERSION_BUILD_METADATA.html
//https://www.openssl.org/docs/man1.1.1/man3/OPENSSL_VERSION_TEXT.html
#ifdef OPENSSL_VERSION_STR
    printf("openssl : %s\n", OPENSSL_VERSION_STR);
#else
    printf("openssl : %s\n", OPENSSL_VERSION_TEXT);
#endif

    printf("jansson : %s\n", JANSSON_VERSION);
    printf("archive : %s\n\n", ARCHIVE_VERSION_ONLY_STRING);

    SysInfo sysinfo = {0};

    if (sysinfo_make(&sysinfo) != 0) {
        return PPKG_ERROR;
    }

    sysinfo_dump(sysinfo);
    sysinfo_free(sysinfo);

    printf("\n");

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return PPKG_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return PPKG_ENV_HOME_NOT_SET;
    }

    size_t  ppkgHomeDirLength = userHomeDirLength + 7;
    char    ppkgHomeDir[ppkgHomeDirLength];
    memset (ppkgHomeDir, 0, ppkgHomeDirLength);
    snprintf(ppkgHomeDir, ppkgHomeDirLength, "%s/.ppkg", userHomeDir);

    printf("ppkg.vers : %s\n", PPKG_VERSION);
    printf("ppkg.home : %s\n", ppkgHomeDir);

    char * path = NULL;

    get_current_executable_realpath(&path);

    printf("ppkg.path : %s\n", path == NULL ? "" : path);

    if (path != NULL) {
        free(path);
    }

    printf("ppkg.github: https://github.com/leleliu008/ppkg\n");

    if (!verbose) {
        return PPKG_OK;
    }

    size_t  installedDirLength = ppkgHomeDirLength + 11;
    char    installedDir[installedDirLength];
    memset (installedDir, 0, installedDirLength);
    snprintf(installedDir, installedDirLength, "%s/installed", ppkgHomeDir);

    if (!exists_and_is_a_directory(installedDir)) {
        return PPKG_OK;
    }

    printf("\nbinDirs:\n");
    ppkg_list_dirs(installedDir, installedDirLength, "bin");

    printf("\nlibDirs:\n");
    ppkg_list_dirs(installedDir, installedDirLength, "lib");

    printf("\naclocalDirs:\n");
    ppkg_list_dirs(installedDir, installedDirLength, "share/aclocal");
    
    return PPKG_OK;
}
