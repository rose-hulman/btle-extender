/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
 
 /*
 SPI0 Pins:
 
                Master ----- Slave
 
 MOSI = 25 -------- 25
 MISO = 28 -------- 28
 SCK =     29 -------- 29
 SS =     24 -------- 24
 
 
 
 */
 

/** @file
 *
 * @defgroup ble_sdk_uart_over_ble_main main.c
 * @{
 * @ingroup  ble_sdk_app_nus_eval
 * @brief    UART over BLE application main file.
 *
 * This file contains the source code for a sample application that uses the Nordic UART service.
 * This application uses the @ref srvlib_conn_params module.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"

#include <stdio.h>
#include <stdbool.h>
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "SEGGER_RTT.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                           /**< Include the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#define DEVICE_NAME                     "Adamo's"                               /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout (in units of seconds). */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define START_STRING                    "Start...\n"                                /**< The string that will be sent over the UART when the application starts. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */


#define DELAY_MS                 1000                /**< Timer Delay in milli-seconds. */
#define TX_RX_BUF_LENGTH         16u                 /**< SPI transaction buffer length. */



#if (SPI0_ENABLED == 1)
    static const nrf_drv_spi_t m_spi_master = NRF_DRV_SPI_INSTANCE(0);
#elif (SPI1_ENABLED == 1)
    static const nrf_drv_spi_t m_spi_master = NRF_DRV_SPI_INSTANCE(1);
#elif (SPI2_ENABLED == 1)
    static const nrf_drv_spi_t m_spi_master = NRF_DRV_SPI_INSTANCE(2);
#else
    #error "No SPI enabled."
#endif




static ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */

static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};  /**< Universally unique service identifier. */


// Data buffers.
static uint8_t m_tx_data[TX_RX_BUF_LENGTH] = {0}; /**< A buffer with data to transfer. */
static uint8_t txt_data[TX_RX_BUF_LENGTH] = {0}; /**< A buffer with data to transfer. */
//static uint8_t spi_tx_data[TX_RX_BUF_LENGTH] = {0}; /**< A buffer with data to transfer. */
//static uint8_t spi_rx_data[TX_RX_BUF_LENGTH] = {0}; /**< A buffer for incoming data. */
static uint8_t m_rx_data[TX_RX_BUF_LENGTH] = {0}; /**< A buffer for incoming data. */
static bool newData = false;
static volatile bool sendPacket = false;
static volatile bool receivePacket = false;
static volatile bool pinToggle = false;
static volatile bool m_transfer_completed = true; /**< A flag to inform about completed transfer. */

/*
#    Functions Added after main()
#
#
#
#
#
#
#
*/
void CC1101_Init(void);
void CC1101_Calibrate(void);





/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

//SPI functions follow --------------------------------------------------------------------------------------------------
//
//
/**@brief Function for error handling, which is called when an error has occurred.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name.
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    UNUSED_VARIABLE(bsp_indication_set(BSP_INDICATE_FATAL_ERROR));

    for (;;)
    {
        // No implementation needed.
    }
}
/**@brief Function for checking if data coming from a SPI slave are valid.
 *
 * @param[in] p_buf     A pointer to a data buffer.
 * @param[in] len       A length of the data buffer.
 * 
 * @note Expected ASCII characters from: 'a' to: ('a' + len - 1).
 *
 * @retval true     Data are valid.
 * @retval false    Data are invalid.
 */
static __INLINE bool buf_check(uint8_t * p_buf, uint16_t len)
{
//    uint16_t i;
/*
    for (i = 0; i < len; i++)
    {
        if (p_buf[i] != (uint8_t)('a' + i))
        {
            return false;
        }
    }
*/
    return true;
}


/**@brief Function for SPI master event callback.
 *
 * Upon receiving an SPI transaction complete event, checks if received data are valid.
 *
 * @param[in] spi_master_evt    SPI master driver event.
 */
