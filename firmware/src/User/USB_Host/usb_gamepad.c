#include <usb_gamepad.h>
#include <stdio.h>
#include <stdlib.h>

#define JOYSTICK_AXIS_MIN           0
#define JOYSTICK_AXIS_MID           127
#define JOYSTICK_AXIS_MAX           255
#define JOYSTICK_AXIS_TRIGGER_MIN   64
#define JOYSTICK_AXIS_TRIGGER_MAX   192

#define JOY_RIGHT       0x01
#define JOY_LEFT        0x02
#define JOY_DOWN        0x04
#define JOY_UP          0x08
#define JOY_BTN_SHIFT   4
#define JOY_BTN1        0x10
#define JOY_BTN2        0x20
#define JOY_BTN3        0x40
#define JOY_BTN4        0x80
#define JOY_MOVE        (JOY_RIGHT | JOY_LEFT | JOY_UP | JOY_DOWN)

#define XBOX360_INPUT_REPORT_LEN   20
#define XBOX360_STICK_DEADZONE     12000

HID_gamepad_Info_TypeDef gamepad_info;
static uint8_t gamepad_report_data[64];

static int16_t Xbox360_ReadLE16S(const uint8_t *buf)
{
    return (int16_t)((uint16_t)buf[0] | ((uint16_t)buf[1] << 8));
}

static USB_Status GamepadDecodeXbox360(uint16_t report_len)
{
    uint8_t jmap = 0;
    uint8_t btn_extra = 0;
    uint8_t buttons_low;
    uint8_t buttons_high;
    uint8_t lt;
    uint8_t rt;
    int16_t lx;
    int16_t ly;
    int16_t rx;
    int16_t ry;

    if (report_len < XBOX360_INPUT_REPORT_LEN)
    {
        return USB_FAIL;
    }

    if ((gamepad_report_data[0] != 0x00U) || (gamepad_report_data[1] != 0x14U))
    {
        return USB_FAIL;
    }

    buttons_low = gamepad_report_data[2];
    buttons_high = gamepad_report_data[3];
    lt = gamepad_report_data[4];
    rt = gamepad_report_data[5];
    lx = Xbox360_ReadLE16S(&gamepad_report_data[6]);
    ly = Xbox360_ReadLE16S(&gamepad_report_data[8]);
    rx = Xbox360_ReadLE16S(&gamepad_report_data[10]);
    ry = Xbox360_ReadLE16S(&gamepad_report_data[12]);

    if (buttons_low & (1U << 0)) jmap |= JOY_UP;
    if (buttons_low & (1U << 1)) jmap |= JOY_DOWN;
    if (buttons_low & (1U << 2)) jmap |= JOY_LEFT;
    if (buttons_low & (1U << 3)) jmap |= JOY_RIGHT;

    if (buttons_high & (1U << 4)) jmap |= JOY_BTN1;
    if (buttons_high & (1U << 5)) jmap |= JOY_BTN2;
    if (buttons_high & (1U << 6)) jmap |= JOY_BTN3;
    if (buttons_high & (1U << 7)) jmap |= JOY_BTN4;

    if (lx > XBOX360_STICK_DEADZONE) jmap |= JOY_RIGHT;
    if (lx < -XBOX360_STICK_DEADZONE) jmap |= JOY_LEFT;
    if (ly > XBOX360_STICK_DEADZONE) jmap |= JOY_UP;
    if (ly < -XBOX360_STICK_DEADZONE) jmap |= JOY_DOWN;

    if (buttons_high & (1U << 0)) btn_extra |= (1U << 0); // LB
    if (buttons_high & (1U << 1)) btn_extra |= (1U << 1); // RB
    if (buttons_low & (1U << 4)) btn_extra |= (1U << 2);  // Start
    if (buttons_low & (1U << 5)) btn_extra |= (1U << 3);  // Back
    if (buttons_high & (1U << 2)) btn_extra |= (1U << 4); // Guide
    if (buttons_low & (1U << 6)) btn_extra |= (1U << 5);  // L3
    if (buttons_low & (1U << 7)) btn_extra |= (1U << 6);  // R3
    if ((lt > 0U) || (rt > 0U)) btn_extra |= (1U << 7);

    gamepad_info.gamepad_data = jmap;
    gamepad_info.gamepad_extraBtn = btn_extra;

    (void)rx;
    (void)ry;

    return USB_OK;
}

