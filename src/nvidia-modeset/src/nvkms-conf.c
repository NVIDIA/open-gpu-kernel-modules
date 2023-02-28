/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nvkms-utils.h"
#include "nvkms-dpy-override.h"

static NvBool CharIsSpace(char c)
{
    return (c == ' '  || /* space           */
            c == '\f' || /* form feed       */
            c == '\n' || /* line feed       */
            c == '\r' || /* carriage return */
            c == '\t' || /* horizontal tab  */
            c == '\v');  /* vertical tab    */
}

/* Display Override Conf File Initialization */
static inline NvU8 AsciiHexCharToNibble(char hex)
{
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 0xA;
    } else if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 0xA;
    } else {
        return 0xFF;
    }
}

/* bin should be able to hold at least (size / 2) bytes */
static size_t AsciiHexToBin(char *bin, const char *hex, size_t size)
{
    size_t hi, bi;
    NvBool highNibble = TRUE;
    NvU8 upnib = 0;

    if (hex == NULL || bin == NULL) {
        return 0;
    }
    if (bin >= hex && bin <= hex + size) {
        /*
         * Although, theoretically, decoding ascii hex in place should work
         * fine - each set of 2 characters encodes 1 byte - it's best to avoid
         * the complexity
         */
        nvAssert(!"ASCII hex would be decoded in place.");
        return 0;
    }

    for(hi = 0, bi = 0; hi < size; hi++) {
        NvU8 nibble;

        if (CharIsSpace(hex[hi])) {
            continue;
        }

        nibble = AsciiHexCharToNibble(hex[hi]);
        if (nibble > 0xF) {
            return 0; /* invalid character */
        }

        if (highNibble) {
            upnib = nibble << 4;
        } else {
            bin[bi++] = upnib | nibble;
        }
        highNibble = !highNibble;
    }

    if (!highNibble) {
        return 0; /* odd number of hex chars */
    }

    return bi;
}

#define SUBPARSER(name) static NvBool Subparser_ ## name(   \
    const char *key,                                        \
    const char *value,                                      \
    nvkms_config_read_file_func_t readfile)

