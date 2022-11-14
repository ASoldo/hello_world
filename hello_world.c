// https://github.com/DroomOne/Flipper-Plugin-Tutorial/blob/main/hello_world.c moving text on input
// https://github.com/zmactep/flipperzero-hello-world/blob/main/3_gui/hello_world.c original
// lab.flipper.net/cli log command

// instructions
// ./fbt faps - build all
// ./fbt launch_app APPSRC=hello_world - run app on device

#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>

typedef struct {
    uint8_t x;
    uint8_t y;
} MyMouse;

typedef struct {
    bool isActive;
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
} MySelection;

static MySelection mySelection = {.isActive = false, .x = 0, .y = 0, .w = 0, .h = 0};
static MyMouse myMouse = {.x = 64, .y = 32};

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    // static unsigned char test_bits[] = {
    //     0x13, 0x00, 0x15, 0x00, 0x93, 0xcd, 0x55, 0xa5, 0x93, 0xc5, 0x00, 0x80, 0x00, 0x60};
    // https://convertio.co/jpg-xbm/
    static unsigned char test_bits2[] = {
        0x01, 0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x0F, 0x07, 0x03, 0x01};

    static unsigned char toRightOutline1[] = {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x10, 0x08, 0x04, 0x02, 0x01};
    // canvas_draw_line(canvas, 0, 15, 50, 50);
    // canvas_draw_triangle(canvas, 50, 50, 10, 10, CanvasDirectionBottomToTop);
    // canvas_draw_box(canvas, 75, 50, 35, 35);
    // canvas_draw_box(canvas, 0, 0, 80, 10);
    // canvas_draw_circle(canvas, 75, 25, 8);

    // canvas_draw_xbm(canvas, 0, 0, 5, 10, toRightOutline1);
    // canvas_draw_str(canvas, 7, 10, "ls");
    // canvas_draw_str(canvas, 7, 20, "- .");
    // canvas_draw_str(canvas, 7, 30, "- ..");

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "x: %d", myMouse.x);
    canvas_draw_str(canvas, myMouse.x, myMouse.y + 13, buffer);

    char buffer2[256];
    snprintf(buffer2, sizeof(buffer2), "y: %d", myMouse.y);
    canvas_draw_str(canvas, myMouse.x, myMouse.y + 20, buffer2);

    if(mySelection.isActive)
        canvas_draw_frame(canvas, mySelection.x, mySelection.y, mySelection.w, mySelection.h);

    canvas_draw_line(canvas, 0, 53, 49, 53);

    canvas_draw_box(canvas, 0, 54, 35, 10);
    canvas_invert_color(canvas);
    canvas_draw_str(canvas, 5, 63, "ae3x");

    canvas_invert_color(canvas);
    canvas_draw_str(canvas, 43, 63, "$");

    // canvas_draw_frame(canvas, 50, 0, 76, 10);
    canvas_draw_xbm(canvas, 35, 54, 5, 10, test_bits2);
    canvas_draw_xbm(canvas, 50, 54, 5, 10, toRightOutline1);
    canvas_draw_line(canvas, 0, 63, 49, 63);

    canvas_draw_line(canvas, 126, 0, 126, 64);
    canvas_draw_frame(canvas, 124, 0, 4, 8);

    canvas_draw_frame(canvas, 57, 53, 5, 11);

    static unsigned char mouse_bits[] = {
        0x3F,
        0x1F,
        0x0F,
        0x1F,
        0x3B,
        0x31,
    };

    canvas_draw_xbm(canvas, myMouse.x, myMouse.y, 6, 6, mouse_bits);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    // Checking if the context is not null
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t hello_world_app(void* p) {
    UNUSED(p);
    extern const NotificationSequence sequence_blink_red_100;
    // Current event of type InputEvent
    InputEvent event;
    // Event queue for 8 elements of size InputEvent
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Create a new viewport
    ViewPort* view_port = view_port_alloc();
    // Create a render callback, without context
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    // Create a callback for keystrokes, pass as context
    // our message queue to push these events into it
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Creating a GUI Application
    Gui* gui = furi_record_open(RECORD_GUI);
    // Connect view port to GUI in full screen mode
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    // Used to notify the user by blinking red (error) or blue (fetch successful)
    NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);

    // Infinite event queue processing loop
    while(1) {
        // Select an event from the queue into the event variable (wait indefinitely if the queue is empty)
        // and check that we managed to do it
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
            // If the "back" button is pressed, then we exit the loop, and hence the application
            if(event.key == InputKeyBack) {
                break;
            }
            if(event.key == InputKeyRight) {
                myMouse.x++;
                if(mySelection.isActive && mySelection.w < 128) {
                    mySelection.w++;
                }
                FURI_LOG_D("my app", "soldo %i", 1);
                if(mySelection.isActive) {
                }

                // FURI_LOG_D("integer %i", someintvalue);
                // FURI_LOG_D("character %c", charvalue);
                // FURI_LOG_D("integer %s", chararray);
                // FURI_LOG_D("integer %i %c", someintvalue, charvarlue);
            }
            if(event.key == InputKeyLeft) {
                myMouse.x--;
                if(mySelection.isActive && mySelection.w > 0) {
                    mySelection.w--;
                }
                FURI_LOG_D("my app", "soldo %i", 2);

                // FURI_LOG_D("integer %i", someintvalue);
                // FURI_LOG_D("character %c", charvalue);
                // FURI_LOG_D("integer %s", chararray);
                // FURI_LOG_D("integer %i %c", someintvalue, charvarlue);
            }
            if(event.key == InputKeyUp) {
                myMouse.y--;
                FURI_LOG_D("my app", "soldo %i", 3);
                if(mySelection.isActive && mySelection.h > 0) {
                    mySelection.h--;
                }

                // FURI_LOG_D("integer %i", someintvalue);
                // FURI_LOG_D("character %c", charvalue);
                // FURI_LOG_D("integer %s", chararray);
                // FURI_LOG_D("integer %i %c", someintvalue, charvarlue);
            }
            if(event.key == InputKeyDown) {
                myMouse.y++;
                if(mySelection.isActive && mySelection.h < 64) {
                    mySelection.h++;
                }
                FURI_LOG_D("my app", "soldo %i", 4);

                // FURI_LOG_D("integer %i", someintvalue);
                // FURI_LOG_D("character %c", charvalue);
                // FURI_LOG_D("integer %s", chararray);
                // FURI_LOG_D("integer %i %c", someintvalue, charvarlue);
            }
            if(event.key == InputKeyOk) {
                // myMouse.x = 64;
                // myMouse.y = 32;
                mySelection.isActive = !mySelection.isActive;
                if(mySelection.isActive) {
                    mySelection.x = myMouse.x;
                    mySelection.y = myMouse.y;
                } else {
                    mySelection.x = 0;
                    mySelection.y = 0;
                    mySelection.w = 0;
                    mySelection.h = 0;
                }

                FURI_LOG_D("my app", "soldo %i", 4);
                notification_message(notifications, &sequence_blink_red_100);

                // FURI_LOG_D("integer %i", someintvalue);
                // FURI_LOG_D("character %c", charvalue);
                // FURI_LOG_D("integer %s", chararray);
                // FURI_LOG_D("integer %i %c", someintvalue, charvarlue);
            }
        }
        view_port_update(view_port);
        // https://lab.flipper.net/cli
        // My Flipper
        // Web platform for your Flipper
        // choose cli, then type this command:
        // log
    }

    // Special cleanup of memory occupied by the queue
    furi_message_queue_free(event_queue);
    view_port_enabled_set(view_port, false);
    // We clean the created objects associated with the interface
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    return 0;
}

// Filetype: Flipper Music Format
// Version: 0
// BPM: 130
// Duration: 16
// Octave: 5
// Notes: A#4, F, F5, F4, D#5, F4, C#5, F4, C5, F4, C#5, F4, C5, F4, A#, F, C5, F4, C#5, F4, D#5, F4, C#5, F4, C5, F4, G#, F, C5, F4, A#, F
