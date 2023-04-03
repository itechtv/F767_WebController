/*
 * wc_sntp.h
 *
 *  Created on: Mar 27, 2023
 *      Author: anton
 */

#ifndef INC_LWIP_SNTP_H_
#define INC_LWIP_SNTP_H_

#include <stdint.h>
/*-----------------------------------------------------------
 * Includes files
 *----------------------------------------------------------*/

/*-----------------------------------------------------------
 * Exported constants
 *----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Exported macro
 *----------------------------------------------------------*/

/*-----------------------------------------------------------
 * Exported function
 *----------------------------------------------------------*/

/*!
* @brief Lwip 的 SNTP 初始化封装接口
*        执行条件：无
*
* @retval: 无
*/
void bsp_sntp_init(void);

/*!
* @brief SNTP 获取时间戳的处理函数
*        执行条件：无
*
* @param [in] : sntp 获取的时间戳
*
* @retval: 无
*/
void sntp_set_time(uint32_t sntp_time);
void SetAndPrintOnUartRTCFromSecond(uint32_t sec);
/*!
 * @brief 获取当前时间戳
 *
 * @param [in] none
 *
 * @retval 当前时间戳
 */
uint32_t get_timestamp(void);

#endif /* INC_LWIP_SNTP_H_ */
