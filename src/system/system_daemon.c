#include "devices/Device.h"
#include "system/Components.h"
#include "system/Serial.h"
#include "system/display.h"
#include "system/system_daemon.h"


void lib7405x_initialize() {
    device_initialize();
    components_initialize();
    serial_initialize();
    gui_initialize();
}

void lib7405x_background_processing() {
    components_background_processing();
}