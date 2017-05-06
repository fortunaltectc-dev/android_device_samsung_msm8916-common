/*
 * Copyright (c) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define LOG_TAG "PowerHAL_H_Ext"
#include <utils/Log.h>

char* touch_paths[] {
    "/sys/devices/soc.0/i2c.78/i2c-11/11-0020/input/input1/enabled",
    "/sys/devices/soc.0/78b9000.i2c/i2c-5/5-0020/input/input1/enabled",
    "/sys/devices/soc.0/i2c.70/i2c-12/12-0020/input/input2/enabled",
    "/sys/devices/soc.0/78b9000.i2c/i2c-5/5-0020/input/input2/enabled",
    "/sys/devices/soc.0/i2c.70/i2c-12/12-0020/input/input3/enabled",
    "/sys/devices/soc.0/i2c.72/i2c-12/12-0048/input/input3/enabled",
    "/sys/class/input/input2/enabled",
    "/sys/class/input/input3/enabled",
    NULL,
};

static char* get_touch_path() {
    int i;
    char * path = NULL;

    for (i = 0; ; i++) {
        path = touch_paths[i];
        if (path == NULL) break;

        if (access(path, F_OK) == 0) break;
    };
    return path;
}

static void sysfs_write(char *path, char *s) {
    char buf[80];
    int len;

    if (path == NULL) return;

    int fd = open(path, O_WRONLY);

    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error opening %s: %s\n", path, buf);
        return;
    }

    len = write(fd, s, strlen(s));
    if (len < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error writing to %s: %s\n", path, buf);
    }
    close(fd);
}

void cm_power_set_interactive_ext(int on) {
    ALOGD("%s: %s input devices", __func__, on ? "enabling" : "disabling");
    sysfs_write(get_touch_path(), on ? "1" : "0");
}
