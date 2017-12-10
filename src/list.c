/* Copyright (c) 2010-2018 the corto developers
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

    CORTO_UNUSED(argc);

    corto_iter it;
    corto_int32 count = 0;
    if (corto_select("//").type("/corto/vstore/package").iter(&it)) {
        goto error;
    }
    while (corto_iter_hasNext(&it)) {
        corto_result *r = corto_iter_next(&it);
        corto_id id;
        char *lib;
        sprintf(id, "%s/%s", r->parent, r->id);
        if ((lib = corto_locate(id, NULL, CORTO_LOCATION_LIB))) {
            corto_log("#[bold]%s #[grey]=>#[normal] #[grey]%s\n",
                id, lib);
        } else {
            corto_raise(); /* Raise exception if it was thrown */
        }
        count ++;
        corto_dealloc(lib);
    }

    corto_log("\n#[white]Total: %d packages\n",
        count);

    return 0;
error:
    corto_raise();
    return -1;
}
