#include <usb_gamepad.h>
#include <usb_mouse.h>
#include <usb_keyboard.h>
#include "usb_host_config.h"
#include "utils.h"
#include "tim.h"
#include "mouse.h"
#include "gpio.h"
#include "keyboard.h"
#include "gamepad.h"



void IWDG_Feed_Init(u16 prer, u16 rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(prer);
    IWDG_SetReload(rlr);
    IWDG_ReloadCounter();
    IWDG_Enable();
}



int main( void )
{
   // USART_Printf_Init( 115200 );
    DUG_PRINTF( "SystemClk:%d\r\n", SystemCoreClock );
    Delay_Init( );
    TIM3_Init( 9, SystemCoreClock / 10000 - 1 );

#if DEF_USBFS_PORT_EN
    USBFS_RCC_Init( );
    USBFS_Host_Init( ENABLE );
    memset( &RootHubDev.bStatus, 0, sizeof( ROOT_HUB_DEVICE ) );
    memset( &HostCtl[ DEF_USBFS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL ].InterfaceNum, 0, DEF_ONE_USB_SUP_DEV_TOTAL * sizeof( HOST_CTL ) );
#endif
    TIM1_Init();
    TIM2_Init();
    TIM4_Init();
    GPIO_Config();
    InitMouse();
    IWDG_Feed_Init( IWDG_Prescaler_32, 4000 );

	while (1) {
	    IWDG_ReloadCounter();
		USBH_MainDeal();
		//Handle HID/Xbox root device
		if ((RootHubDev.bType == USB_DEV_CLASS_HID)
				|| (RootHubDev.bType == DEF_DEV_TYPE_XBOX360)) {

			for (int itf = 0; itf < DEF_INTERFACE_NUM_MAX; itf++) {
				//Handle mouse
				if (HostCtl[0].Interface[itf].HIDRptDesc.type
						== REPORT_TYPE_MOUSE) {
				    HID_MOUSE_Data *mousemap = USB_GetMouseInfo(
							&HostCtl[0].Interface[itf]);
					ProcessMouse(mousemap);
				}

				//Handle gamepad
				if ((HostCtl[0].Interface[itf].HIDRptDesc.type
						== REPORT_TYPE_JOYSTICK)
						|| (HostCtl[0].Interface[itf].Type == DEC_XBOX360)) {

		HID_gamepad_Info_TypeDef *gamepad = GetGamepadInfo(
							&HostCtl[0].Interface[itf]);
						ProcessGamepad(gamepad);
				}

				// Handle Keyboard
				if (HostCtl[0].Interface[itf].HIDRptDesc.type
						== REPORT_TYPE_KEYBOARD) {
					//HID_KEYBD_Info_TypeDef *USBH_HID_GetKeybdInfo(Interface *Itf)
				    HID_Keyboard_Data *kbd = USBH_HID_GetKeyboardInfo(
							&HostCtl[0].Interface[itf]);

					amikb_process(kbd);

				}

			}
		}

		//Handle HUB Device

		if (RootHubDev.bType == USB_DEV_CLASS_HUB) {

			//Iterate over all devices
			for (uint8_t device = 1; device < 5; device++)
			{
				//Iterate over all interfaces
				for (int itf = 0; itf < DEF_INTERFACE_NUM_MAX; itf++) {
					//Handle mouse
					if (HostCtl[device].Interface[itf].HIDRptDesc.type
							== REPORT_TYPE_MOUSE) {
					    HID_MOUSE_Data *mousemap = USB_GetMouseInfo(
								&HostCtl[device].Interface[itf]);
							ProcessMouse(mousemap);

					}

					//Handle gamepad
					if ((HostCtl[device].Interface[itf].HIDRptDesc.type
							== REPORT_TYPE_JOYSTICK)
							|| (HostCtl[device].Interface[itf].Type == DEC_XBOX360)) {

			HID_gamepad_Info_TypeDef *gamepad = GetGamepadInfo(
								&HostCtl[device].Interface[itf]);
							ProcessGamepad(gamepad);
					}

					// Handle Keyboard
					if (HostCtl[device].Interface[itf].HIDRptDesc.type
							== REPORT_TYPE_KEYBOARD) {
					    HID_Keyboard_Data *kbd = USBH_HID_GetKeyboardInfo(
								&HostCtl[device].Interface[itf]);
							amikb_process(kbd);

					}

				}
			}



			}

		}

	}