HID_gamepad_Info_TypeDef *GetGamepadInfo(Interface *Itf)
{
    //refresh value of joymap and return value
    if (GamepadDecode(Itf) == USB_OK)
    {
        return &gamepad_info;
    }

    return NULL;
}

USB_Status GamepadDecode(Interface *Itf)
{
    uint16_t report_len;

    if (Itf->HidRptLen == 0U)
    {
        return USB_FAIL;
    }

    report_len = Itf->HidRptLen;
    if (report_len > sizeof(gamepad_report_data))
    {
        report_len = sizeof(gamepad_report_data);
    }

    if (FifoRead(&Itf->buffer, gamepad_report_data, report_len) == 0U)
    {
        return USB_FAIL;
    }

    if (Itf->Type == DEC_XBOX360)
    {
        return GamepadDecodeXbox360(report_len);
    }

    {
        uint8_t jmap = 0;
        uint8_t btn = 0;
        uint8_t btn_extra = 0;
        int16_t a[2];
        uint8_t i;

        hid_report_t conf = Itf->HIDRptDesc;

        // skip report id if present
        uint8_t *p = gamepad_report_data + (conf.report_id ? 1 : 0);

        // process axis
        for (i = 0; i < 2; i++)
        {
            // if logical minimum is > logical maximum then logical minimum
            // is signed. This means that the value itself is also signed
            int is_signed = conf.joystick_mouse.axis[i].logical.min >
                            conf.joystick_mouse.axis[i].logical.max;
            a[i] = collect_bits(p, conf.joystick_mouse.axis[i].offset,
                                conf.joystick_mouse.axis[i].size, is_signed);
        }

        // process first 4 buttons
        for (i = 0; i < 4; i++)
        {
            if (p[conf.joystick_mouse.button[i].byte_offset] &
                conf.joystick_mouse.button[i].bitmask)
            {
                btn |= (1U << i);
            }
        }

        // process extra buttons
        for (i = 4; i < 12; i++)
        {
            if (p[conf.joystick_mouse.button[i].byte_offset] &
                conf.joystick_mouse.button[i].bitmask)
            {
                btn_extra |= (1U << (i - 4));
            }
        }

        for (i = 0; i < 2; i++)
        {
            int hrange = (conf.joystick_mouse.axis[i].logical.max -
                          abs(conf.joystick_mouse.axis[i].logical.min)) / 2;
            int dead = hrange / 63;

            if (a[i] < conf.joystick_mouse.axis[i].logical.min) a[i] = conf.joystick_mouse.axis[i].logical.min;
            else if (a[i] > conf.joystick_mouse.axis[i].logical.max) a[i] = conf.joystick_mouse.axis[i].logical.max;

            a[i] = a[i] - (abs(conf.joystick_mouse.axis[i].logical.min) +
                           conf.joystick_mouse.axis[i].logical.max) / 2;

            hrange -= dead;
            if (a[i] < -dead) a[i] += dead;
            else if (a[i] > dead) a[i] -= dead;
            else a[i] = 0;

            a[i] = (a[i] * 127) / hrange;

            if (a[i] < -127) a[i] = -127;
            else if (a[i] > 127) a[i] = 127;

            a[i] = a[i] + 127; // mist wants a value in the range [0..255]
        }

        if (a[0] < JOYSTICK_AXIS_TRIGGER_MIN) jmap |= JOY_LEFT;
        if (a[0] > JOYSTICK_AXIS_TRIGGER_MAX) jmap |= JOY_RIGHT;
        if (a[1] < JOYSTICK_AXIS_TRIGGER_MIN) jmap |= JOY_UP;
        if (a[1] > JOYSTICK_AXIS_TRIGGER_MAX) jmap |= JOY_DOWN;
        jmap |= btn << JOY_BTN_SHIFT;

        gamepad_info.gamepad_data = jmap;
        gamepad_info.gamepad_extraBtn = btn_extra;
    }

    return USB_OK;
}