SUBPARSER(override)
{
    size_t i;
    size_t idLen = 0;
    size_t valLen = nvkms_strlen(value);

    NvU32 gpuId = NV0000_CTRL_GPU_INVALID_ID;
    const char *name = NULL;

    static nv_gpu_info_t devs[NV_MAX_GPUS];
    NvU32 numdevs = nvkms_enumerate_gpus(devs);

    if (key[0] == '[') {
        while (key[idLen] != 0 && key[idLen] != ']') {
            idLen++;
        }
    }

    while (key[idLen] != 0 && key[idLen] != '.') {
        idLen++;
    }

    /* Get the GPU ID */
    if (key[0] == '[') {
        /* GPU PCI Address */
        NvU32 domain = 0xFFFFFFFF, temp = 0;
        NvU8  bus = 0xFF, slot = 0xFF, function = 0xFF;
        size_t j;

        for (i = 1, j = 0; (key[i] != 0) && (key[i] != ']') && (j < 4); i++) {
            if (key[i] == ':') {
                if (j == 0) {
                    domain = temp;
                } else if (j == 1) {
                    bus = (NvU8)temp;
                } else {
                    break;
                }
                j++;
                temp = 0;
            } else if (key[i] == '.') {
                if (j == 2) {
                    slot = (NvU8)temp;
                } else {
                    break;
                }
                j++;
                temp = 0;
            } else {
                NvU8 nibble = AsciiHexCharToNibble(key[i]);
                if (nibble > 0xF) {
                    break;
                }
                temp <<= 4;
                temp |= nibble;
            }
        }

        if (j == 3) {
            function = (NvU8)temp;
            j++;
        }

        if (j != 4) {
            nvEvoLog(EVO_LOG_WARN, "Syntax error in override entry: "
                     "Error reading PCI Address: %.*s "
                     "(%zu fields read)", (int)idLen, key, j);
            nvClearDpyOverrides();
            return FALSE;
        }

        for (size_t i = 0; i < numdevs; i++) {
            if (devs[i].pci_info.domain   == domain &&
                devs[i].pci_info.bus      == bus    &&
                devs[i].pci_info.slot     == slot   &&
                devs[i].pci_info.function == function) {

                gpuId = devs[i].gpu_id;
                break;
            }
        }
        if (gpuId == NV0000_CTRL_GPU_INVALID_ID) {
            nvEvoLog(EVO_LOG_WARN, "Error in override entry: "
                     "No GPU with PCI Address %04x:%02hhx:%02hhx.%hhx",
                     domain, bus, slot, function);
            nvClearDpyOverrides();
            return FALSE;
        }
    } else if (idLen == 5 && nvkms_memcmp(key, "tegra", 5) == 0){
        gpuId = NVKMS_DEVICE_ID_TEGRA;
    } else {
        nvEvoLog(EVO_LOG_WARN, "Syntax error in override entry: "
                 "Unknown GPU designator: %.*s", (int)idLen, key);
        return FALSE;
    }

    /* Get the dpy name */
    if (key[idLen] != '.' || key[idLen + 1] == 0) {
        nvEvoLog(EVO_LOG_WARN, "Syntax error in override entry: "
                 "Expected '.' followed by display name");
        return FALSE;
    }
    name = key + (idLen + 1);

    /* Get the edid */
    if (value[0] == '"' && value[valLen - 1] == '"') {
        valLen -= 2;
        value += 1;
    }

    if (value[0] == '/') {
        size_t bufflen = 0;
        char *buff = NULL;

        if (readfile != NULL) {
            char *fname = nvCalloc(valLen + 1, 1);
            nvkms_memcpy(fname, value, valLen);
            bufflen = readfile(fname, &buff);
            nvFree(fname);
        }

        if (bufflen == 0) {
            nvEvoLog(EVO_LOG_WARN, "Error in override entry: "
                     "Error opening EDID file: %.*s", (int)valLen, value);
            nvClearDpyOverrides();
            return FALSE;
        }

        nvCreateDpyOverride(gpuId, name, TRUE, buff, bufflen);
        nvkms_free(buff, bufflen);
    } else if (valLen == 12 && nvkms_memcmp(value, "disconnected", 12) == 0) {
        nvCreateDpyOverride(gpuId, name, FALSE, NULL, 0);
    } else {
        char *edidBuf = nvCalloc(valLen / 2, 1);
        size_t decoded = AsciiHexToBin(edidBuf, value, valLen);
        if (decoded == 0) {
            nvEvoLog(EVO_LOG_WARN, "Error in override entry: "
                     "Error decoding ASCII hex: %.*s\n", (int)valLen, value);
            nvFree(edidBuf);
            nvClearDpyOverrides();
            return FALSE;
        }

        nvCreateDpyOverride(gpuId, name, TRUE, edidBuf, decoded);
        nvFree(edidBuf);
    }

    return TRUE;
}

#undef SUBPARSER

static NvBool Subparse(
    const char *keyhead,
    const char *keytail,
    const char *value,
    nvkms_config_read_file_func_t readfile)
{
#define SUBPARSE(name) if (nvkms_strcmp(keyhead, #name) == 0) { \
    return Subparser_ ## name(keytail, value, readfile); \
}
    SUBPARSE(override);
    nvEvoLog(EVO_LOG_WARN, "Error reading configuration file: "
             "Parser not found for key: %s.%s", keyhead, keytail);
    return FALSE;
#undef SUBPARSE
}

#define ST_KEYHEAD 0 /* read head of key */
#define ST_KEYTAIL 1 /* read tail of key */
#define ST_VALUE   2 /* read value of key */
#define ST_EQUALS  3 /* expect '=' */
#define ST_SEND    4 /* send values */

/*
 * This function implements a state machine:
 * KEYHEAD -> KEYTAIL : when a '.' is read
 * KEYHEAD -> EQUALS  : when whitespace is read
 * KEYHEAD -> VALUE   : when '=' is read
 * KEYTAIL -> EQUALS  : when whitespace is read
 * KEYTAIL -> VALUE   : when '=' is read
 * EQUALS  -> VALUE   : when '=' is read
 * VALUE   -> SEND    : when whitespace is read
 * SEND    -> KEYHEAD : consumes no input but sends strings to further parser functions
 *
 * Here, whitespace means any block of one or more whitespace characters, as
 * determined by the CharIsSpace() helper function. Whitespace only causes state
 * transitions if at least one non-whitespace character has been read in that
 * state
 *
 * When a '#' is read, all input is consumed until a '\n' is read or the end of
 * the buffer is reached. This input is treated as whitespace for the purposes
 * of the above state transitions and is considered part of any surrounding
 * whitespace
 *
 * When a '"', '(', '{', '[', or '<' is read while in KEYHEAD, KEYTAIL, or VALUE,
 * a substate is entered which consumes all input until a corresponding '"', ')',
 * '}', ']', or '>' is read. While in this substate, '.', whitespace, '=', and '#'
 * do not cause state transitions.
 *
 * While in KEYHEAD, KEYTAIL, or VALUE, all consumed input which does not cause
 * state transitions are added to the corresponding string to be passed to
 * further parser functions.
 *
 * NOTE: buff is not guaranteed to be NULL-terminated
 */