static void spi_master_event_handler(nrf_drv_spi_event_t event)
{
    uint32_t err_code = NRF_SUCCESS;
    bool result = false;

    switch (event)
    {
        case NRF_DRV_SPI_EVENT_DONE:
            // Check if data are valid.
            result = buf_check(m_rx_data, TX_RX_BUF_LENGTH);
            APP_ERROR_CHECK_BOOL(result);

            err_code = bsp_indication_set(BSP_INDICATE_RCV_OK);
            APP_ERROR_CHECK(err_code);

            // Inform application that transfer is completed.
            m_transfer_completed = true;
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief The function initializes TX buffer to values to be sent and clears RX buffer.
 *
 * @note Function initializes TX buffer to values from 'A' to ('A' + len - 1)
 *       and clears RX buffer (fill by 0).
 *
 * @param[in] p_tx_data     A pointer to a buffer TX.
 * @param[in] p_rx_data     A pointer to a buffer RX.
 * @param[in] len           A length of the data buffers.
 */
static void init_buffers(uint8_t * const p_tx_data, uint8_t * const p_rx_data, const uint16_t  len)
{
    uint16_t i;
    
    for (i = 0; i < len; i++)
    {
        p_tx_data[i] = m_tx_data[i];
        p_rx_data[i] = 0;
    }
        
        
}


/**@brief Functions prepares buffers and starts data transfer.
 *
 * @param[in] p_tx_data     A pointer to a buffer TX.
 * @param[in] p_rx_data     A pointer to a buffer RX.
 * @param[in] len           A length of the data buffers.
 */
static void spi_send_recv(uint8_t * const p_tx_data,
                          uint8_t * const p_rx_data,
                          const uint16_t  len)
{
    // Initalize buffers.
    init_buffers(p_tx_data, p_rx_data, len);
        
    
    // Start transfer.
    
    uint32_t err_code = nrf_drv_spi_transfer(&m_spi_master,
        p_tx_data, len, p_rx_data, len);
    //m_tx_data
    //SEGGER_RTT_printf(0, "\nSent: %s\n", p_tx_data);
    //SEGGER_RTT_printf(0, "\nRecieved: %s\n", p_rx_data);
    
    APP_ERROR_CHECK(err_code);
    nrf_delay_us(10);
}


/**@brief Function for initializing bsp module.
 *
void bsp_configuration()
{
    uint32_t err_code = NRF_SUCCESS;

    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        // Do nothing.
    }

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);
        
    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
}
//
*/
//SPI functions end ------------------------------------------------------------------------------------------------------------



/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of 
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_nus    Nordic UART Service structure.
 * @param[in] p_data   Data to be send to UART module.
 * @param[in] length   Length of the data.
 */
/**@snippet [Handling the data received over BLE] */


static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{        SEGGER_RTT_WriteString(0, "received data \n");
        newData = true;
        memset(txt_data,0,sizeof(txt_data));
    for (uint32_t i = 0; i < length; i++)
    {
                //m_tx_data[i] = p_data[i];
                txt_data[i] = p_data[i];
                SEGGER_RTT_printf(0,"%c", p_data[i]);
        while(app_uart_put(p_data[i]) != NRF_SUCCESS);
    }
      SEGGER_RTT_printf(0, "to send: %s\n", txt_data);
        SEGGER_RTT_WriteString(0, "\n");
    while(app_uart_put('\n') != NRF_SUCCESS);
        
}
/**@snippet [Handling the data received over BLE] */



/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t       err_code;
    ble_nus_init_t nus_init;
    
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;
    
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;
    
    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;
    
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;
        default:
            break;
    }
}


