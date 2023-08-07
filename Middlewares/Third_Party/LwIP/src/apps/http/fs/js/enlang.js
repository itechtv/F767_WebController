var langbutton = new Array();
langbutton[1] = "ID - is a unique numerical identifier of the pin. Assigned automatically";
langbutton[2] = "PIN - The unique number of the digital or analog pin.";
langbutton[3] = "Pullup type (EXTERNAL_PULLDOWN, INTERNAL_PULLUP, EXTERNAL_PULLUP)";
langbutton[4] = "Bounce interval (ms) - Suppression interval of bounce of button contacts, usually 40ms are enough.";
langbutton[5] = "Hold interval (ms) - The time in ms that the button has to be held down for it to go into the Hold state.";
langbutton[6] = "Repeat - 1 or 0 - 'On' or 'Off' - enable or disable auto-repeat when in the hold mode.";
langbutton[7] = "Repeat interval (ms) - The interval in ms at which to repeat the presses in the hold mode (the Repeat field must be set to 1). It is usually 150ms.";
langbutton[8] = "Double-click interval (ms) - The interval in ms during which two consecutive button presses will generate a DoubleClick event. 0 - does not track a double-click. It is usually 800ms.";
langbutton[9] = "Prevent Click - 1 or 0 - Prevent (1) - a Click event from being generated the first time you click if it is followed by a DoubleClick. Makes sense only if Double-click interval is greater than zero.";
langbutton[10] = "Relay connection - Here will appear one or more relays of pin(s) with which this button interacts.";
langbutton[11] = "INFO - Give a name of this button for quick navigation. Example: \"Kitchen\", \"Children room\", etc. Max. 30 characters!";
langbutton[12] = "On/Off - 'On' or 'Off' this pin/switch.";
langbutton[13] = "Action - Some actions with this pin.";

var langeditbutton = new Array();
langeditbutton[1] = "ID - is a unique numerical identifier of the pin. Assigned automatically";
langeditbutton[2] = "PIN - The unique number of the digital or analog pin.";
langeditbutton[3] = "Pullup type (EXTERNAL_PULLDOWN, INTERNAL_PULLUP, EXTERNAL_PULLUP)";
langeditbutton[4] = "Bounce interval (ms) - Suppression interval of bounce of button contacts, usually 40ms are enough.";
langeditbutton[5] = "Hold interval (ms) - The time in ms that the button has to be held down for it to go into the Hold state.";
langeditbutton[6] = "Repeat - 1 or 0 - 'On' or 'Off' - enable or disable auto-repeat when in the hold mode.";
langeditbutton[7] = "Repeat interval (ms) - The interval in ms at which to repeat the presses in the hold mode (the Repeat field must be set to 1). It is usually 150ms.";
langeditbutton[8] = "Double-click interval (ms) - The interval in ms during which two consecutive button presses will generate a DoubleClick event. 0 - does not track a double-click. It is usually 800ms.";
langeditbutton[9] = "Prevent Click - 1 or 0 - Prevent (1) - a Click event from being generated the first time you click if it is followed by a DoubleClick. Makes sense only if Double-click interval is greater than zero.";
langeditbutton[10] = "Relay connection - Here will appear one or more relays of pin(s) with which this button interacts.";
langeditbutton[11] = "INFO - Give a name of this button for quick navigation. Example: \"Kitchen\", \"Children room\", etc. Max. 30 characters!";
langeditbutton[12] = "On/Off - 'On' or 'Off' this pin/switch.";

var langrelay = new Array();
langrelay[1] = "ID - is a unique numerical identifier of the pin. Assigned automatically";
langrelay[2] = "PIN - The unique number of the digital or analog pin.";
langrelay[3] = "PWM frequency (Hz) - PWM frequency in Hertz. If 0, then PWM is not used.";
langrelay[4] = "ON - State of pin -1 is 'On', 0 is 'Off'.";
langrelay[5] = "Invert State - port inversion (Off - logical 1 at the output, on - 0).";
langrelay[6] = "Dimmer value (0-255) - If you use PWM, it will between (0-255).";
langrelay[7] = "Power On Restore - What parameters should be saved when changing and restored when the controller is on None - do not save anything, when you turn on the state and value are equal to those set when configuring. State - restore the last set state of parameter On. Value - restore the last set value of dimmer (PWM). State&Value - restore state and value.";
langrelay[8] = "INFO - Give a name to the selected relay for quick navigation, e.g.\"Kitchen\", \"Kids room\", etc. No more than 30 characters!";
langrelay[9] = "On/Off - 'On' or 'Off' this relay.";
langrelay[10] = "Action - Some actions with this pin.";

