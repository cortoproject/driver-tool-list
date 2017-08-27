/* Copyright (c) 2010-2017 the corto developers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <corto/corto.h>
#include <corto/argparse/argparse.h>
#include <driver/tool/list/list.h>

int cortomain(int argc, char *argv[]) {
    corto_ll locate, project;

    CORTO_UNUSED(argc);

    corto_argdata *data = corto_argparse(
      argv,
      (corto_argdata[]){
        /* Ignore first argument */
        {"$0", NULL, NULL},
        {"--locate", &locate, NULL},

        /* Match at most one project directory */
        {"$?*", &project, NULL},
        {NULL}
      }
    );

    if (project) {
        corto_chdir(corto_ll_get(project, 0));
        corto_ll packages = corto_loadGetPackages();
        if (packages && corto_ll_size(packages)) {
            corto_iter iter = corto_ll_iter(packages);
            while (corto_iter_hasNext(&iter)) {
                corto_string str = corto_iter_next(&iter);
                corto_string package = corto_locate(str, NULL, CORTO_LOCATION_LIB);
                if (package) {
                    printf("  %s%s%s  =>  %s\n", CORTO_CYAN, str, CORTO_NORMAL, package);
                } else {
                  printf("  %s%s%s  =>  %smissing%s\n", CORTO_CYAN, str, CORTO_NORMAL, CORTO_RED, CORTO_NORMAL);
                }
            }
        } else {
            corto_lasterr();
            printf("no packages to list\n");
        }

        if (packages) {
            corto_loadFreePackages(packages);
        }
    } else {
        corto_iter it;
        corto_int32 count = 0, globalCount = 0;
        if (corto_select("//").type("/corto/vstore/package").iter(&it)) {
            goto error;
        }
        while (corto_iter_hasNext(&it)) {
            corto_result *r = corto_iter_next(&it);
            corto_id id; sprintf(id, "%s/%s", r->parent, r->id);
            char *lib = corto_locate(id, NULL, CORTO_LOCATION_LIB);
            if (lib) {
                if (locate) {
                    char *env = corto_locate(id, NULL, CORTO_LOCATION_ENV);
                    corto_bool isGlobal = !strcmp(env, "/usr/local");
                    count ++;
                    if (isGlobal) globalCount ++;
                    printf("%s%s%s  %s=>%s  %s\n", 
                        CORTO_CYAN, id, CORTO_NORMAL, 
                        isGlobal ? CORTO_GREEN : CORTO_MAGENTA, CORTO_NORMAL,
                        lib);
                } else {
                    printf("%s\n", id);
                }
            } else if (corto_lasterr()) {
                printf("%s%s%s%s\n", 
                    CORTO_RED, id, CORTO_NORMAL, corto_lasterr());                
            }
            corto_dealloc(lib);
        }
        if (locate) {
            printf("\nTotal: %d packages (%d %sglobal%s, %d %slocal%s)\n", 
                count, 
                globalCount, CORTO_GREEN, CORTO_NORMAL, 
                count - globalCount, CORTO_MAGENTA, CORTO_NORMAL);
        }
    }

    corto_argclean(data);

    return 0;
error:
    corto_error("%s", corto_lasterr());
    return -1;
}