/**@brief Function for the Application's S110 SoftDevice event handler.
 *
 * @param[in] p_ble_evt S110 SoftDevice event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t                         err_code;
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
                    SEGGER_RTT_WriteString(0, "CONNECTED\n");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
                    SEGGER_RTT_WriteString(0, "DISCONNECTED\n");
            err_code = bsp_indication_set(BSP_INDICATE_IDLE);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        default:
                    SEGGER_RTT_WriteString(0, "BLE IDLE...\n");
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a S110 SoftDevice event to all modules with a S110 SoftDevice 
 *        event handler.
 *
 * @details This function is called from the S110 SoftDevice event interrupt handler after a S110 
 *          SoftDevice event has been received.
 *
 * @param[in] p_ble_evt  S110 SoftDevice event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    
}


/**@brief Function for the S110 SoftDevice initialization.
 *
 * @details This function initializes the S110 SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;
    
    // Initialize SoftDevice.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);

    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
#if (defined(S130) || defined(S132))
    ble_enable_params.gatts_enable_params.attr_tab_size   = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
#endif
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            err_code = ble_advertising_restart_without_whitelist();
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            break;
    }
}


/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to 
 *          a string. The string will be be sent over BLE when the last character received was a 
 *          'new line' i.e '\n' (hex 0x0D) or if the string has reached a length of 
 *          @ref NUS_MAX_DATA_LENGTH.
 */
/**@snippet [Handling the data received over UART] */
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;
    uint32_t       err_code;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') || (index >= (BLE_NUS_MAX_DATA_LEN)))
            {
                err_code = ble_nus_string_send(&m_nus, data_array, index);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
                
                index = 0;
            }
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}
/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
static void uart_init(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_ENABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud38400
    };

    APP_UART_FIFO_INIT( &comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);
    APP_ERROR_CHECK(err_code);
}
/**@snippet [UART Initialization] */


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    ble_advdata_t scanrsp;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = false;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = m_adv_uuids;

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = BLE_ADV_FAST_ENABLED;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, &scanrsp, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}




