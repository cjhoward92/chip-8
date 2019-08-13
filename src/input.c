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

static c8_keyboard_t *keyboards = NULL;

static int get_device_path(char **device_path, const unsigned char device_id) {
    if (device_id < 0 || device_id >= MAX_EVENT)
        return -1;

    *device_path = (char *)malloc(MAX_PATH);
    snprintf(*device_path, MAX_PATH, "/dev/input/event%d", (int)device_id);

    return 0;
}

static int check_path_exists(const char* device_path) {
    if (!device_path)
        return -1;
    
    struct stat *device_stat = (struct stat *)(malloc(sizeof(struct stat)));
    if (stat(device_path, device_stat))
        return -1;
    
    free(device_stat);
    return 0;
}

static int open_input(const char* device_path) {
    return open(device_path, O_RDONLY|O_NONBLOCK);
}

static int get_device_name(char **device_name, int device_fd) {
    size_t max_len = sizeof(char) * 256;
    *device_name = (char *)malloc(max_len); // TODO: constant
    if (ioctl(device_fd, EVIOCGNAME(max_len), *device_name) < 0)
        return -1;

    return 0;
}

// Test that the device accepts key events
static int test_device_accepts_keys(int device_fd) {
    unsigned long ev_buf = 0;
    if ((ioctl(device_fd, EVIOCGBIT(0, sizeof(ev_buf)), &ev_buf)) < 0)
        return -1;

    if (!TEST_BIT(ev_buf, EV_KEY))
        return -1;

    return 0;
}

// test that the device supports our HEX keypad
static int test_device_accepts_key_codes(int device_fd) {
    size_t nchar = KEY_MAX / 8 + 1;
    unsigned char bits[nchar];
    for (short i = 0; i < 16; i++) {
        if(ioctl(device_fd, EVIOCGBIT(EV_KEY, sizeof(bits)), &bits) < 0)
            return -1;

        if (!(bits[keys[i]/8] & (1 << (keys[i] % 8))))
            return -1;
    }
    return 0;
}

// TODO: refactor
int test_device_for_keyboard(c8_keyboard_t **keyboard, const char* device_path) {
    int device_fd = -1;
    int err_code = -1;
    char *device_name = NULL;

    *keyboard = NULL;

    if ((device_fd = open_input(device_path)) < 0)
        return err_code;
    
    if (get_device_name(&device_name, device_fd) < 0)
        goto clean_up;

    if (test_device_accepts_keys(device_fd) < 0)
        goto clean_up;
    
    if (test_device_accepts_key_codes(device_fd) < 0)
        goto clean_up;

    err_code = 0;
    fprintf(stdout, "Device '%s' does support keys\n", device_name);

    *keyboard = (c8_keyboard_t *)(malloc(sizeof(c8_keyboard_t)));
    (*keyboard)->next = NULL;
    (*keyboard)->device_fd = device_fd;

    (*keyboard)->name = (char *)malloc(strlen(device_name));
    strcpy((*keyboard)->name, device_name);

    (*keyboard)->path = (char *)malloc(strlen(device_path));
    strcpy((*keyboard)->path, device_path);

clean_up:
    if (!(*keyboard))
        close(device_fd);
    if (device_name)
        free(device_name);

    return err_code;
}

int init_keyboard() {
    unsigned char device_id = 0;
    char *device_path;

    c8_keyboard_t *head = NULL, *last = NULL, *cur = NULL;
    while (get_device_path(&device_path, device_id++) == 0) {
        // fprintf(stdout, "Found path '%s'\n", device_path);

        if (check_path_exists(device_path))
            goto loop;

        if (test_device_for_keyboard(&cur, device_path) == 0) {
            if (!head) {
                head = cur;
                last = cur;
            } else {
                last->next = cur;
                last = last->next;
            }
        }

loop:
        free(device_path);
    }

    // Set up the keyboards linked list
    if (head) {
        keyboards = head;
    }
    head = NULL;
    last = NULL;
    cur = NULL;

    return 0;
}

static void free_keyboard(c8_keyboard_t *keyboard) {
    if (!keyboard)
        return;
    
    free_keyboard(keyboard->next);
    free(keyboard->name);
    free(keyboard->path);
    close(keyboard->device_fd);
}

int shutdown_keyboard() {
    free_keyboard(keyboards);
    return 0;
}

unsigned char get_key() {
    // TODO: read keys from keyboard
    return 0;
}