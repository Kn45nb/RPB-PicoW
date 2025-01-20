/*============================================================================================================================================================================
#include    lib 
============================================================================================================================================================================*/
#include    <stdio.h>
#include    <stdint.h>

// SDK Pico 2.0.0
#include    "pico/stdlib.h"
#include    "hardware/spi.h"
#include    "hardware/i2c.h"
#include    "pico/cyw43_arch.h"
#include    "hardware/uart.h"

// Other Lib


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
#define     SSD1306_I2C_ADDR                0x3C            // Địa chỉ I2C của SSD1306
#define     SCREEN_WIDTH                    128
#define     SCREEN_HEIGHT                   64

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
Const Type        Var                       Val             Mô tả                                               Đơn vị      Note
============================================================================================================================================================================*/
        uint16_t    batteryStatus           = 0x0004;       // Trạng thái pin                                   N/a         Bắt buộc
                                                                                                                            // 0x0001: Sạc
                                                                                                                            // 0x0002: Xả
                                                                                                                            // 0x0004: Full
                                                                                                                            // 0x0008: Low
                                                                                                                            // 0x0010: Error
        uint16_t    remainingCapacity       = 900;          // Dung lượng còn lại của pin                       mAh
        uint16_t    voltage                 = 19000;        // Điện áp hiện tại của pin                         mV
        uint16_t    current                 = 7000;         // Dòng điện pin đang cấp hoặc nhận                 mA          Âm sạc, dương xả
const   uint16_t    designCapacity          = 1000;         // Dung lượng thiết kế của pin                      mAh         Không đổi trong vòng đời Cell
        uint16_t    fullChargeCapacity      = 950;          // Dung lượng thực khi sạc đầy                      mAh         
        uint16_t    cycleCount              = 10;           // Số chu kỳ sạc-xả                                 Times
        uint16_t    temperature             = 2980;         // Nhiệt độ hiện tại của pin                        Kelvin
        uint8_t     relativeSOC             = 75;           // Dung lượng còn lại với dung lượng thực           %           =remainingCapacity/fullChargeCapacity to 8bit
        uint8_t     absoluteSOC             = 70;           // Dung lượng còn lại với dung lượng thiết kế       %           =remainingCapacity/designCapacity to 8 bit
        uint16_t    avgTimeToEmpty          = 90;           // Thời gian trung bình cho đến khi pin cạn         Minutes     chỉ áp dụng khi đang dùng   |
        uint16_t    avgTimeToFull           = 120;          // Thời gian trung bình để sạc đầy                  Minutes     Chỉ áp dụng khi đang sạc    |mạch sạc xả cùng chân
        uint16_t    chargingCurrent         = 2000;         // Dòng điện sạc hiện tại                           mA
        uint16_t    chargingVoltage         = 12500;        // Điện áp sạc hiện tại                             mV
const   char        manufacturerName[]      = "Notebook";   // Tên nhà sản xuất pin                             N/a
const   char        deviceName[]            = "BAT";        // Tên thiết bị pin                                 N/a
const   char        serialNumber[]          = "0001";       // Số serial duy nhất của pin                       N/a
        uint16_t    alarmCapacity           = 500;          // Dung lượng ngưỡng cảnh báo pin yếu               mAh         Tùy thuộc vào Cell
        uint8_t     currentCommand          = 0x00;         // Lưu lệnh hiện tại từ Motherboard IMB410TN        N/a



/*============================================================================================================================================================================
Sub Funsion
============================================================================================================================================================================*/
// Lệnh khởi tạo SSD1306
const uint8_t ssd1306_init_cmds[] = 
{
    0xAE,       // Display OFF
    0xD5, 0x80, // Set Display Clock Div
    0xA8, 0x3F, // Set Multiplex Ratio (1/64)
    0xD3, 0x00, // Set Display Offset
    0x40,       // Set Start Line Address
    0x8D, 0x14, // Enable charge pump
    0x20, 0x00, // Set Memory Addressing Mode (Horizontal addressing mode)
    0xA1,       // Set Segment Re-map (mirror horizontally)
    0xC8,       // Set COM Output Scan Direction (mirror vertically)
    0xDA, 0x12, // Set COM Pins Hardware Configuration
    0x81, 0xCF, // Set Contrast Control
    0xD9, 0xF1, // Set Pre-charge Period
    0xDB, 0x40, // Set VCOMH Deselect Level
    0xA4,       // Disable Entire Display On
    0xA6,       // Set Normal Display
    0xAF        // Display ON
};

// Hàm gửi lệnh tới SSD1306
void ssd1306_send_command(uint8_t cmd)
{
    uint8_t buffer[2] = {0x00, cmd};
    i2c_write_blocking(I2C_PORT, SSD1306_I2C_ADDR, buffer, 2, false);
}

// Hàm gửi dữ liệu tới SSD1306
void ssd1306_send_data(uint8_t *data, size_t len)
{
    uint8_t buffer[len + 1];
    buffer[0] = 0x40; // Byte đầu tiên là chỉ báo dữ liệu
    memcpy(&buffer[1], data, len);
    i2c_write_blocking(I2C_PORT, SSD1306_I2C_ADDR, buffer, len + 1, false);
}

// Hàm khởi tạo SSD1306
void ssd1306_init()
{
    for (size_t i = 0; i < sizeof(ssd1306_init_cmds); i++) {
        ssd1306_send_command(ssd1306_init_cmds[i]);
    }
}

// Hàm làm sạch màn hình SSD1306
void ssd1306_clear()
{
    uint8_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT / 8] = {0};
    for (uint8_t page = 0; page < 8; page++) {
        ssd1306_send_command(0xB0 + page); // Set Page Start Address
        ssd1306_send_command(0x00);       // Set Lower Column Start Address
        ssd1306_send_command(0x10);       // Set Higher Column Start Address
        ssd1306_send_data(buffer, SCREEN_WIDTH);
    }
}

// Hàm hiển thị chuỗi ký tự trên SSD1306
void ssd1306_display_text(const char *text)
{
    // Ví dụ hiển thị dữ liệu cứng nhắc. Muốn mềm dẻo hơn cần thư viện GFX.
    uint8_t text_data[SCREEN_WIDTH] = {0xFF, 0x81, 0x81, 0x81, 0xFF}; // Khung ký tự "A".
    ssd1306_send_command(0xB0);  // Page Start Address
    ssd1306_send_command(0x00);  // Lower Column Start Address
    ssd1306_send_command(0x10);  // Higher Column Start Address
    ssd1306_send_data(text_data, sizeof(text_data));
}



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

    // SPI initialisation
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
    // Khởi tạo SSD1306
    ssd1306_init();
    ssd1306_clear();

    // Hiển thị "Hello"
    ssd1306_display_text("Hello");

    while (true)
    {
        tight_loop_contents();
        // Example to turn on the Pico W LED
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }

    return 0;
    
    
    
    
    
    

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

    // while (true)
    // {

    //     printf("Hello, world!\n");
        
    //     sleep_ms(1000);
    // }
}