/**@brief Function for placing the application in low power state while waiting for events.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

//void SpiWriteReg(int addr, int value)
//{
//    digitalWrite(SS_PIN, LOW);        // tell CC1101 you are about to WRITE
//    while(digitalRead(MISO_PIN));    // wait for it to accept data
//    SpiTransfer(addr);                // write the address of register
//    SpiTransfer(value);                // write the byte you want to store
//    digitalWrite(SS_PIN, HIGH);        // inform that the WRITE process is over
//}
void CC1101_WriteBurst(uint8_t address, uint8_t * data, uint16_t len)
{
  uint8_t burstAddress;
  int i;

  burstAddress = (address | 0x40);//set R/W=0 and B=1
  m_tx_data[0] = burstAddress; //set address
   for(i=1;i<=len+1;i++){
   m_tx_data[i] = data[i-1];
    }                                                                          
   nrf_gpio_pin_clear(SPIM0_SS_PIN);  //set SS low
   while(nrf_gpio_pin_read(SPIM0_MISO_PIN));  //wait until SO goes low(0)
   spi_send_recv(m_tx_data, m_rx_data, len);
   while(nrf_gpio_pin_read(SPIM0_MISO_PIN));  //wait for SS to low
		if(nrf_gpio_pin_read(14)){
					SEGGER_RTT_WriteString(0, "Pin 15 read\n");
				}
   nrf_gpio_pin_set(SPIM0_SS_PIN);          //set SS high      
}

void CC1101_ReadSingle(uint8_t address)
{              
     uint8_t value[2];
     uint8_t readAddress;
    readAddress = (address | 0x80);                 //set R/w=1 and B=0
    m_tx_data[0] = readAddress; //set address
    m_tx_data[1] = 0x00;
    nrf_gpio_pin_clear(SPIM0_SS_PIN);    //set SS low
    while(nrf_gpio_pin_read(SPIM0_MISO_PIN));  //wait until SO goes low(0)
    spi_send_recv(m_tx_data, value, 2u);
    nrf_gpio_pin_set(SPIM0_SS_PIN);     //set SS high

}


void CC1101_WriteSingle(uint8_t address, uint8_t data)
{
			m_tx_data[0] = address; //set address
      m_tx_data[1] = data;
      nrf_gpio_pin_clear(SPIM0_SS_PIN);  //set SS low
      while(nrf_gpio_pin_read(SPIM0_MISO_PIN));  //wait until SO goes low(0)
      spi_send_recv(m_tx_data, m_rx_data, 2u);
      while(nrf_gpio_pin_read(SPIM0_MISO_PIN));  //wait for SS to low
      nrf_gpio_pin_set(SPIM0_SS_PIN);          //set SS high
                
}

//void SendDataPacket(int *txBuffer,int size)
//{
//    SpiWriteReg(_TXFIFO,size);
//    SpiWriteBurstReg(_TXFIFO,txBuffer,size);            // Write data to send
//    SpiStrobe(_STX);                                    // Start send
//    while (!digitalRead(GDO0));                            // Wait for GDO0 to be set -> sync transmitted
//    while (digitalRead(GDO0));                            // Wait for GDO0 to be cleared -> end of packet
//    SpiStrobe(_SFTX);                                    // Flush TXfifo
//}
void SpiStrobe(uint8_t Strobe)
{
										nrf_gpio_pin_clear(SPIM0_SS_PIN);                    //set SS low
                    while(nrf_gpio_pin_read(SPIM0_MISO_PIN));    //wait until SO goes low(0)
                    m_tx_data[0] = Strobe;    //send SRES command strobe
                    m_tx_data[1] = 0x00;    //
                    while(nrf_gpio_pin_read(SPIM0_MISO_PIN));    //wait until SO goes low(0)
                    spi_send_recv(m_tx_data, m_rx_data, 2u);
										nrf_gpio_pin_set(SPIM0_SS_PIN);
}
void SendDataPacket(uint8_t * TX_data,uint16_t TXFIFO_Address_Size)
{   
		int _STX=0x35;//Command strobe for transmit mode
    int _SFTX=0x3B;//Command strobe for flush TXFIFO
    int TXFIFO_Address=0x3F;//Address of TX FIFO buffer
		//sendPacket = true;
    SpiStrobe(_SFTX);    // Flush TXfifo
  
    nrf_gpio_pin_set(SPIM0_SS_PIN);
		
		nrf_delay_us(1000);
    CC1101_WriteBurst(TXFIFO_Address, TX_data, TXFIFO_Address_Size);//Sends a burst command indicating data and address to send to
		//sendPacket = true;
    SpiStrobe(_STX);//Send transmit mode command strobe
		//SEGGER_RTT_printf(0, "read 14 %x\n",nrf_gpio_pin_read(14));
//		nrf_delay_us(10000);
//				if(nrf_gpio_pin_read(14)){
//					SEGGER_RTT_WriteString(0, "Pin 15 read\n");
//				}
//    nrf_delay_us(2000);
//  nrf_gpio_pin_set(SPIM0_SS_PIN);
//    CC1101_ReadSingle(0x18);
    //nrf_delay_us(10000);
    //SpiStrobe(0x36);
    //nrf_delay_us(1000);
    //SpiStrobe(_SFTX);    // Flush TXfifo
	
    //nrf_delay_ms(100);
//    while(m_rx_data[0]==0x25);
//				nrf_gpio_cfg_watcher(14);
//		nrf_gpio_cfg_input(14,NRF_GPIO_PIN_NOPULL);


		SEGGER_RTT_WriteString(0,"Entering while\n");
		//while(pinToggle == true);
		//SEGGER_RTT_WriteString(0,"Exiting first while\n");
		while(pinToggle == false);
		SEGGER_RTT_WriteString(0,"Exiting second while\n");
		//sendPacket = false;
		pinToggle = false;	
    nrf_gpio_pin_set(SPIM0_SS_PIN);
		nrf_delay_us(100);
		SpiStrobe(0x36);
		nrf_delay_us(100);
}


void SpiReadBurstReg(uint8_t address)
{
               uint8_t burstAddress;
               int i;
               burstAddress = (address | 0xC0);//set R/W=1 and B=1
               m_tx_data[0] = burstAddress;
               m_tx_data[1] = 0x00;
              
               nrf_gpio_pin_clear(SPIM0_SS_PIN);
               nrf_drv_spi_transfer(&m_spi_master, m_tx_data, 2u, m_rx_data, 8u);
              
               nrf_gpio_pin_set(SPIM0_SS_PIN);
}
void RecvDataPacket(void)
{
	
              uint8_t SRX = 0x34;
              uint8_t SFRX = 0x3A;
              SpiStrobe(SRX);							
							nrf_delay_us(1000);
              SpiReadBurstReg(0x3B);		//status register of RXFIFO & with 0x7F because idk why
							nrf_delay_us(1000);
							SpiReadBurstReg(0x3B);		//status register of RXFIFO & with 0x7F because idk why
							nrf_delay_us(1000);

}
void pin_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
	if(pin==14){
		if(action == NRF_GPIOTE_POLARITY_LOTOHI){
			//TODO: Low to high action
			SEGGER_RTT_WriteString(0,"Low to high\n");
		}
		else if(action == NRF_GPIOTE_POLARITY_HITOLO){
			//TODO: High to low action
			SEGGER_RTT_WriteString(0,"High to low\n");
			if(pinToggle == false)
				pinToggle = true;
		}
		else if(action== NRF_GPIOTE_POLARITY_TOGGLE){
			//TODO: Toggle action?
			SEGGER_RTT_printf(0,"Pin %d toggled\n",(uint32_t)pin);
			if(pinToggle == false)
				pinToggle = true;

		}
		SEGGER_RTT_printf(0,"pinToggle = %d, action = %x, sendpacket - %d\n",pinToggle,action, sendPacket);
	}
	//SEGGER_RTT_printf(0,"Pin %d toggled\n",(uint32_t)pin);
	
}


/**@brief Application main function.
 */
