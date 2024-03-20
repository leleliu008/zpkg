#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "core/log.h"

#include "ppkg.h"

int ppkg_main(int argc, char* argv[]) {
    if (argc == 1) {
        ppkg_help();
        return PPKG_OK;
    }

    if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
        ppkg_help();
        return PPKG_OK;
    }

    if ((strcmp(argv[1], "-V") == 0) || (strcmp(argv[1], "--version") == 0)) {
        printf("%s\n", PPKG_VERSION);
        return PPKG_OK;
    }

    if (strcmp(argv[1], "sysinfo") == 0) {
        return ppkg_sysinfo();
    }

    if (strcmp(argv[1], "buildinfo") == 0) {
        return ppkg_buildinfo();
    }

    if (strcmp(argv[1], "env") == 0) {
        bool verbose = false;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
                break;
            }
        }

        int ret = ppkg_env(verbose);

        if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    int ret = ppkg_setenv_SSL_CERT_FILE();

    if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
        fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
    } else if (ret == PPKG_ERROR) {
        fprintf(stderr, "occurs error.\n");
    }

    if (ret != PPKG_OK) {
        return ret;
    }

    if (strcmp(argv[1], "setup") == 0) {
        bool verbose = false;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
                break;
            }
        }

        return ppkg_setup(verbose);
    }

    if (strcmp(argv[1], "integrate") == 0) {
        bool verbose = false;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
                break;
            }
        }

        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s integrate <zsh|bash|fish>\n", argv[0]);
            return PPKG_ERROR_ARG_IS_NULL;
        } else if (strcmp(argv[2], "zsh") == 0) {
            return ppkg_integrate_zsh_completion (NULL, verbose);
        } else if (strcmp(argv[2], "bash") == 0) {
            return ppkg_integrate_bash_completion(NULL, verbose);
        } else if (strcmp(argv[2], "fish") == 0) {
            return ppkg_integrate_fish_completion(NULL, verbose);
        } else {
            LOG_ERROR2("unrecognized argument: ", argv[2]);
            return PPKG_ERROR_ARG_IS_INVALID;
        }
    }

    if (strcmp(argv[1], "upgrade-self") == 0) {
        bool verbose = false;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
                break;
            }
        }

        return ppkg_upgrade_self(verbose);
    }

    if (strcmp(argv[1], "cleanup") == 0) {
        bool verbose = false;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
                break;
            }
        }

        return ppkg_cleanup(verbose);
    }

    if (strcmp(argv[1], "gen-url-transform-sample") == 0) {
        return ppkg_generate_url_transform_sample();
    }

    if (strcmp(argv[1], "update") == 0) {
        int ret = ppkg_formula_repo_list_update();

        if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "search") == 0) {
        const char * targetPlatformName = NULL;

        char verbose = false;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strcmp(argv[i], "-p") == 0) {
                targetPlatformName = argv[++i];

                if (targetPlatformName == NULL) {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                }

                if (targetPlatformName[0] == '\0') {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }

                const char * supportedTargetPlatformNames[6] = { "linux", "macos", "freebsd", "openbsd", "netbsd", "dragonflybsd" };

                bool isSupported = false;

                for (int j = 0; j < 6; j++) {
                    if (strcmp(targetPlatformName, supportedTargetPlatformNames[j]) == 0) {
                        isSupported = true;
                        break;
                    }
                }

                if (!isSupported) {
                    LOG_ERROR2("unsupported target platform name: ", targetPlatformName);
                    return PPKG_ERROR_ARG_IS_INVALID;
                }
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            }
        }

        int ret = ppkg_search(argv[2], targetPlatformName, verbose);

        if (ret == PPKG_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s search <KEYWORD>, <KEYWORD> is not given.\n", argv[0]);
        } else if (ret == PPKG_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s search <KEYWORD>, <KEYWORD> is empty string.\n", argv[0]);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "info-available") == 0) {
        const char * targetPlatformName = NULL;

        for (int i = 4; i < argc; i++) {
            if (strcmp(argv[i], "-p") == 0) {
                targetPlatformName = argv[++i];

                if (targetPlatformName == NULL) {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                }

                if (targetPlatformName[0] == '\0') {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }

                const char * supportedTargetPlatformNames[6] = { "linux", "macos", "freebsd", "openbsd", "netbsd", "dragonflybsd" };

                bool isSupported = false;

                for (int j = 0; j < 6; j++) {
                    if (strcmp(targetPlatformName, supportedTargetPlatformNames[j]) == 0) {
                        isSupported = true;
                        break;
                    }
                }

                if (!isSupported) {
                    LOG_ERROR2("unsupported target platform name: ", targetPlatformName);
                    return PPKG_ERROR_ARG_IS_INVALID;
                }
            }
        }

        int ret = ppkg_available_info(argv[2], targetPlatformName, argv[3]);

        if (ret == PPKG_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == PPKG_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == PPKG_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not match pattern %s\n", argv[0], PPKG_PACKAGE_NAME_PATTERN);
        } else if (ret == PPKG_ERROR_ARG_IS_UNKNOWN) {
            fprintf(stderr, "Usage: %s info <PACKAGE-NAME> [KEY], unrecognized KEY: %s\n", argv[0], argv[3]);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package '%s' is not available for target '%s'\n", argv[2], targetPlatformName);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package '%s' is not installed.\n", argv[2]);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "info-installed") == 0) {
        char * targetPlatformSpec = NULL;

        for (int i = 4; i < argc; i++) {
            if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            }
        }

        const char * packageName = NULL;

        PPKGTargetPlatform targetPlatform;

        int ret = ppkg_inspect_package(argv[2], targetPlatformSpec, &packageName, &targetPlatform);

        if (ret != PPKG_OK) {
            return ret;
        }

        ret = ppkg_installed_info(packageName, &targetPlatform, argv[3]);

        if (ret == PPKG_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s info-installed <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not given.\n", argv[0]);
        } else if (ret == PPKG_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s info-installed <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is empty string.\n", argv[0]);
        } else if (ret == PPKG_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s info-installed <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not match pattern %s\n", argv[0], PPKG_PACKAGE_NAME_PATTERN);
        } else if (ret == PPKG_ERROR_ARG_IS_UNKNOWN) {
            fprintf(stderr, "Usage: %s info-installed <PACKAGE-NAME> [KEY], unrecognized KEY: %s\n", argv[0], argv[3]);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package '%s' is not available for target '%s'\n", argv[2], targetPlatform.name);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package '%s' is not installed.\n", argv[2]);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "depends") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s depends <PACKAGE-NAME>, <PACKAGE-NAME> is unspecified.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        if (argv[2][0] == '\0') {
            fprintf(stderr, "Usage: %s depends <PACKAGE-NAME>, <PACKAGE-NAME> should be a non-empty string.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_EMPTY;
        }

        PPKGDependsOutputType outputType = PPKGDependsOutputType_BOX;

        const char * targetPlatformName = NULL;

        char * outputPath = NULL;

        bool verbose = false;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strcmp(argv[i], "-p") == 0) {
                targetPlatformName = argv[++i];

                if (targetPlatformName == NULL) {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                }

                if (targetPlatformName[0] == '\0') {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }

                const char * supportedTargetPlatformNames[6] = { "linux", "macos", "freebsd", "openbsd", "netbsd", "dragonflybsd" };

                bool isSupported = false;

                for (int j = 0; j < 6; j++) {
                    if (strcmp(targetPlatformName, supportedTargetPlatformNames[j]) == 0) {
                        isSupported = true;
                        break;
                    }
                }

                if (!isSupported) {
                    LOG_ERROR2("unsupported target platform name: ", targetPlatformName);
                    return PPKG_ERROR_ARG_IS_INVALID;
                }
            } else if (strcmp(argv[i], "-t") == 0) {
                const char * type = argv[i + 1];

                if (type == NULL) {
                    fprintf(stderr, "-t <OUTPUT-TYPE>, <OUTPUT-TYPE> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                }

                if (type[0] == '\0') {
                    fprintf(stderr, "-o <OUTPUT-TYPE>, <OUTPUT-TYPE> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }

                if (strcmp(type, "dot") == 0) {
                    outputType = PPKGDependsOutputType_DOT;
                    i++;
                } else if (strcmp(type, "box") == 0) {
                    outputType = PPKGDependsOutputType_BOX;
                    i++;
                } else if (strcmp(type, "svg") == 0) {
                    outputType = PPKGDependsOutputType_SVG;
                    i++;
                } else if (strcmp(type, "png") == 0) {
                    outputType = PPKGDependsOutputType_PNG;
                    i++;
                } else {
                    LOG_ERROR2("unsupported type: ", type);
                    return PPKG_ERROR_ARG_IS_INVALID;
                }
             } else if (strcmp(argv[i], "-o") == 0) {
                outputPath = argv[i + 1];

                if (outputPath == NULL) {
                    fprintf(stderr, "-o <OUTPUT-PATH>, <OUTPUT-PATH> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                }

                if (outputPath[0] == '\0') {
                    fprintf(stderr, "-o <OUTPUT-PATH>, <OUTPUT-PATH> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }

                i++;
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            }
        }

        int ret = ppkg_depends(argv[2], targetPlatformName, outputType, outputPath);

        if (ret == PPKG_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s depends <PACKAGE-NAME>, <PACKAGE-NAME> does not match pattern %s\n", argv[0], PPKG_PACKAGE_NAME_PATTERN);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package '%s' is not available for target '%s'\n", argv[2], targetPlatformName);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }


    if (strcmp(argv[1], "fetch") == 0) {
        const char * targetPlatformName = NULL;

        bool verbose = false;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strcmp(argv[i], "-p") == 0) {
                targetPlatformName = argv[++i];

                if (targetPlatformName == NULL) {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                }

                if (targetPlatformName[0] == '\0') {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }

                const char * supportedTargetPlatformNames[6] = { "linux", "macos", "freebsd", "openbsd", "netbsd", "dragonflybsd" };

                bool isSupported = false;

                for (int j = 0; j < 6; j++) {
                    if (strcmp(targetPlatformName, supportedTargetPlatformNames[j]) == 0) {
                        isSupported = true;
                        break;
                    }
                }

                if (!isSupported) {
                    LOG_ERROR2("unsupported target platform name: ", targetPlatformName);
                    return PPKG_ERROR_ARG_IS_INVALID;
                }
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            }
        }

        int ret = ppkg_fetch(argv[2], targetPlatformName, verbose);

        if (ret == PPKG_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0], argv[1]);
        } else if (ret == PPKG_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0], argv[1]);
        } else if (ret == PPKG_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], PPKG_PACKAGE_NAME_PATTERN);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package '%s' is not available for target '%s'\n", argv[2], targetPlatformName);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "install") == 0) {
        PPKGInstallOptions installOptions = {0};

        installOptions.logLevel = PPKGLogLevel_normal;

        int packageIndexArray[argc];
        int packageIndexArraySize = 0;

        char * targetPlatformSpec = NULL;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-x") == 0) {
                installOptions.xtrace = true;
            } else if (strcmp(argv[i], "-q") == 0) {
                installOptions.logLevel = PPKGLogLevel_silent;
            } else if (strcmp(argv[i], "-v") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
            } else if (strcmp(argv[i], "-vv") == 0) {
                installOptions.logLevel = PPKGLogLevel_very_verbose;
                installOptions.verbose_net = true;
                installOptions.verbose_env = true;
                installOptions.verbose_cc  = true;
                installOptions.verbose_ld  = true;
            } else if (strcmp(argv[i], "-v:net") == 0) {
                installOptions.logLevel = PPKGLogLevel_normal;
                installOptions.verbose_net = true;
            } else if (strcmp(argv[i], "-v:env") == 0) {
                installOptions.logLevel = PPKGLogLevel_normal;
                installOptions.verbose_env = true;
            } else if (strcmp(argv[i], "-v:cc") == 0) {
                installOptions.logLevel = PPKGLogLevel_normal;
                installOptions.verbose_cc = true;
            } else if (strcmp(argv[i], "-v:ld") == 0) {
                installOptions.logLevel = PPKGLogLevel_normal;
                installOptions.verbose_ld = true;
            } else if (strcmp(argv[i], "--dry-run") == 0) {
                installOptions.dryrun = true;
            } else if (strcmp(argv[i], "--keep-session-dir") == 0) {
                installOptions.keepSessionDIR = true;
            } else if (strcmp(argv[i], "--export-compile-commands-json") == 0) {
                installOptions.exportCompileCommandsJson = true;
            } else if (strcmp(argv[i], "--enable-ccache") == 0) {
                installOptions.enableCcache = true;
            } else if (strcmp(argv[i], "--enable-bear") == 0) {
                installOptions.enableBear = true;
            } else if (strcmp(argv[i], "--build-type=debug") == 0) {
                installOptions.buildType = PPKGBuildType_debug;
            } else if (strcmp(argv[i], "--build-type=release") == 0) {
                installOptions.buildType = PPKGBuildType_release;
            } else if (strcmp(argv[i], "--link-type=static-full") == 0) {
                installOptions.linkType = PPKGLinkType_static_full;
            } else if (strcmp(argv[i], "--link-type=shared-full") == 0) {
                installOptions.linkType = PPKGLinkType_shared_full;
            } else if (strcmp(argv[i], "--link-type=static-most") == 0) {
                installOptions.linkType = PPKGLinkType_static_most;
            } else if (strcmp(argv[i], "--link-type=shared-most") == 0) {
                installOptions.linkType = PPKGLinkType_shared_most;
            } else if (strncmp(argv[i], "--jobs=", 7) == 0) {
                char * jobsStr = &argv[i][7];

                if (jobsStr[0] == '\0') {
                    LOG_ERROR1("--jobs=<N> , <N> should be a non-empty string");
                    return PPKG_ERROR;
                } else {
                    int j = 0;

                    for (;;) {
                        char c = jobsStr[j];

                        if (c == '\0') {
                            break;
                        }

                        if ((c >= '0') && (c <= '9')) {
                            j++;
                        } else {
                            LOG_ERROR1("--jobs=<N> , <N> should be a integer.");
                            return PPKG_ERROR;
                        }
                    }
                }

                installOptions.parallelJobsCount = atoi(jobsStr);
            } else if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            } else if (argv[i][0] == '-') {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            } else {
                packageIndexArray[packageIndexArraySize] = i;
                packageIndexArraySize++;
            }
        }

        if (packageIndexArraySize == 0) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>..., <PACKAGE-NAME> is not given.\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        for (int i = 0; i < packageIndexArraySize; i++) {
            const char * package = argv[packageIndexArray[i]];

            const char * packageName = NULL;

            PPKGTargetPlatform targetPlatform;

            int ret = ppkg_inspect_package(package, targetPlatformSpec, &packageName, &targetPlatform);

            if (ret != PPKG_OK) {
                return ret;
            }

            ret = ppkg_install(packageName, &targetPlatform, &installOptions);

            if (ret == PPKG_ERROR_ARG_IS_INVALID) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], PPKG_PACKAGE_NAME_PATTERN);
            } else if (ret == PPKG_ERROR_PACKAGE_NOT_AVAILABLE) {
                fprintf(stderr, "package '%s' is not available.\n", packageName);
            } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
                fprintf(stderr, "package '%s' is not installed.\n", packageName);
            } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (ret == PPKG_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (ret != PPKG_OK) {
                return ret;
            }
        }

        return PPKG_OK;
    }

    if (strcmp(argv[1], "reinstall") == 0) {
        PPKGInstallOptions installOptions = {0};

        installOptions.logLevel = PPKGLogLevel_normal;

        int packageIndexArray[argc];
        int packageIndexArraySize = 0;

        char * targetPlatformSpec = NULL;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-q") == 0) {
                installOptions.logLevel = PPKGLogLevel_silent;
            } else if (strcmp(argv[i], "-v") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
            } else if (strcmp(argv[i], "-vv") == 0) {
                installOptions.logLevel = PPKGLogLevel_very_verbose;
                installOptions.verbose_net = true;
                installOptions.verbose_env = true;
                installOptions.verbose_cc  = true;
                installOptions.verbose_ld  = true;
            } else if (strcmp(argv[i], "-v:net") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
                installOptions.verbose_net = true;
            } else if (strcmp(argv[i], "-v:env") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
                installOptions.verbose_env = true;
            } else if (strcmp(argv[i], "-v:cc") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
                installOptions.verbose_cc = true;
            } else if (strcmp(argv[i], "-v:ld") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
                installOptions.verbose_ld = true;
            } else if (strcmp(argv[i], "--dry-run") == 0) {
                installOptions.dryrun = true;
            } else if (strcmp(argv[i], "--keep-session-dir") == 0) {
                installOptions.keepSessionDIR = true;
            } else if (strcmp(argv[i], "--export-compile-commands-json") == 0) {
                installOptions.exportCompileCommandsJson = true;
            } else if (strcmp(argv[i], "--enable-ccache") == 0) {
                installOptions.enableCcache = true;
            } else if (strcmp(argv[i], "--enable-bear") == 0) {
                installOptions.enableBear = true;
            } else if (strcmp(argv[i], "--build-type=debug") == 0) {
                installOptions.buildType = PPKGBuildType_debug;
            } else if (strcmp(argv[i], "--build-type=release") == 0) {
                installOptions.buildType = PPKGBuildType_release;
            } else if (strcmp(argv[i], "--link-type=static-full") == 0) {
                installOptions.linkType = PPKGLinkType_static_full;
            } else if (strcmp(argv[i], "--link-type=shared-full") == 0) {
                installOptions.linkType = PPKGLinkType_shared_full;
            } else if (strcmp(argv[i], "--link-type=static-most") == 0) {
                installOptions.linkType = PPKGLinkType_static_most;
            } else if (strcmp(argv[i], "--link-type=shared-most") == 0) {
                installOptions.linkType = PPKGLinkType_shared_most;
            } else if (strncmp(argv[i], "--jobs=", 7) == 0) {
                char * jobsStr = &argv[i][7];

                if (jobsStr[0] == '\0') {
                    LOG_ERROR1("--jobs=<N> , <N> should be a non-empty string");
                    return PPKG_ERROR;
                } else {
                    int j = 0;

                    for (;;) {
                        char c = jobsStr[j];

                        if (c == '\0') {
                            break;
                        }

                        if ((c >= '0') && (c <= '9')) {
                            j++;
                        } else {
                            LOG_ERROR1("--jobs=<N> , <N> should be a integer.");
                            return PPKG_ERROR;
                        }
                    }
                }

                installOptions.parallelJobsCount = atoi(jobsStr);
            } else if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            } else if (argv[i][0] == '-') {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            } else {
                packageIndexArray[packageIndexArraySize] = i;
                packageIndexArraySize++;
            }
        }

        if (packageIndexArraySize == 0) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>..., <PACKAGE-NAME> is not given.\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        for (int i = 0; i < packageIndexArraySize; i++) {
            const char * package = argv[packageIndexArray[i]];

            const char * packageName = NULL;

            PPKGTargetPlatform targetPlatform;

            int ret = ppkg_inspect_package(package, targetPlatformSpec, &packageName, &targetPlatform);

            if (ret != PPKG_OK) {
                return ret;
            }

            ret = ppkg_reinstall(packageName, &targetPlatform, &installOptions);

            if (ret == PPKG_ERROR_ARG_IS_INVALID) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], PPKG_PACKAGE_NAME_PATTERN);
            } else if (ret == PPKG_ERROR_PACKAGE_NOT_AVAILABLE) {
                fprintf(stderr, "package '%s' is not available.\n", packageName);
            } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
                fprintf(stderr, "package '%s' is not installed.\n", packageName);
            } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (ret == PPKG_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (ret != PPKG_OK) {
                return ret;
            }
        }

        return PPKG_OK;
    }

    if (strcmp(argv[1], "upgrade") == 0) {
        PPKGInstallOptions installOptions = {0};

        installOptions.logLevel = PPKGLogLevel_normal;

        int packageIndexArray[argc];
        int packageIndexArraySize = 0;

        char * targetPlatformSpec = NULL;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-q") == 0) {
                installOptions.logLevel = PPKGLogLevel_silent;
            } else if (strcmp(argv[i], "-v") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
            } else if (strcmp(argv[i], "-vv") == 0) {
                installOptions.logLevel = PPKGLogLevel_very_verbose;
                installOptions.verbose_net = true;
                installOptions.verbose_env = true;
                installOptions.verbose_cc  = true;
                installOptions.verbose_ld  = true;
            } else if (strcmp(argv[i], "-v:net") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
                installOptions.verbose_net = true;
            } else if (strcmp(argv[i], "-v:env") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
                installOptions.verbose_env = true;
            } else if (strcmp(argv[i], "-v:cc") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
                installOptions.verbose_cc = true;
            } else if (strcmp(argv[i], "-v:ld") == 0) {
                installOptions.logLevel = PPKGLogLevel_verbose;
                installOptions.verbose_ld = true;
            } else if (strcmp(argv[i], "--dry-run") == 0) {
                installOptions.dryrun = true;
            } else if (strcmp(argv[i], "--keep-session-dir") == 0) {
                installOptions.keepSessionDIR = true;
            } else if (strcmp(argv[i], "--export-compile-commands-json") == 0) {
                installOptions.exportCompileCommandsJson = true;
            } else if (strcmp(argv[i], "--enable-ccache") == 0) {
                installOptions.enableCcache = true;
            } else if (strcmp(argv[i], "--enable-bear") == 0) {
                installOptions.enableBear = true;
            } else if (strcmp(argv[i], "--build-type=debug") == 0) {
                installOptions.buildType = PPKGBuildType_debug;
            } else if (strcmp(argv[i], "--build-type=release") == 0) {
                installOptions.buildType = PPKGBuildType_release;
            } else if (strcmp(argv[i], "--link-type=static-full") == 0) {
                installOptions.linkType = PPKGLinkType_static_full;
            } else if (strcmp(argv[i], "--link-type=shared-full") == 0) {
                installOptions.linkType = PPKGLinkType_shared_full;
            } else if (strcmp(argv[i], "--link-type=static-most") == 0) {
                installOptions.linkType = PPKGLinkType_static_most;
            } else if (strcmp(argv[i], "--link-type=shared-most") == 0) {
                installOptions.linkType = PPKGLinkType_shared_most;
            } else if (strncmp(argv[i], "--jobs=", 7) == 0) {
                char * jobsStr = &argv[i][7];

                if (jobsStr[0] == '\0') {
                    LOG_ERROR1("--jobs=<N> , <N> should be a non-empty string");
                    return PPKG_ERROR;
                } else {
                    int j = 0;

                    for (;;) {
                        char c = jobsStr[j];

                        if (c == '\0') {
                            break;
                        }

                        if ((c >= '0') && (c <= '9')) {
                            j++;
                        } else {
                            LOG_ERROR1("--jobs=<N> , <N> should be a integer.");
                            return PPKG_ERROR;
                        }
                    }
                }

                installOptions.parallelJobsCount = atoi(jobsStr);
            } else if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            } else if (argv[i][0] == '-') {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            } else {
                packageIndexArray[packageIndexArraySize] = i;
                packageIndexArraySize++;
            }
        }

        for (int i = 0; i < packageIndexArraySize; i++) {
            const char * package = argv[packageIndexArray[i]];

            const char * packageName = NULL;

            PPKGTargetPlatform targetPlatform;

            int ret = ppkg_inspect_package(package, targetPlatformSpec, &packageName, &targetPlatform);

            if (ret != PPKG_OK) {
                return ret;
            }

            ret = ppkg_upgrade(packageName, &targetPlatform, &installOptions);

            if (ret == PPKG_ERROR_ARG_IS_INVALID) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], PPKG_PACKAGE_NAME_PATTERN);
            } else if (ret == PPKG_ERROR_PACKAGE_NOT_AVAILABLE) {
                fprintf(stderr, "package '%s' is not available.\n", packageName);
            } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
                fprintf(stderr, "package '%s' is not installed.\n", packageName);
            } else if (ret == PPKG_ERROR_PACKAGE_NOT_OUTDATED) {
                fprintf(stderr, "package '%s' is not outdated.\n", packageName);
            } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (ret == PPKG_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (ret != PPKG_OK) {
                return ret;
            }
        }

        return PPKG_OK;
    }

    if (strcmp(argv[1], "uninstall") == 0) {
        int packageIndexArray[argc];
        int packageIndexArraySize = 0;

        bool verbose = false;

        char * targetPlatformSpec = NULL;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            } else {
                packageIndexArray[packageIndexArraySize] = i;
                packageIndexArraySize++;
            }
        }

        if (packageIndexArraySize == 0) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>..., <PACKAGE-NAME> is not given.\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        for (int i = 0; i < packageIndexArraySize; i++) {
            const char * package = argv[packageIndexArray[i]];

            const char * packageName = NULL;

            PPKGTargetPlatform targetPlatform;

            int ret = ppkg_inspect_package(package, targetPlatformSpec, &packageName, &targetPlatform);

            if (ret != PPKG_OK) {
                return ret;
            }

            ret = ppkg_uninstall(packageName, &targetPlatform, verbose);

            if (ret == PPKG_ERROR_ARG_IS_INVALID) {
                fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], PPKG_PACKAGE_NAME_PATTERN);
            } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
                fprintf(stderr, "package '%s' is not installed.\n", packageName);
            } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
                fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
            } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
                fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
            } else if (ret == PPKG_ERROR) {
                fprintf(stderr, "occurs error.\n");
            }

            if (ret != PPKG_OK) {
                return ret;
            }
        }

        return PPKG_OK;
    }

    if (strcmp(argv[1], "ls-available") == 0) {
        const char * targetPlatformName = NULL;

        bool verbose = false;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strcmp(argv[i], "-p") == 0) {
                targetPlatformName = argv[++i];

                if (targetPlatformName == NULL) {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                }

                if (targetPlatformName[0] == '\0') {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }

                const char * supportedTargetPlatformNames[6] = { "linux", "macos", "freebsd", "openbsd", "netbsd", "dragonflybsd" };

                bool isSupported = false;

                for (int j = 0; j < 6; j++) {
                    if (strcmp(targetPlatformName, supportedTargetPlatformNames[j]) == 0) {
                        isSupported = true;
                        break;
                    }
                }

                if (!isSupported) {
                    LOG_ERROR2("unsupported target platform name: ", targetPlatformName);
                    return PPKG_ERROR_ARG_IS_INVALID;
                }
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            }
        }

        int ret = ppkg_show_the_available_packages(targetPlatformName, verbose);

        if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "ls-installed") == 0) {
        char * targetPlatformSpec = NULL;

        bool verbose = false;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            }
        }

        int ret;

        if (targetPlatformSpec == NULL) {
            ret = ppkg_list_the_installed_packages(NULL, verbose);
        } else {
            PPKGTargetPlatform targetPlatform;

            ret = ppkg_inspect_target_platform_spec(targetPlatformSpec, &targetPlatform);

            if (ret != PPKG_OK) {
                return ret;
            }

            ret = ppkg_list_the_installed_packages(&targetPlatform, verbose);
        }

        if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "ls-outdated") == 0) {
        char * targetPlatformSpec = NULL;

        bool verbose = false;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            }
        }

        int ret;

        if (targetPlatformSpec == NULL) {
            ret = ppkg_list_the__outdated_packages(NULL, verbose);
        } else {
            PPKGTargetPlatform targetPlatform;

            ret = ppkg_inspect_target_platform_spec(targetPlatformSpec, &targetPlatform);

            if (ret != PPKG_OK) {
                return ret;
            }

            ret = ppkg_list_the__outdated_packages(&targetPlatform, verbose);
        }

        if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "is-available") == 0) {
        const char * targetPlatformName = NULL;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-p") == 0) {
                targetPlatformName = argv[++i];

                if (targetPlatformName == NULL) {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                }

                if (targetPlatformName[0] == '\0') {
                    fprintf(stderr, "-p <TARGET-PLATFORM-NAME>, <TARGET-PLATFORM-NAME> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }

                const char * supportedTargetPlatformNames[6] = { "linux", "macos", "freebsd", "openbsd", "netbsd", "dragonflybsd" };

                bool isSupported = false;

                for (int j = 0; j < 6; j++) {
                    if (strcmp(targetPlatformName, supportedTargetPlatformNames[j]) == 0) {
                        isSupported = true;
                        break;
                    }
                }

                if (!isSupported) {
                    LOG_ERROR2("unsupported target platform name: ", targetPlatformName);
                    return PPKG_ERROR_ARG_IS_INVALID;
                }
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            }
        }

        int ret = ppkg_check_if_the_given_package_is_available(argv[2], targetPlatformName);

        if (ret == PPKG_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not given.\n", argv[0], argv[1]);
        } else if (ret == PPKG_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is empty string.\n", argv[0], argv[1]);
        } else if (ret == PPKG_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], PPKG_PACKAGE_NAME_PATTERN);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package '%s' is not available for target '%s'\n", argv[2], targetPlatformName);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "is-installed") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s is-installed <PACKAGE-SPEC>, <PACKAGE-SPEC> is unspecified.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_NULL;
        } else if (argv[2][0] == '\0') {
            fprintf(stderr, "Usage: %s is-installed <PACKAGE-SPEC>, <PACKAGE-SPEC> must be a non-empty string.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_EMPTY;
        }

        char * targetPlatformSpec = NULL;

        for (int i = 4; i < argc; i++) {
            if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            }
        }

        const char * packageName = NULL;

        PPKGTargetPlatform targetPlatform;

        int ret = ppkg_inspect_package(argv[2], targetPlatformSpec, &packageName, &targetPlatform);

        if (ret != PPKG_OK) {
            return ret;
        }

        ret = ppkg_check_if_the_given_package_is_installed(packageName, &targetPlatform);

        if (ret == PPKG_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], PPKG_PACKAGE_NAME_PATTERN);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package '%s' is not installed.\n", argv[2]);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "is-outdated") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s is-installed <PACKAGE-SPEC>, <PACKAGE-SPEC> is unspecified.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_NULL;
        } else if (argv[2][0] == '\0') {
            fprintf(stderr, "Usage: %s is-installed <PACKAGE-SPEC>, <PACKAGE-SPEC> must be a non-empty string.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_EMPTY;
        }

        char * targetPlatformSpec = NULL;

        for (int i = 4; i < argc; i++) {
            if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            }
        }

        const char * packageName = NULL;

        PPKGTargetPlatform targetPlatform;

        int ret = ppkg_inspect_package(argv[2], targetPlatformSpec, &packageName, &targetPlatform);

        if (ret != PPKG_OK) {
            return ret;
        }

        ret = ppkg_check_if_the_given_package_is_outdated(packageName, &targetPlatform);

        if (ret == PPKG_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], PPKG_PACKAGE_NAME_PATTERN);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_AVAILABLE) {
            fprintf(stderr, "package [%s] is not available.\n", argv[2]);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package [%s] is not installed.\n", argv[2]);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "formula-repo-list") == 0) {
        return ppkg_formula_repo_list_printf();
    }

    if (strcmp(argv[1], "formula-repo-add") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        if (argv[3] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        int pinned  = 0;
        int enabled = 1;

        const char * branch = NULL;

        for (int i = 4; i < argc; i++) {
            if (strcmp(argv[i], "--pin") == 0) {
                pinned = 1;
            } else if (strcmp(argv[i], "--unpin") == 0) {
                pinned = 0;
            } else if (strcmp(argv[i], "--enable") == 0) {
                enabled = 1;
            } else if (strcmp(argv[i], "--disable") == 0) {
                enabled = 0;
            } else if (strncmp(argv[i], "--branch=", 9) == 0) {
                if (argv[i][9] == '\0') {
                    fprintf(stderr, "--branch=<VALUE>, <VALUE> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                } else {
                    branch = &argv[i][9];
                }
            } else {
                fprintf(stderr, "unrecognized option: %s\n", argv[i]);
                return PPKG_ERROR_ARG_IS_UNKNOWN;
            }
        }

        return ppkg_formula_repo_add(argv[2], argv[3], branch, pinned, enabled);
    }

    if (strcmp(argv[1], "formula-repo-init") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        if (argv[3] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> <FORMULA-REPO-URL> [--branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        int pinned  = 1;
        int enabled = 1;

        char * branch = NULL;

        for (int i = 4; i < argc; i++) {
            if (strcmp(argv[i], "--pin") == 0) {
                pinned = 1;
            } else if (strcmp(argv[i], "--unpin") == 0) {
                pinned = 0;
            } else if (strcmp(argv[i], "--enable") == 0) {
                enabled = 1;
            } else if (strcmp(argv[i], "--disable") == 0) {
                enabled = 0;
            } else if (strncmp(argv[i], "--branch=", 9) == 0) {
                if (argv[i][9] == '\0') {
                    fprintf(stderr, "--branch=<VALUE>, <VALUE> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                } else {
                    branch = &argv[i][9];
                }
            } else {
                fprintf(stderr, "unrecognized option: %s\n", argv[i]);
                return PPKG_ERROR_ARG_IS_UNKNOWN;
            }
        }

        return ppkg_formula_repo_create(argv[2], argv[3], branch, pinned, enabled);
    }

    if (strcmp(argv[1], "formula-repo-del") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME>\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        return ppkg_formula_repo_remove(argv[2]);
    }

    if (strcmp(argv[1], "formula-repo-sync") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME>\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        return ppkg_formula_repo_sync_(argv[2]);
    }

    if (strcmp(argv[1], "formula-repo-conf") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> [--url=VALUE --branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        if (argv[3] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME> [--url=VALUE --branch=VALUE --pin/--unpin --enable/--disable]\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        int pinned  = -1;
        int enabled = -1;

        const char * branch = NULL;
        const char * url = NULL;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "--pin") == 0) {
                pinned = 1;
            } else if (strcmp(argv[i], "--unpin") == 0) {
                pinned = 0;
            } else if (strcmp(argv[i], "--enable") == 0) {
                enabled = 1;
            } else if (strcmp(argv[i], "--disable") == 0) {
                enabled = 0;
            } else if (strncmp(argv[i], "--url=", 6) == 0) {
                if (argv[i][6] == '\0') {
                    fprintf(stderr, "--url=<VALUE>, <VALUE> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                } else {
                    url = &argv[i][6];
                }
            } else if (strncmp(argv[i], "--branch=", 9) == 0) {
                if (argv[i][9] == '\0') {
                    fprintf(stderr, "--branch=<VALUE>, <VALUE> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                } else {
                    branch = &argv[i][9];
                }
            } else {
                fprintf(stderr, "unrecognized option: %s\n", argv[i]);
                return PPKG_ERROR_ARG_IS_UNKNOWN;
            }
        }

        return ppkg_formula_repo_config(argv[2], url, branch, pinned, enabled);
    }

    if (strcmp(argv[1], "formula-repo-info") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s %s <FORMULA-REPO-NAME>\n", argv[0], argv[1]);
            return PPKG_ERROR_ARG_IS_NULL;
        }

        return ppkg_formula_repo_info_(argv[2]);
    }

    if (strcmp(argv[1], "tree") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s tree <PACKAGE-SPEC> [KEY], <PACKAGE-SPEC> is unspecified.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_NULL;
        } else if (argv[2][0] == '\0') {
            fprintf(stderr, "Usage: %s tree <PACKAGE-SPEC> [KEY], <PACKAGE-SPEC> must be a non-empty string.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_EMPTY;
        }

        char * targetPlatformSpec = NULL;

        for (int i = 4; i < argc; i++) {
            if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            }
        }

        const char * packageName = NULL;

        PPKGTargetPlatform targetPlatform;

        int ret = ppkg_inspect_package(argv[2], targetPlatformSpec, &packageName, &targetPlatform);

        if (ret != PPKG_OK) {
            return ret;
        }

        ret = ppkg_tree(packageName, &targetPlatform, argc - 3, &argv[3]);

        if (ret == PPKG_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s tree <PACKAGE-NAME> [KEY], <PACKAGE-NAME> is not match pattern %s\n", argv[0], PPKG_PACKAGE_NAME_PATTERN);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package '%s' is not installed.\n", packageName);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "logs") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s logs <PACKAGE-SPEC>, <PACKAGE-SPEC> is unspecified.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_NULL;
        } else if (argv[2][0] == '\0') {
            fprintf(stderr, "Usage: %s logs <PACKAGE-SPEC>, <PACKAGE-SPEC> must be a non-empty string.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_EMPTY;
        }

        char * targetPlatformSpec = NULL;

        for (int i = 4; i < argc; i++) {
            if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            }
        }

        const char * packageName = NULL;

        PPKGTargetPlatform targetPlatform;

        int ret = ppkg_inspect_package(argv[2], targetPlatformSpec, &packageName, &targetPlatform);

        if (ret != PPKG_OK) {
            return ret;
        }

        ret = ppkg_logs(packageName, &targetPlatform);

        if (ret == PPKG_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME>, <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], PPKG_PACKAGE_NAME_PATTERN);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package '%s' is not installed.\n", argv[2]);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "pack") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s pack <PACKAGE-SPEC>, <PACKAGE-SPEC> is unspecified.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_NULL;
        } else if (argv[2][0] == '\0') {
            fprintf(stderr, "Usage: %s pack <PACKAGE-SPEC>, <PACKAGE-SPEC> must be a non-empty string.\n", argv[0]);
            return PPKG_ERROR_ARG_IS_EMPTY;
        }

        ArchiveType outputType = ArchiveType_tar_xz;

        char * targetPlatformSpec = NULL;

        char * outputPath = NULL;

        bool verbose = false;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else if (strcmp(argv[i], "-t") == 0) {
                const char * type = argv[i + 1];

                if (type == NULL) {
                    fprintf(stderr, "-t <OUTPUT-TYPE>, <OUTPUT-TYPE> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                }

                if (type[0] == '\0') {
                    fprintf(stderr, "-o <OUTPUT-TYPE>, <OUTPUT-TYPE> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }

                if (strcmp(type, "zip") == 0) {
                    outputType = ArchiveType_zip;
                    i++;
                } else if (strcmp(type, "tar.gz") == 0) {
                    outputType = ArchiveType_tar_gz;
                    i++;
                } else if (strcmp(type, "tar.lz") == 0) {
                    outputType = ArchiveType_tar_lz;
                    i++;
                } else if (strcmp(type, "tar.xz") == 0) {
                    outputType = ArchiveType_tar_xz;
                    i++;
                } else if (strcmp(type, "tar.bz2") == 0) {
                    outputType = ArchiveType_tar_bz2;
                    i++;
                } else {
                    LOG_ERROR2("unsupported type: ", type);
                    return PPKG_ERROR_ARG_IS_INVALID;
                }
             } else if (strcmp(argv[i], "-o") == 0) {
                outputPath = argv[i + 1];

                if (outputPath == NULL) {
                    fprintf(stderr, "-o <OUTPUT-PATH>, <OUTPUT-PATH> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_INVALID;
                }

                if (outputPath[0] == '\0') {
                    fprintf(stderr, "-o <OUTPUT-PATH>, <OUTPUT-PATH> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }

                i++;
            } else if (strncmp(argv[i], "--target=", 9) == 0) {
                targetPlatformSpec = &argv[i][9];

                if (targetPlatformSpec[0] == '\0') {
                    fprintf(stderr, "--target=<TARGET-PLATFORM-SPEC>, <TARGET-PLATFORM-SPEC> should be a non-empty string.\n");
                    return PPKG_ERROR_ARG_IS_EMPTY;
                }
            } else {
                LOG_ERROR2("unrecognized argument: ", argv[i]);
                return PPKG_ERROR_ARG_IS_INVALID;
            }
        }

        const char * packageName = NULL;

        PPKGTargetPlatform targetPlatform;

        int ret = ppkg_inspect_package(argv[2], targetPlatformSpec, &packageName, &targetPlatform);

        if (ret != PPKG_OK) {
            return ret;
        }

        ret = ppkg_pack(packageName, &targetPlatform, outputType, outputPath, verbose);

        if (ret == PPKG_ERROR_ARG_IS_NULL) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME> [-t tar.gz|tar.xz|tar.bz2|zip], <PACKAGE-NAME> is not given.\n", argv[0], argv[1]);
        } else if (ret == PPKG_ERROR_ARG_IS_EMPTY) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME> [-t tar.gz|tar.xz|tar.bz2|zip], <PACKAGE-NAME> is empty string.\n", argv[0], argv[1]);
        } else if (ret == PPKG_ERROR_ARG_IS_INVALID) {
            fprintf(stderr, "Usage: %s %s <PACKAGE-NAME> [-t tar.gz|tar.xz|tar.bz2|zip], <PACKAGE-NAME> is not match pattern %s\n", argv[0], argv[1], PPKG_PACKAGE_NAME_PATTERN);
        } else if (ret == PPKG_ERROR_PACKAGE_NOT_INSTALLED) {
            fprintf(stderr, "package '%s' is not installed.\n", argv[2]);
        } else if (ret == PPKG_ERROR_ENV_HOME_NOT_SET) {
            fprintf(stderr, "%s\n", "HOME environment variable is not set.\n");
        } else if (ret == PPKG_ERROR_ENV_PATH_NOT_SET) {
            fprintf(stderr, "%s\n", "PATH environment variable is not set.\n");
        } else if (ret == PPKG_ERROR) {
            fprintf(stderr, "occurs error.\n");
        }

        return ret;
    }

    if (strcmp(argv[1], "util") == 0) {
        return ppkg_util(argc, argv);
    }

    LOG_ERROR2("unrecognized action: ", argv[1]);
    return PPKG_ERROR_ARG_IS_UNKNOWN;
}

int main(int argc, char* argv[]) {
    return ppkg_main(argc, argv);
}
