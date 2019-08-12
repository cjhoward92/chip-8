/**
 * This file contains the logic necessary to listen to
 * the keyboard and ingest key events. I decided to
 * handle this myself as opposed to using a library
 * like SDL2 so I could learn more about input
 * peripherals on Linux.
 * */

#include "input.h"

#define MAX_PATH 20
#define MAX_EVENT 32

static unsigned char keys[16] = {
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
};

static unsigned char keyboard_id = -1;

static int get_device_path(char **device_path, const unsigned char device_id) {
    if (device_id < 0)
        return -1;

    *device_path = (char *)malloc(MAX_PATH);
    snprintf(*device_path, MAX_PATH, "/dev/input/event%d", (int)device_id);

    return 0;
}

int check_path_exists(const char* device_path) {
    if (!device_path)
        return -1;
    
    struct stat *device_stat = (struct stat *)(malloc(sizeof(struct stat)));
    if (stat(device_path, device_stat))
        return -1;
    
    free(device_stat);
    return 0;
}

// TODO: refactor
int test_device_for_keyboard(const char* device_path) {
    int device_fd = -1;
    char device_name[256] = "";

    if ((device_fd = open(device_path, O_RDONLY|O_NONBLOCK)) < 0)
        return -1;
    
    if (ioctl(device_fd, EVIOCGNAME(sizeof(device_name)), device_name) < 0) {
        close(device_fd);
        return -1;
    }
    // fprintf(stdout, "Device: %s\n", device_name);

    unsigned long ev_buf = 0;
    if ((ioctl(device_fd, EVIOCGBIT(0, sizeof(ev_buf)), &ev_buf)) < 0) {
        fprintf(stderr, "There was an error with device '%s'\n", device_path);
        close(device_fd);
        return -1;
    }

    // Test that the device accepts key events
    if (!TEST_BIT(ev_buf, EV_KEY)) {
        close(device_fd);
        return -1;
    }
    
    // test that the device supports our HEX keypad
    size_t nchar = KEY_MAX / 8 + 1;
    unsigned char bits[nchar];
    for (short i = 0; i < 16; i++) {
        if(ioctl(device_fd, EVIOCGBIT(EV_KEY, sizeof(bits)), &bits) < 0) {
            close(device_fd);
            return -1;
        }
        if (!(bits[keys[i]/8] & (1 << (keys[i] % 8)))) {
            close(device_fd);
            return -1;
        }
    }

    fprintf(stdout, "Device '%s' does support keys\n", device_name);

    close(device_fd);
    return 0;
}

int init_keyboard() {
    if (keyboard_id < 0)
        return -1;
    
    unsigned char device_id = 0;
    char *device_path;
    while (!get_device_path(&device_path, device_id++)) {
        // fprintf(stdout, "Found path '%s'\n", device_path);

        if (check_path_exists(device_path)) {
            free(device_path);
            return -1;
        }

        if (!test_device_for_keyboard(device_path)) {
            // do something????
        }

        free(device_path);
        if (!device_id)
            break;
    }

    return 0;
}

unsigned char get_key() {
    // TODO: read keys from keyboard
    return 0;
}