var langeditrelay = new Array();
langeditrelay[1] = "ID - is a unique numerical identifier of the pin. Assigned automatically";
langeditrelay[2] = "PIN - The unique number of the digital or analog pin.";
langeditrelay[3] = "PWM frequency (Hz) - PWM frequency in Hertz. If 0, then PWM is not used.";
langeditrelay[4] = "ON - State of pin -1 is 'On', 0 is 'Off'.";
langeditrelay[5] = "Invert State - port inversion (Off - logical 1 at the output, on - 0).";
langeditrelay[6] = "Dimmer value (0-255) - If you use PWM, it will between (0-255).";
langeditrelay[7] = "Power On Restore - What parameters should be saved when changing and restored when the controller is on None - do not save anything, when you turn on the state and value are equal to those set when configuring. State - restore the last set state of parameter On. Value - restore the last set value of dimmer (PWM). State&Value - restore state and value.";
langeditrelay[8] = "INFO - Give a name to the selected relay for quick navigation, e.g.\"Kitchen\", \"Kids room\", etc. No more than 30 characters!";
langeditrelay[9] = "On/Off - 'On' or 'Off' this relay.";

var langtimers = new Array();
langtimers[1] = "ID - is a unique numerical identifier of the cron. Assigned automatically";
langtimers[2] = "Configure a cron.";
langtimers[3] = "What action must be performed at the time specified in the timer.";
langtimers[4] = "INFO - Give a name to the selected timer for quick navigation, e.g.\"Lawn Watering\", \"Backyard Light\", etc. No more than 30 characters!";
langtimers[5] = "Action - Some actions with this cron.";

var langsettings = new Array();
langsettings[1] = "Login - Enter the username for authorization.";
langsettings[2] = "Password - Enter your password for authorization.";
langsettings[3] = "Language - Select the interface/help language.";
langsettings[4] = "Time zone - Select your time zone.";
langsettings[5] = "IP address - Enter a static IP address. After the reboot, the STM32 will be available at the entered address!";
langsettings[6] = "Subnet mask - Enter the subnet mask.";
langsettings[7] = "Default gateway - Enter the default gateway.";
langsettings[8] = "MAC address - If you have more than one device, you need to give each device a unique MAC address.";
langsettings[9] = "Host - Enter the static IP address of the MQTT broker.";
langsettings[10] = "Port - Specify the MQTT port of the broker.";
langsettings[11] = "Client - Specify the client ID.";
langsettings[12] = "User - Specify a user name.";
langsettings[13] = "Password - Enter password for authorization.";
langsettings[14] = "Topic - Specify the name of the topic";
langsettings[15] = "Full Topic - Specify the full topic.";
langsettings[16] = "QOS - Specify the quality of service.";
langsettings[17] = "Longitude - Round up the longitude value to three decimal places.";
langsettings[18] = "Latitude - Round up the latitude value to three decimal places.";
langsettings[19] = "Sunrise - The sunrise time is automatically set according to your coordinates.";
langsettings[20] = "Sunset - The sunset time is set automatically according to your coordinates.";

var cronhelp = new Array();
cronhelp[1] = "CRON is valid all the time, will fire every second.";
cronhelp[2] = "CRON is valid at the beginning of each minute.";
cronhelp[3] = "CRON is valid every Tuesday all day long.";
cronhelp[4] = "CRON is valid every beginning of the minute between hours 13-15 afternoon, between Tuesday and Thursday.";
cronhelp[5] = "CRON is valid every 5 seconds starting at 0.";
cronhelp[6] = "CRON is valid every 5 seconds each 5 minutes, from 00:00 to 55:55.";
cronhelp[7] = "Every Friday at midnight.";
cronhelp[8] = "Every 2 hours at beginning of the hour.";
cronhelp[9] = "Every second of every minute every 2 hours (0, 2, 4, .., 22).";
cronhelp[10] = "At midnight, 00:00 every week between Monday and Friday.";
cronhelp[11] = "Every 6 hours at (min:sec) 23:15 (00:23:15, 06:23:15, 12:23:15, …).";
cronhelp[12] = "At 00:00:00 beginning of the month.";
cronhelp[13] = "Every beginning of the quarter at 00:00:00.";
cronhelp[14] = "At 20:15:20 every Saturday in August.";
cronhelp[15] = "At 20:15:20 every Saturday that is also 8th day in month (both must match, day Saturday and date 8th).";
cronhelp[16] = "Every second between 30 and 45.";
cronhelp[17] = "Every 3rd second in every minute, when seconds are between 30 and 45.";
cronhelp[18] = "Every beginning of a minute when minute is between 23 and 59.";
cronhelp[19] = "Every second when seconds are from 50-59 and 00-10 (overflow mode).";

var actionhelp = new Array();
actionhelp[1] = "Pin 18 will turn on (ON) at the specified time (CRON), stay on for 5 seconds and turn off (OFF) at the end of the pause (p - PAUSE).";
actionhelp[2] = "Pin 12 will change its state (TOGGLE) after 5 seconds (p - PAUSE).";