int main(void)
{
    uint32_t err_code;
    bool erase_bonds;
    uint8_t  start_string[] = START_STRING;
    printf("%s",start_string);
    // Initialize.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
		nrf_drv_gpiote_init();
    uart_init();
    //buttons_leds_init(&erase_bonds);
    ble_stack_init();
        
    gap_params_init();
    services_init();
    advertising_init();
    conn_params_init();



    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);

    APP_ERROR_CHECK(err_code);
		
      //More SPI Additions
			
         nrf_drv_spi_config_t const config =
    {
        #if (SPI0_ENABLED == 1)
            .sck_pin  = SPIM0_SCK_PIN,
            .mosi_pin = SPIM0_MOSI_PIN,
            .miso_pin = SPIM0_MISO_PIN,
            .ss_pin   = SPIM0_SS_PIN,
        #elif (SPI1_ENABLED == 1)
            .sck_pin  = SPIM1_SCK_PIN,
            .mosi_pin = SPIM1_MOSI_PIN,
            .miso_pin = SPIM1_MISO_PIN,
            .ss_pin   = SPIM1_SS_PIN,
        #elif (SPI2_ENABLED == 1)
            .sck_pin  = SPIM2_SCK_PIN,
            .mosi_pin = SPIM2_MOSI_PIN,
            .miso_pin = SPIM2_MISO_PIN,
            .ss_pin   = SPIM2_SS_PIN,
        #endif
        .irq_priority = APP_IRQ_PRIORITY_LOW,
        .orc          = 0xCC,
        .frequency    = NRF_DRV_SPI_FREQ_1M,
        .mode         = NRF_DRV_SPI_MODE_0,
        .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST,
    };
    ret_code_t err_code1 = nrf_drv_spi_init(&m_spi_master, &config, spi_master_event_handler);
    APP_ERROR_CHECK(err_code1);
        //SEGGER_RTT_WriteString(0, "after error check\n");
        
        //spi_send_recv(m_tx_data, m_rx_data, 2u);
        //err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    // Enter main loop.
        CC1101_Init();
        CC1101_Calibrate();
		
				uint32_t err_code2;
				nrf_drv_gpiote_in_config_t config2 = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
				config2.pull = NRF_GPIO_PIN_PULLUP;
				err_code2 = nrf_drv_gpiote_in_init(14, &config2, pin_event_handler);
				APP_ERROR_CHECK(err_code2);
				nrf_gpio_cfg_sense_input(14,NRF_GPIO_PIN_NOPULL,NRF_GPIO_PIN_SENSE_LOW);

        for (;;)
    {
            
            //Repeated sends data - remove this if statement out if you want to send data from BTLE only
            if (m_transfer_completed)
        {
                    m_transfer_completed = false;
                    uint8_t TX_FIFO_data[5u] = {1,2,3,4,5}; 
                    SendDataPacket(TX_FIFO_data, 6u);//Additional byte (5+1) is header byte
										nrf_delay_ms(1);
                }
            
                if (m_transfer_completed & newData)
        {
										m_transfer_completed = false;
                    newData = false;

                    
                    //SEGGER_RTT_printf(0, "\nstring length %d\n", strlen((char *) txt_data));
                    SendDataPacket(txt_data, strlen((char *) txt_data) + 1);//Additional byte (5+1) is header byte
                    nrf_delay_ms(1);
                    
        }
				
//              CC1101_Init();
                //power_manage();
                
    }
}
void CC1101_Init(void){
	
	
	nrf_gpio_pin_clear(SPIM0_SS_PIN);
	nrf_delay_ms(1);
	nrf_gpio_pin_set(SPIM0_SS_PIN);
	nrf_delay_ms(1);
	nrf_gpio_pin_clear(SPIM0_SS_PIN);
	
	SpiStrobe(0x30);	//SRES = 0x30
	while(nrf_gpio_pin_read(SPIM0_MISO_PIN));
	nrf_gpio_pin_set(SPIM0_SS_PIN);
	
}
void CC1101_Calibrate(void)
{
                CC1101_WriteSingle(0x0B,0x06);              //FSCTRL1
                CC1101_WriteSingle(0x0C,0x00);              //FSCTRL0
                CC1101_WriteSingle(0x0D,0x21);              //FREQ2
                CC1101_WriteSingle(0x0E,0x62);               //FREQ1
                CC1101_WriteSingle(0x0F,0x76);               //FREQ0
                CC1101_WriteSingle(0x10,0xF5);               //MDMCFG4
                CC1101_WriteSingle(0x11,0x83);               //MDMCFG3
                CC1101_WriteSingle(0x12,0x13);               //MDMCFG2
                CC1101_WriteSingle(0x13,0x22);               //MDMCFG1
                CC1101_WriteSingle(0x14,0xF8);               //MDMCFG0
                CC1101_WriteSingle(0x0A,0x00);              //CHANNR
                CC1101_WriteSingle(0x15,0x15);               //DEVIATN
                CC1101_WriteSingle(0x21,0x56);               //FREND1
                CC1101_WriteSingle(0x22,0x10);               //FREND0
                CC1101_WriteSingle(0x18,0x18);               //MCSM0
                CC1101_WriteSingle(0x17,0x00);                             //MCSM1 //20 was previously 00
                CC1101_WriteSingle(0x19,0x16);               //FOCCFG
                CC1101_WriteSingle(0x1A,0x6C);              //BSCFG
                CC1101_WriteSingle(0x1B,0x03);              //AGCCTRL2
                CC1101_WriteSingle(0x1C,0x40);              //AGCCTRL1
                CC1101_WriteSingle(0x1D,0x91);              //AGCCTRL0
                CC1101_WriteSingle(0x23,0xE9);               //FSCAL3
                CC1101_WriteSingle(0x24,0x2A);              //FSCAL2
                CC1101_WriteSingle(0x25,0x00);               //FSCAL1
                CC1101_WriteSingle(0x26,0x1F);               //FSCAL0
                CC1101_WriteSingle(0x29,0x59);               //FSTEST
                CC1101_WriteSingle(0x2C,0x81);              //TEST2
                CC1101_WriteSingle(0x2D,0x35);              //TEST1
                CC1101_WriteSingle(0x2E,0x09);               //TEST0
                CC1101_WriteSingle(0x00,0x29);               //IOCFG2
                CC1101_WriteSingle(0x02,0x06);               //IOCFG0
                CC1101_WriteSingle(0x07,0x04);               //PKTCTRL1
                CC1101_WriteSingle(0x08,0x05);               //PKTCTRL0
                CC1101_WriteSingle(0x09,0x00);               //ADDR
                CC1101_WriteSingle(0x06,0xFF);               //PKTLEN
                CC1101_WriteSingle(0x04,0xD3);              //SYNC1
                CC1101_WriteSingle(0x05,0x91);               //SYNC0
                            CC1101_WriteSingle(0x03,0xFF);              //FIFO THR

}