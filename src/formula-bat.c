#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ppkg.h"

int ppkg_formula_bat(const char * packageName) {
    char * formulaFilePath = NULL;

    int ret = ppkg_formula_locate(packageName, &formulaFilePath);

    if (ret != PPKG_OK) {
        return ret;
    }

    execlp("bat", "bat", "--paging=never", formulaFilePath, NULL);

    perror("bat");

    free(formulaFilePath);

    return PPKG_ERROR;
}