NvBool nvKmsReadConf(
    const char *buff,
    size_t size,
    nvkms_config_read_file_func_t readfile)
{
    size_t i;
    NvBool ret = FALSE; /* set false until success noted */

    int    state    = ST_KEYHEAD; /* state machine's current state */
    char   watchfor = 0; /* marker for paired char blocks */
    const char *ptr = NULL; /* pointer into buffer */
    char  *strs[3]  = { NULL, NULL, NULL }; /* alloced strings to be copied to */
    size_t lens[3]  = { 0, 0, 0 }; /* lengths of strings in strs */

    /* named pointers into strs */
    char ** const keyhead  = &strs[ST_KEYHEAD];
    char ** const keytail  = &strs[ST_KEYTAIL];
    char ** const value    = &strs[ST_VALUE];

    /* verify that there's always a string allocated in each of the slots */
    for (i = 0; i < 3; ++i) {
        strs[i] = nvCalloc(1, 1);
        if (strs[i] == NULL) {
            nvEvoLog(EVO_LOG_WARN, "Error reading configuration file: "
                     "Out of memory");
            goto teardown;
        }
        lens[i] = 1;
    }

#define COPYPTR() do {                                                          \
    size_t strlen;                                                              \
    nvAssert(ptr != NULL);                                                      \
    nvAssert(state == ST_KEYHEAD || state == ST_KEYTAIL || state == ST_VALUE);  \
    strlen = (buff + i) - ptr;                                                  \
    if (lens[state] < strlen + 1) {                                             \
        /* allocate strlen + 1 to ensure a null terminator */                   \
        nvFree(strs[state]);                                                    \
        strs[state] = nvCalloc(strlen + 1, 1);                                  \
        if (strs[state] == NULL) {                                              \
            nvEvoLog(EVO_LOG_WARN, "Error reading configuration file: "         \
                     "Out of memory");                                          \
            goto teardown;                                                      \
        }                                                                       \
        lens[state] = strlen + 1;                                               \
    }                                                                           \
    nvkms_memcpy(strs[state], ptr, strlen);                                     \
    strs[state][strlen] = 0;                                                    \
    ptr = NULL;                                                                 \
} while (0)

    for (i = 0; i < size; i++) {
        /*
         * If watchfor is set, then either we're in a comment or we're in a
         * paired block. If we're in a comment (watchfor == '\n'), then we
         * want ptr to be NULL, so that the comment isn't included in content.
         * If we're not in a comment (watchfor == '"', ')', '}', ']', or '>'),
         * we want ptr to be non-NULL, so that the paired block is included in
         * the content.
         */
        nvAssert(watchfor == 0 ||
                 (watchfor != '\n' && ptr != NULL) ||
                 (watchfor == '\n' && ptr == NULL));

        if (state == ST_KEYHEAD) {
            if (watchfor == 0) {
                if (buff[i] == '.') {
                    if (ptr == NULL) {
                        nvEvoLog(EVO_LOG_WARN, "Syntax error in configuration file: "
                                 "'.' at start of key");
                        goto teardown;
                    }

                    COPYPTR();
                    state = ST_KEYTAIL;
                } else if (buff[i] == '=') {
                    if (ptr == NULL) {
                        nvEvoLog(EVO_LOG_WARN, "Syntax error in configuration file: "
                                 "key expected before '='");
                        goto teardown;
                    }

                    COPYPTR();
                    state = ST_VALUE;
                } else if (buff[i] == '"') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = '"';
                } else if (buff[i] == '(') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = ')';
                } else if (buff[i] == '{') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = '}';
                } else if (buff[i] == '[') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = ']';
                } else if (buff[i] == '<') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = '>';
                } else if (buff[i] == '#') {
                    if (ptr != NULL) {
                        COPYPTR();
                        state = ST_EQUALS;
                    }
                    watchfor = '\n';
                } else if (CharIsSpace(buff[i])) {
                    if (ptr != NULL) {
                        COPYPTR();
                        state = ST_EQUALS;
                    }
                } else if (ptr == NULL) {
                    ptr = buff + i;
                }
            } else if (buff[i] == watchfor) {
                watchfor = 0;
            }
        } else if (state == ST_KEYTAIL) {
            if (watchfor == 0) {
                if (buff[i] == '=') {
                    if (ptr == NULL) {
                        nvEvoLog(EVO_LOG_WARN, "Syntax error in configuration file: "
                                 "identifier expected after '.' in \"%s\" key",
                                 *keyhead);
                        goto teardown;
                    }

                    COPYPTR();
                    state = ST_VALUE;
                } else if (buff[i] == '"') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = '"';
                } else if (buff[i] == '(') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = ')';
                } else if (buff[i] == '{') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = '}';
                } else if (buff[i] == '[') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = ']';
                } else if (buff[i] == '<') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = '>';
                } else if (buff[i] == '#') {
                    if (ptr != NULL) {
                        COPYPTR();
                        state = ST_EQUALS;
                    }
                    watchfor = '\n';
                } else if (CharIsSpace(buff[i])) {
                    if (ptr != NULL) {
                        COPYPTR();
                        state = ST_EQUALS;
                    }
                } else if (ptr == NULL) {
                    ptr = buff + i;
                }
            } else if (buff[i] == watchfor) {
                watchfor = 0;
            }
        } else if (state == ST_EQUALS) {
            nvAssert(ptr == NULL);
            /* watchfor should only ever be set if we're in a comment */
            nvAssert(watchfor == 0 || watchfor == '\n');

            if (watchfor == 0) {
                if (buff[i] == '=') {
                    state = ST_VALUE;
                } else if (buff[i] == '#') {
                    watchfor = '\n';
                } else if (!CharIsSpace(buff[i])) {
                    nvEvoLog(EVO_LOG_WARN, "Syntax error in configuration file: "
                             "expected '=' before value");
                    goto teardown;
                }
            } else if (buff[i] == watchfor) {
                watchfor = 0;
            }

        } else if (state == ST_VALUE) {
            if (watchfor == 0) {
                if (buff[i] == '=') {
                    nvEvoLog(EVO_LOG_WARN, "Syntax error in configuration file: "
                             "unexpected '=' in value");
                    goto teardown;
                } else if (buff[i] == '"') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = '"';
                } else if (buff[i] == '(') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = ')';
                } else if (buff[i] == '{') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = '}';
                } else if (buff[i] == '[') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = ']';
                } else if (buff[i] == '<') {
                    if (ptr == NULL) {
                        ptr = buff + i;
                    }
                    watchfor = '>';
                } else if (buff[i] == '#') {
                    if (ptr != NULL) {
                        COPYPTR();
                        state = ST_SEND;
                    }
                    watchfor = '\n';
                } else if (CharIsSpace(buff[i])) {
                    if (ptr != NULL) {
                        COPYPTR();
                        state = ST_SEND;
                    }
                } else if (ptr == NULL) {
                    ptr = buff + i;
                }
            } else if (buff[i] == watchfor) {
                watchfor = 0;
            }
        } else if (state == ST_SEND) {
            if (!Subparse(*keyhead, *keytail, *value, readfile)) {
                goto teardown;
            }
            state = ST_KEYHEAD;
            (*keyhead)[0] = 0;
            (*keytail)[0] = 0;
            (*value)[0]   = 0;
            i--; /* don't consume input */
        } else {
            nvAssert(!"Invalid state!");
        }
    }

    if (state == ST_SEND) {
        if (!Subparse(*keyhead, *keytail, *value, readfile)) {
            goto teardown;
        }
    } else if ((state != ST_KEYHEAD) || (ptr != NULL)) {
        /*
         * if state is KEYHEAD and ptr is NULL, then we've just got trailing
         * whitespace or comments, which is valid syntax
         */
        nvEvoLog(EVO_LOG_WARN, "Syntax error in configuration file: "
                 "trailing input after last key-value pair");
        goto teardown;
    }

    ret = TRUE;

    /* fallthrough */
teardown:
    for (i = 0; i < 3; i++) {
        nvFree(strs[i]);
    }
    return ret;
#undef COPYPTR
}

#undef ST_KEYHEAD
#undef ST_KEYTAIL
#undef ST_VALUE
#undef ST_EQUALS
#undef ST_SEND
