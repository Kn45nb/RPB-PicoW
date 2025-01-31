/*============================================================================================================================================================================
Information
==============================================================================================================================================================================
SDK: 		v2.1.0
Toolchain:	13.3Rel1
Ninja:		v1.12.1
CMake:		v3.31.4
============================================================================================================================================================================*/



/*============================================================================================================================================================================
#include	Lib
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


// I2C defines (Default 400KHz)
#define     I2C_PORT                        i2c0
#define     I2C_SDA                         8
#define     I2C_SCL                         9
// SSD1306 defines
#define     SCREEN_WIDTH                    128
#define     SCREEN_HEIGHT                   64
#define     OLED_RESET                      -1
#define     SCREEN_ADDRESS                  0x3C



// UART defines (default UART 'uart0')
#define     UART_ID                         uart1
#define     BAUD_RATE                       115200
// PIN
#define     UART_TX_PIN                     4
#define     UART_RX_PIN                     5


// ACPI 5.0 defines
#define     CMD_BATTERY_STATUS              0x16
#define     CMD_REMAINING_CAPACITY          0x0F
#define     CMD_VOLTAGE                     0x09
#define     CMD_CURRENT                     0x0A
#define     CMD_DESIGN_CAPACITY             0x18
#define     CMD_FULL_CHARGE_CAPACITY        0x10
#define     CMD_CYCLE_COUNT                 0x17
#define     CMD_TEMPERATURE                 0x08
#define     CMD_RELATIVE_SOC                0x0D
#define     CMD_ABSOLUTE_SOC                0x0E
#define     CMD_AVG_TIME_TO_EMPTY           0x12
#define     CMD_AVG_TIME_TO_FULL            0x13
#define     CMD_CHARGING_CURRENT            0x14
#define     CMD_CHARGING_VOLTAGE            0x15
#define     CMD_MANUFACTURER_NAME           0x20
#define     CMD_DEVICE_NAME                 0x21
#define     CMD_SERIAL_NUMBER               0x1C
#define     CMD_ALARM_CAPACITY              0x1A



/*============================================================================================================================================================================
Const Type        Var                           Val                     Mô tả                                               Đơn vị          Note
============================================================================================================================================================================*/
// _BIX
const   uint8_t     REVISION                    = 0x1;                  // Version of the data structure _BIX               // N/a          // Basic: 0x1
/*cont*/bool        POWER_UNIT                  = 0x1;                  // Bit Logic đơn vị                                 // N/a          // 0: mWh, 1: mAh
const   uint32_t    DESIGN_CAPACITY             = 0x186A0;              // Dung lượng thiết kế của pin                      // POWER_UNIT   // max: 4294967295
        uint32_t    LAST_FULL_CHARGE_CAPACITY   = 0x186A0;              // Dung lượng sạc đầy cuối cùng của pin             // POWER_UNIT   // max: DESIGN_CAPACITY
const   bool        BATTERY_TECHNOLOGY          = 0x1;                  // Loại công nghệ pin (khả năng tái tạo)            // N/a          // 0: No, 1: Yes
const   uint16_t    DESIGN_VOLTAGE              = 0x4A38;               // Điện áp thiết kế của pin                         // mV           // Basic voltage: 19V
        uint32_t    DESIGN_CAPACITY_OF_WARNING  = 0x2710;               // Dung lượng cảnh báo thiết kế của pin             // POWER_UNIT   // max: LAST_FULL_CHARGE_CAPACITY
        uint32_t    DESIGN_CAPACITY_OF_LOW      = 0x2710;               // Dung lượng thấp thiết kế của pin (hibernation)   // POWER_UNIT   // max: LAST_FULL_CHARGE_CAPACITY
        uint8_t     CYCLE_COUNT                 = 0x0;                  // Số lần sạc của pin                               // Times        // max: 255
/*cont*/uint32_t     MEASUREMENT_ACCURACY        = 0x1;                  // Độ chính xác đo lường                           // PPM          // 0: 1%, 1: 0.1% @Kn45nb Cần tối ưu đoạn này


const   char        MANUFACTURER[]          = "Notebook";           // Tên nhà sản xuất pin                             // N/a          // Can change it
const   char        NAME[]                  = "BAT";                // Tên thiết bị pin                                 // N/a          // Can change it
const   char        SERIAL_NUMBER[]         = "0001";               // Số serial duy nhất của pin                       // N/a          // Can change it
const   char        CHEMISTRY[]             = "LION";               // Loại hóa học của pin                             // N/a          // Can change it


// _BST
        uint32_t    CAPACITY_REMAINING      = 0x384;                // Dung lượng còn lại của pin                       mWh         max: 0x186A0
        uint16_t    VOLTAGE_PRESENT         = 0x4E20;               // Điện áp hiện tại của pin                         mV          Basic voltage: 19V out that is warning




        
        
        



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
    i2c_init(I2C_PORT, 400*1000);                   // 400Khz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
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
