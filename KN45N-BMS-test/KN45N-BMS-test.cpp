/*============================================================================================================================================================================
Information
==============================================================================================================================================================================
SDK: 		v2.1.0
Toolchain:	13.3Rel1
Ninja:		v1.12.1
CMake:		v3.31.4
============================================================================================================================================================================*/



/*============================================================================================================================================================================
Include	Lib
============================================================================================================================================================================*/
#include	<stdio.h>
#include	<stdint.h>


// SDK Pico
#include	"pico/stdlib.h"
#include	"hardware/spi.h"
#include	"hardware/i2c.h"
#include	"pico/cyw43_arch.h"
#include	"hardware/uart.h"

// Other Lib
// #include	"SSD1306.h"



/*============================================================================================================================================================================
Defines     Var                             Val             Mô tả
============================================================================================================================================================================*/
// SPI defines
#define     SPI_PORT                        spi0
#define     PIN_MISO                        16
#define     PIN_CS                          17
#define     PIN_SCK                         18
#define     PIN_MOSI                        19


// I2C defines
#define     I2C_PORT                        i2c0
#define     I2C_SDA                         8
#define     I2C_SCL                         9
// SSD1306 defines
#define     SCREEN_WIDTH                    128
#define     SCREEN_HEIGHT                   64
#define     OLED_RESET                      -1
#define     SCREEN_ADDRESS                  0x3C



// // UART defines (default UART 'uart0')
// #define     UART_ID                         uart1
// #define     BAUD_RATE                       115200
// // PIN
// #define     UART_TX_PIN                     4
// #define     UART_RX_PIN                     5




/*============================================================================================================================================================================
Const Type        Var                           Val                     Mô tả                                               Đơn vị          Note
============================================================================================================================================================================*/
// _BIX                                                                                                                                     // Same as _BIF old version
const   uint8_t     REVISION                    = 0x1;                  // Version of the data structure _BIX               // N/a          // Basic: 0x1
/*cont*/bool        POWER_UNIT                  = 0x1;                  // Bit Logic đơn vị                                 // N/a          // 0: mWh, 1: mAh
const   uint32_t    DESIGN_CAPACITY             = 0x186A0;              // Dung lượng thiết kế của pin                      // POWER_UNIT   // max: 4294967295
        uint32_t    LAST_FULL_CHARGE_CAPACITY   = 0x186A0;              // Dung lượng sạc đầy cuối cùng của pin             // POWER_UNIT   // max: DESIGN_CAPACITY
const   bool        BATTERY_TECHNOLOGY          = 0x1;                  // Loại công nghệ pin (khả năng tái tạo)            // N/a          // 0: No, 1: Yes
const   uint16_t    DESIGN_VOLTAGE              = 0x4A38;               // Điện áp thiết kế của pin                         // mV           // Basic voltage: 19V
        uint32_t    DESIGN_CAPACITY_OF_WARNING  = 0x2710;               // Dung lượng cảnh báo thiết kế của pin             // POWER_UNIT   // max: LAST_FULL_CHARGE_CAPACITY
        uint32_t    DESIGN_CAPACITY_OF_LOW      = 0x2710;               // Dung lượng thấp thiết kế của pin (hibernation)   // POWER_UNIT   // max: LAST_FULL_CHARGE_CAPACITY
        uint8_t     CYCLE_COUNT                 = 0x0;                  // Số lần sạc của pin                               // Times        // max: 255
/*cont*/uint32_t    MEASUREMENT_ACCURACY        = 0x1;                  // Độ chính xác đo lường                            // PPM          // @Kn45nb Cần tối ưu đoạn này
/*cont*/uint16_t    MAX_SAMPLING_TIME           = 0x1;                  // Thời gian lấy mẫu tối đa                         // ms           // 
/*cont*/uint16_t    MIN_SAMPLING_TIME           = 0x1;                  // Thời gian lấy mẫu tối thiểu                      // ms           //
const   uint16_t    MAX_OUTPUT_VOLTAGE          = 0x4A38;               // Điện áp đầu ra tối đa                            // mV           // Basic voltage: 19V
const   uint8_t     MEASUREMENT_GRANULARITY     = 0x1;                  // Độ tinh mịch (phân giải) đo lường                // N/a          //
const   uint8_t     CAPACITY_GRANULARITY_1      = 0x1;                  // Độ tinh mịch (phân giải) dung lượng trên ngưỡng  // POWER_UNIT   //
const   uint8_t     CAPACITY_GRANULARITY_2      = 0x1;                  // Độ tinh mịch (phân giải) dung lượng dưới ngưỡng  // POWER_UNIT   //
const   char        MODEL_NUMBER[]              = "Notebook";           // Mã số model của pin                              // N/a          //
const   char        SERIAL_NUMBER[]             = "0001";               // Số serial duy nhất của pin                       // N/a          //
const   char        BATTEY_TYPE[]               = "LION";               // Loại pin                                         // N/a          //
const   char        OEM_INFORMATION[]           = "MAKE BY @Kn45nb";    // Thông tin OEM                                    // N/a          //


// _BST
        uint8_t     BATTERY_STATE               = 0x4;                  // Trạng thái pin                                   // N/a          // 0x0001: Sạc
                                                                                                                                            // 0x0002: Xả
                                                                                                                                            // 0x0004: Full
                                                                                                                                            // 0x0008: Low
                                                                                                                                            // 0x0010: Error
        int16_t     BATTERY_PRESENT_RATE        = 0x0;                  // Tốc độ Sạc/xả (Điện áp giữa 2 đầu nguồn điện)    // POWER_UNIT   // Âm sạc, dương xả
        uint32_t    BATTEY_REMAINING_CAPACITY   = 0x0;                  // Dung lượng còn lại của pin                       // POWER_UNIT   // max: DESIGN_CAPACITY
        uint16_t    BATTERY_VOLTAGE             = 0x4A38;               // Điện áp hiện tại của pin                         // mV           // Basic voltage: 19V



/*============================================================================================================================================================================
Sub Funsion
============================================================================================================================================================================*/



/*============================================================================================================================================================================
Main Funsion
============================================================================================================================================================================*/
int main()
{
    stdio_init_all();


    // Initialise the Wi-Fi chip
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed\n");
        return -1;
    }


    // // SPI initialisation
    // spi_init(SPI_PORT, 1000*1000);                  // 1MHz
    // gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    // gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    // gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    // gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    // // Chip select is active-low, so we'll initialise it to a driven-high state
    // gpio_set_dir(PIN_CS, GPIO_OUT);
    // gpio_put(PIN_CS, 1);
    // https://github.com/raspberrypi/pico-examples/tree/master/spi


    // I2C Initialisation
    // i2c_init(I2C_PORT, 400*1000);                   // 400Khz
    // gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA);
    // gpio_pull_up(I2C_SCL);
    // https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // SSD1306 oled(i2c0);
    // oled.begin();
    // oled.clearDisplay();

    // // Hiển thị văn bản
    // oled.printText(0, 0, "Hello, OLED!", true);
    // oled.display();
    
    
    
    
    
    

    // // Set up our UART
    // uart_init(UART_ID, BAUD_RATE);
    // // Set the TX and RX pins by using the function select on the GPIO
    // // Set datasheet for more information on function select
    // gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    // gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    // Send out a string, with CR/LF conversions
    // uart_puts(UART_ID, " Hello, UART!\n");
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart


    while (true)
    {
        tight_loop_contents();
        // Pico W LED
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(1000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(1000);
    }
}
