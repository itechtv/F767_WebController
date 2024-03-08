/*
 * ds18b20.c
 *
 *	The MIT License.
 *  Created on: 20.09.2018
 *      Author: Mateusz Salamon
 *      www.msalamon.pl
 *      mateusz@msalamon.pl
 *
 */
#include "ds18b20.h"
#include "stdio.h"
#include "main.h"

uint8_t totalsensors = 0; // Общее количество датчико на всех пинах.
uint8_t TempSensorCount = 0;
extern struct dbPinsConf PinsConf[NUMPIN];
extern struct dbPinsInfo PinsInfo[NUMPIN];
//
//	VARIABLES
//
//Ds18b20Sensor_t	ds18b20[SIZE][_DS18B20_MAX_SENSORS];//?ZERG? ЗАМЕНА
Ds18b20Sensor_t	ds18b20[_DS18B20_MAX_SENSORS];//
//OneWire_t OneWire;
uint8_t	OneWireDevices;

//uint8_t quentySensorCounts[SIZE]; // Массив количества датчиков на всех пинах.
OneWire_t OneWire_instances[NUMPIN];
//
//	FUNCTIONS
//
void configureGPIOpin(struct dbPinsInfo* mysensor) {
//    printf("Configured port: %p, pin: %d like ONEWIRE!\r\n", mysensor->port, mysensor->pin);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    char* portName = "";
    char* pinName = "";
    // Проверка порта и включение тактирования
    if (mysensor->gpio_name == GPIOA){
        portName = "GPIOA";
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (mysensor->gpio_name == GPIOB){
        portName = "GPIOB";
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (mysensor->gpio_name == GPIOC){
        portName = "GPIOC";
        __HAL_RCC_GPIOC_CLK_ENABLE();
    } else if (mysensor->gpio_name == GPIOD){
        portName = "GPIOD";
        __HAL_RCC_GPIOD_CLK_ENABLE();
    } else if (mysensor->gpio_name == GPIOE){
        portName = "GPIOE";
        __HAL_RCC_GPIOE_CLK_ENABLE();
    } else if (mysensor->gpio_name == GPIOF){
        portName = "GPIOF";
        __HAL_RCC_GPIOF_CLK_ENABLE();
    } else if (mysensor->gpio_name == GPIOG){
        portName = "GPIOG";
        __HAL_RCC_GPIOG_CLK_ENABLE();
    } else if (mysensor->gpio_name == GPIOH){
        portName = "GPIOH";
        __HAL_RCC_GPIOH_CLK_ENABLE();
    } else{
        printf("Unsupported port!\r\n");
    }

    if (mysensor->hal_pin == GPIO_PIN_0){
        pinName = "GPIO_PIN_0";
    } else if (mysensor->hal_pin == GPIO_PIN_1){
        pinName = "GPIO_PIN_1";
    } else if (mysensor->hal_pin == GPIO_PIN_2){
        pinName = "GPIO_PIN_2";
    } else if (mysensor->hal_pin == GPIO_PIN_3){
        pinName = "GPIO_PIN_3";
    } else if (mysensor->hal_pin == GPIO_PIN_4){
        pinName = "GPIO_PIN_4";
    } else if (mysensor->hal_pin == GPIO_PIN_5){
        pinName = "GPIO_PIN_5";
    } else if (mysensor->hal_pin == GPIO_PIN_6){
        pinName = "GPIO_PIN_6";
    } else if (mysensor->hal_pin == GPIO_PIN_7){
        pinName = "GPIO_PIN_7";
    } else if (mysensor->hal_pin == GPIO_PIN_8){
        pinName = "GPIO_PIN_8";
    } else if (mysensor->hal_pin == GPIO_PIN_9){
        pinName = "GPIO_PIN_9";
    } else if (mysensor->hal_pin == GPIO_PIN_10){
        pinName = "GPIO_PIN_10";
    } else if (mysensor->hal_pin == GPIO_PIN_11){
        pinName = "GPIO_PIN_11";
    } else if (mysensor->hal_pin == GPIO_PIN_12){
        pinName = "GPIO_PIN_12";
    } else if (mysensor->hal_pin == GPIO_PIN_13){
        pinName = "GPIO_PIN_13";
    } else if (mysensor->hal_pin == GPIO_PIN_14){
        pinName = "GPIO_PIN_14";
    } else if (mysensor->hal_pin == GPIO_PIN_15){
        pinName = "GPIO_PIN_15";
    } else {
        printf("Unsupported pin!\r\n");
    }
    printf("Configured port: %s (%p), pin: %s (%d) like ONEWIRE!\r\n", portName, mysensor->gpio_name, pinName, mysensor->hal_pin);
    HAL_GPIO_WritePin(mysensor->gpio_name, mysensor->hal_pin, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = mysensor->hal_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(mysensor->gpio_name, &GPIO_InitStruct);
}
//
//	Start conversion of @number sensor
//
uint8_t DS18B20_Start(uint8_t pin_index, uint8_t sensor_index) {
    Ds18b20Sensor_t* sensor = &ds18b20[sensor_index];

    if (!DS18B20_Is((uint8_t*)sensor->Address))
        return 0;

    OneWire_Reset(&OneWire_instances[pin_index]);
    OneWire_SelectWithPointer(&OneWire_instances[pin_index], sensor->Address);
    OneWire_WriteByte(&OneWire_instances[pin_index], DS18B20_CMD_CONVERTTEMP);

    return 1;
}

//
//	Start conversion on all sensors
//
void DS18B20_StartAll(OneWire_t* onewire)
{
	OneWire_Reset(onewire); // Reset the bus
	OneWire_WriteByte(onewire, ONEWIRE_CMD_SKIPROM); // Skip ROM command
	OneWire_WriteByte(onewire, DS18B20_CMD_CONVERTTEMP); // Start conversion on all sensors
}

//
//	Read one sensor
//
//uint8_t DS18B20_Read(uint8_t number, float *destination)
//{
//	if( number >= TempSensorCount) // If read sensor is not availible
//		return 0;
//
//	uint16_t temperature;
//	uint8_t resolution;
//	float result;
//	uint8_t i = 0;
//	uint8_t data[DS18B20_DATA_LEN];
//#ifdef _DS18B20_USE_CRC
//	uint8_t crc;
//
//#endif
//
//
//	if (!DS18B20_Is((uint8_t*)&ds18b20[number].Address)) // Check if sensor is DS18B20 family
//		return 0;
//
//	if (!OneWire_ReadBit(&OneWire)) // Check if the bus is released
//		return 0; // Busy bus - conversion is not finished
//
//	OneWire_Reset(&OneWire); // Reset the bus
//	OneWire_SelectWithPointer(&OneWire, (uint8_t*)&ds18b20[number].Address); // Select the sensor by ROM
//	OneWire_WriteByte(&OneWire, ONEWIRE_CMD_RSCRATCHPAD); // Read scratchpad command
//
//	for (i = 0; i < DS18B20_DATA_LEN; i++) // Read scratchpad
//		data[i] = OneWire_ReadByte(&OneWire);
//
//#ifdef _DS18B20_USE_CRC
//	crc = OneWire_CRC8(data, 8); // CRC calculation
//
//	if (crc != data[8])
//		return 0; // CRC invalid
//#endif
//	temperature = data[0] | (data[1] << 8); // Temperature is 16-bit length
//
//	OneWire_Reset(&OneWire); // Reset the bus
//
//	resolution = ((data[4] & 0x60) >> 5) + 9; // Sensor's resolution from scratchpad's byte 4
//
//	switch (resolution) // Chceck the correct value dur to resolution
//	{
//		case DS18B20_Resolution_9bits:
//			result = temperature*(float)DS18B20_STEP_9BIT;
//		break;
//		case DS18B20_Resolution_10bits:
//			result = temperature*(float)DS18B20_STEP_10BIT;
//		 break;
//		case DS18B20_Resolution_11bits:
//			result = temperature*(float)DS18B20_STEP_11BIT;
//		break;
//		case DS18B20_Resolution_12bits:
//			result = temperature*(float)DS18B20_STEP_12BIT;
//		 break;
//		default:
//			result = 0xFF;
//	}
//
//	*destination = result;
//
//	return 1; //temperature valid
//}
//TODO
uint8_t DS18B20_Read(uint8_t pin_index, uint8_t sensor_index, uint8_t* destination) {
    Ds18b20Sensor_t* sensor = &ds18b20[sensor_index];
//    printf("READing... \r\n");
    if (!DS18B20_Is((uint8_t*)sensor->Address)){
    	printf("DS18B20_Read=>!DS18B20_Is ");
        for (uint8_t d = 0; d < 8; d++) {
            printf("%02X", sensor->Address[d]);
        }
        printf(" ERROR! \r\n");
        return 0;
    }
//    printf("DS18B20_Read => !DS18B20_Is is OK!\r\n");
    uint16_t temperature;
    uint8_t resolution;
    float result;
    uint8_t data[DS18B20_DATA_LEN];
#ifdef _DS18B20_USE_CRC
    uint8_t crc;
#endif
	if (!DS18B20_Is((uint8_t*)&ds18b20[sensor_index].Address)) // Check if sensor is DS18B20 family
		return 0;
	if (!OneWire_ReadBit(&OneWire_instances[pin_index])) // Check if the bus is released
		return 0; // Busy bus - conversion is not finished
	OneWire_Reset(&OneWire_instances[pin_index]); // Reset the bus
	OneWire_SelectWithPointer(&OneWire_instances[pin_index], (uint8_t*)&ds18b20[sensor_index].Address); // Select the sensor by ROM
	OneWire_WriteByte(&OneWire_instances[pin_index], ONEWIRE_CMD_RSCRATCHPAD); // Read scratchpad command
	for (uint8_t j = 0; j < DS18B20_DATA_LEN; j++) // Read scratchpad
		data[j] = OneWire_ReadByte(&OneWire_instances[pin_index]);
#ifdef _DS18B20_USE_CRC
	crc = OneWire_CRC8(data, 8); // CRC calculation

	if (crc != data[8])
		return 0; // CRC invalid
#endif
	temperature = data[0] | (data[1] << 8); // Temperature is 16-bit length
	OneWire_Reset(&OneWire_instances[pin_index]); // Reset the bus
	resolution = ((data[4] & 0x60) >> 5) + 9; // Sensor's resolution from scratchpad's byte 4
	switch (resolution) // Chceck the correct value dur to resolution
	{
		case DS18B20_Resolution_9bits:
			result = temperature*(float)DS18B20_STEP_9BIT;
		break;
		case DS18B20_Resolution_10bits:
			result = temperature*(float)DS18B20_STEP_10BIT;
		 break;
		case DS18B20_Resolution_11bits:
			result = temperature*(float)DS18B20_STEP_11BIT;
		break;
		case DS18B20_Resolution_12bits:
			result = temperature*(float)DS18B20_STEP_12BIT;
		 break;
		default:
			result = 0xFF;
	}
	*destination = result;
	ds18b20[sensor_index].Temperature = result;
//	printf("Temperature[%d][%d] = %.2f\n", pin_index, sensor_index, result);
	return 1; //temperature valid
}

uint8_t DS18B20_GetResolution(uint8_t number)
{
	uint8_t i = 0;
	if( number >= TempSensorCount)
		return 0;

	uint8_t conf;

	if (!DS18B20_Is((uint8_t*)&ds18b20[number].Address))
		return 0;

	OneWire_Reset(&OneWire_instances[i]); // Reset the bus
	OneWire_SelectWithPointer(&OneWire_instances[i], (uint8_t*)&ds18b20[number].Address); // Select the sensor by ROM
	OneWire_WriteByte(&OneWire_instances[i], ONEWIRE_CMD_RSCRATCHPAD); // Read scratchpad command

	OneWire_ReadByte(&OneWire_instances[i]);
	OneWire_ReadByte(&OneWire_instances[i]);
	OneWire_ReadByte(&OneWire_instances[i]);
	OneWire_ReadByte(&OneWire_instances[i]);

	conf = OneWire_ReadByte(&OneWire_instances[i]); // Register 5 is the configuration register with resolution
	conf &= 0x60; // Mask two resolution bits
	conf >>= 5; // Shift to left
	conf += 9; // Get the result in number of resolution bits

	return conf;
}
/*
  sensor_index - индекс конкретного датчика в массиве ds18b20.
  iterator - индекс строки dbPinsInfo ds18b20[].
 */
//TODO  pin_index, sensor_index
uint8_t DS18B20_SetResolution(uint8_t pin_index, uint8_t sensor_index, DS18B20_Resolution_t resolution, Ds18b20Sensor_t *sensor, OneWire_t *onewire) {
    DS18B20_GetROM(sensor_index, sensor->Address);
    // Проверяем полученный ROM
//    printf("***Address after DS18B20_GetROM***: ");
//    for (int i = 0; i < 8; i++) {
//        printf("%02X", sensor->Address[i]);
//    }
//    printf("\r\n");
//    printf("+++ SetResolution -> pin_index = %d\r\n",pin_index);
    OneWire_Reset(onewire);
    uint8_t th, tl, conf;

    if (!DS18B20_Is((uint8_t*)sensor->Address)) {
        printf("DS18B20_Is = ERROR!");
        for (uint8_t d = 0; d < 8; d++) {
            printf("%02X", sensor->Address[d]);
        }
        printf("\r\n");
        return 0;
    } else {
        printf("DS18B20_Is (sensor_index%d) = ", sensor_index);
        for (uint8_t d = 0; d < 8; d++) {
            printf("%02X", sensor->Address[d]);
        }
        printf("\r\n");
    }

    OneWire_Reset(onewire); // Reset the bus
    OneWire_SelectWithPointer(onewire, sensor->Address); // Select the sensor by ROM
    OneWire_WriteByte(onewire, ONEWIRE_CMD_RSCRATCHPAD); // Read scratchpad command

    OneWire_ReadByte(onewire);
    OneWire_ReadByte(onewire);

    th = OneWire_ReadByte(onewire); // Writing to scratchpad begins from the temperature alarms bytes
    tl = OneWire_ReadByte(onewire); // so I have to store them.
    conf = OneWire_ReadByte(onewire); // Config byte

    if (resolution == DS18B20_Resolution_9bits) {
        conf &= ~(1 << DS18B20_RESOLUTION_R1);
        conf &= ~(1 << DS18B20_RESOLUTION_R0);
    } else if (resolution == DS18B20_Resolution_10bits) {
        conf &= ~(1 << DS18B20_RESOLUTION_R1);
        conf |= 1 << DS18B20_RESOLUTION_R0;
    } else if (resolution == DS18B20_Resolution_11bits) {
        conf |= 1 << DS18B20_RESOLUTION_R1;
        conf &= ~(1 << DS18B20_RESOLUTION_R0);
    } else if (resolution == DS18B20_Resolution_12bits) {
        conf |= 1 << DS18B20_RESOLUTION_R1;
        conf |= 1 << DS18B20_RESOLUTION_R0;
    }

    OneWire_Reset(onewire); // Reset the bus
    OneWire_SelectWithPointer(onewire, sensor->Address); // Select the sensor by ROM
    OneWire_WriteByte(onewire, ONEWIRE_CMD_WSCRATCHPAD); // Write scratchpad command

    OneWire_WriteByte(onewire, th); // Write 3 bytes to scratchpad
    OneWire_WriteByte(onewire, tl);
    OneWire_WriteByte(onewire, conf);

    OneWire_Reset(onewire); // Reset the bus
    OneWire_SelectWithPointer(onewire, sensor->Address); // Select the sensor by ROM
    OneWire_WriteByte(onewire, ONEWIRE_CMD_CPYSCRATCHPAD); // Copy scratchpad to EEPROM

    return 1;
}

uint8_t DS18B20_Is(uint8_t* ROM)
{
	if (*ROM == DS18B20_FAMILY_CODE) // Check family code
		return 1;
	return 0;
}

uint8_t DS18B20_AllDone(OneWire_t* OneWire)
{
	return OneWire_ReadBit(OneWire); // Bus is down - busy
}

//void DS18B20_ReadAll(void)
//{
//	uint8_t i;
//
//	if (DS18B20_AllDone())
//	{
//		for(i = 0; i < TempSensorCount; i++) // All detected sensors loop
//		{
//			ds18b20[i].ValidDataFlag = 0;
//
//			if (DS18B20_Is((uint8_t*)&ds18b20[i].Address))
//			{
//				ds18b20[i].ValidDataFlag = DS18B20_Read(i, &ds18b20[i].Temperature); // Read single sensor
//			}
//		}
//	}
//}Ds18b20Sensor_t	ds18b20[SIZE][_DS18B20_MAX_SENSORS];
void DS18B20_ReadAll(OneWire_t *onewire, uint8_t pin_index) {
	if (DS18B20_AllDone(onewire)) {
//		printf("DS18B20_ReadAl pin_index = %d, quenty of sensor = %d \r\n", pin_index, quentySensorCounts[pin_index]);
//		for (uint8_t i = 0; i < quentySensorCounts[pin_index]; i++){ // All detected sensors loop
		for (uint8_t i = 0; i < _DS18B20_MAX_SENSORS; i++) { // All detected sensors loop
			if (ds18b20[i].idpin == pin_index) {
				ds18b20[i].ValidDataFlag = 0;
//			    printf("test - ValidDataFlag = 0\r\n");
				if (DS18B20_Is((uint8_t*) &ds18b20[i].Address)) {
//				    printf("test-DS18B20_Read...\r\n");
					ds18b20[i].ValidDataFlag = DS18B20_Read(pin_index, i,(uint8_t*) &ds18b20[i].Temperature); // Read single sensor
//				             printf("DS18B20_Read is OK!\r\n"); // DS18B20_Read(uint8_t pin_index, uint8_t sensor_index, uint8_t* destination);
				}
			}
		}
	}
}
//TODO  pin_index, sensor_index
//void DS18B20_GetROM(uint8_t number, uint8_t *ROM) {
//	if (number >= TempSensorCount)
//		number = TempSensorCount;
////	  ROM[0] = 0x11;
////	  ROM[1] = 0x22;
////	  ROM[2] = 0x33;
////	  ROM[3] = 0x44;
////	  ROM[4] = 0x55;
////	  ROM[5] = 0x66;
////	  ROM[6] = 0x77;
////	  ROM[7] = 0x88;
//	printf("DS18B20_GetROM = ");
//	for (int i = 0; i < 8; i++) {
//		ROM[i] = ds18b20[i][number].Address[i];
//		printf("%02X",ROM[i]);
//	}
//	printf("\r\n");
//}
//void DS18B20_GetROM(uint8_t i, uint8_t number, uint8_t *ROM) {
void DS18B20_GetROM(uint8_t sensor_index, uint8_t *ROM){
  if (sensor_index >= TempSensorCount){
	  sensor_index = TempSensorCount;
  }
  Ds18b20Sensor_t *sensor = &ds18b20[sensor_index];
//  ROM[0] = 0x11;
//  ROM[1] = 0x22;
//  ROM[2] = 0x33;
//  ROM[3] = 0x44;
//  ROM[4] = 0x55;
//  ROM[5] = 0x66;
//  ROM[6] = 0x77;
//  ROM[7] = 0x88;
//  printf("DS18B20_GetROM = ");
  for (int j = 0; j < 8; j++) {
    ROM[j] = sensor->Address[j];
  }
//  printf("\r\n");
}

//void DS18B20_WriteROM(uint8_t i, uint8_t number, uint8_t *ROM) {
void 	DS18B20_WriteROM(uint8_t pin_index, uint8_t sensor_index, uint8_t *ROM){
  if (sensor_index >= TempSensorCount)
    return;

  Ds18b20Sensor_t *sensor = &ds18b20[sensor_index];

  for (int j = 0; j < 8; j++) {
    sensor->Address[j] = ROM[j];
  }
}

uint8_t DS18B20_Quantity(void)
{
	return TempSensorCount;
}

uint8_t DS18B20_GetTemperature(uint8_t sensor_index, float* destination, uint8_t pin_index)
{
//	printf("777pin_index=[%d] sensor_index=[%d].ValidDataFlag = %d\n", pin_index, sensor_index, ds18b20->ValidDataFlag);
	if(!ds18b20[sensor_index].ValidDataFlag || ds18b20[sensor_index].idpin != pin_index)
		return 0;
//	Ds18b20Sensor_t *sensor = &ds18b20[pin_index][sensor_index];
//	printf("777ds18b20[%d][%d].Temperature = %f\n", pin_index, sensor_index, ds18b20->Temperature);
//	if(ds18b20[sensor_index].idpin == pin_index){
	*destination = ds18b20[sensor_index].Temperature;
	return 1;
//	}else{
//		return 0;
//	}
}

//void DS18B20_Init(DS18B20_Resolution_t resolution)
//{
//	uint8_t next = 0, i = 0, j;
//	OneWire_Init(&OneWire, DS18B20_GPIO_Port, DS18B20_Pin); // Init OneWire bus
//
//	next = OneWire_First(&OneWire); // Search first OneWire device
//	while(next)
//	{
//		TempSensorCount++;
//		OneWire_GetFullROM(&OneWire, (uint8_t*)&ds18b20[i++].Address); // Get the ROM of next sensor
//		next = OneWire_Next(&OneWire);
//		if(TempSensorCount >= _DS18B20_MAX_SENSORS) // More sensors than set maximum is not allowed
//			break;
//	}
//
//	for(j = 0; j < i; j++)
//	{
//		DS18B20_SetResolution(j, resolution); // Set the initial resolution to sensor
//
//		DS18B20_StartAll(); // Start conversion on all sensors
//	}
//}

void DS18B20_Init(DS18B20_Resolution_t resolution,struct dbPinsInfo *mysensor) {
	uint8_t next = 0;
	uint8_t pin_index = 0;
	uint8_t sensor_index = 0;
	TempSensorCount = 0;
	for (pin_index = 0; pin_index < NUMPIN; pin_index++) {
		if (PinsConf[pin_index].topin == 4) {
			OneWire_Init(&OneWire_instances[pin_index], mysensor[pin_index].gpio_name, mysensor[pin_index].hal_pin);
			char port;
			if (mysensor[pin_index].gpio_name == GPIOA) {
			    port = 'A';
			} else if (mysensor[pin_index].gpio_name == GPIOB) {
			    port = 'B';
			} else if (mysensor[pin_index].gpio_name == GPIOC) {
			    port = 'C';
			} else if (mysensor[pin_index].gpio_name == GPIOD) {
			    port = 'D';
			} else if (mysensor[pin_index].gpio_name == GPIOE) {
			    port = 'E';
			} else if (mysensor[pin_index].gpio_name == GPIOF) {
			    port = 'F';
			} else if (mysensor[pin_index].gpio_name == GPIOG) {
			    port = 'G';
			} else if (mysensor[pin_index].gpio_name == GPIOH) {
			    port = 'H';
			}else {
			    port = 'Z';
			}
			printf("pin_index = %d, Port: %c, Pin = %s\r\n", pin_index, port, mysensor[pin_index].pins);

			next = OneWire_First(&OneWire_instances[pin_index]);
			uint8_t LocalSensorCount = 0;
			while (next) {
				ds18b20[TempSensorCount].idpin = pin_index;
				printf("pin_index-%d TempSensorCount-%d\r\n",pin_index, TempSensorCount);
				OneWire_GetFullROM(&OneWire_instances[pin_index], ds18b20[TempSensorCount].Address);//Получаем ROM и сохраняем в Address
				char port2;
				if (mysensor[pin_index].gpio_name == GPIOA) {
				    port2 = 'A';
				} else if (mysensor[pin_index].gpio_name == GPIOB) {
				    port2 = 'B';
				} else if (mysensor[pin_index].gpio_name == GPIOC) {
				    port2 = 'C';
				} else if (mysensor[pin_index].gpio_name == GPIOD) {
				    port2 = 'D';
				} else if (mysensor[pin_index].gpio_name == GPIOE) {
				    port2 = 'E';
				} else if (mysensor[pin_index].gpio_name == GPIOF) {
				    port2 = 'F';
				} else if (mysensor[pin_index].gpio_name == GPIOG) {
				    port2 = 'G';
				} else if (mysensor[pin_index].gpio_name == GPIOH) {
				    port2 = 'H';
				} else {
				    port2 = 'Y';
				}
				printf("Sensor Count = %d (pin_index = %d), Pin = %d, Port: %c\r\n", TempSensorCount, pin_index, mysensor[pin_index].hal_pin, port2);

				printf("ROM: ");
				for (int k = 0; k < 8; k++) {
					printf("%02X", ds18b20[TempSensorCount].Address[k]);
				}
				printf("\r\n");
				next = OneWire_Next(&OneWire_instances[pin_index]);
				TempSensorCount++;// Увеличиваем счетчик датчиков на текущем пине
				LocalSensorCount++;
				if (LocalSensorCount >= 125) // Если количество датчиков превышает максимальное допустимое значение
					break;
			}
			for ((void)sensor_index; sensor_index < TempSensorCount; sensor_index++) {
				// Проверяем ROM перед отправкой.
	//			printf("+++Address[i] befor SetResolution+++: ");
	//			for (int i = 0; i < 8; i++) {
	//				printf("%02X", ds18b20[sensor_index].Address[i]);
	//			}
	//			printf("\r\n");

				DS18B20_SetResolution(ds18b20[sensor_index].idpin, sensor_index, resolution,&ds18b20[sensor_index],&OneWire_instances[pin_index]);
				DS18B20_StartAll(&OneWire_instances[pin_index]);
			}
			sensor_index = TempSensorCount;
			if (TempSensorCount >= 250) // Если количество датчиков превышает максимальное допустимое значение
				break;
			}
	}
}
