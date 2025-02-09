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
// Base
#include	<stdio.h>
#include	<stdint.h>


// SDK Pico
#include	"pico/stdlib.h"

// SPI
#include	"hardware/spi.h"

// I2C
#include	"hardware/i2c.h"

// Wiless 
#include	"pico/cyw43_arch.h"

// UART
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


// I2C_0 defines
#define     I2C_PORT_0                      i2c0
#define     I2C_SDA                         8
#define     I2C_SCL                         9
#define     I2C_SLAVE_ADDR                  0xB            // SMBus Host Slave Interface: 0x8, Smart Battery Charger/Charger Selector or Charger System Manager: 0x9, Smart Battery System Manager or Smart Battery Selector: 0xA, Smart Battery: 0xB

// I2C_1 defines
#define     I2C_PORT_1                      i2c1
#define     I2C_SDA                         12
#define     I2C_SCL                         13



// // UART defines (default UART 'uart0')
// #define     UART_ID                         uart1
// #define     BAUD_RATE                       115200
// // PIN
// #define     UART_TX_PIN                     4
// #define     UART_RX_PIN                     5




/*============================================================================================================================================================================
Const   Type        Var                           Val                     Mô tả                                               Đơn vị          Note
============================================================================================================================================================================*/
// Commant of Master
        uint32_t    CMD                         = 0x0;                  // Command of Master                                 // N/a          //


// Package: _BIX
const   uint32_t    REVISION                    = 0x0;                  // Version of the data structure _BIX               // N/a          // Basic: 0x0 (?const)
const   uint32_t    POWER_UNIT                  = 0x0;                  // Biến giá trị đơn vị                              // N/a          // 0: mWh, 1: mAh
const   uint32_t    DESIGN_CAPACITY             = 0x186A0;              // Dung lượng thiết kế của pin                      // POWER_UNIT   // !set: 0x0 || 0xFFFFFFFF
        uint32_t    LAST_FULL_CHARGE_CAPACITY   = 0x186A0;              // Dung lượng sạc đầy cuối cùng của pin             // POWER_UNIT   // !set: 0x0 || 0xFFFFFFFF, max: DESIGN_CAPACITY, @Kn45nb Cần một bộ logic i++, khởi tạo là DESIGN_CAPACITY trừ dần mỗi lần time full.
const   uint32_t    BATTERY_TECHNOLOGY          = 0x1;                  // Công nghệ tái tạo pin (khả năng sạc)             // N/a          // 0: No, 1: Yes
const   uint32_t    DESIGN_VOLTAGE              = 0x4A38;               // Điện áp thiết kế của pin                         // mV           // !set: 0x0 || 0xFFFFFFFF, Basic voltage: 19V/12V
        uint32_t    DESIGN_CAPACITY_OF_WARNING  = 0x2710;               // Dung lượng cảnh báo thiết kế của pin             // POWER_UNIT   // Windows ignores this value (Nhưng Bios thì rất hay đọc)
        uint32_t    DESIGN_CAPACITY_OF_LOW      = 0x1388;               // Dung lượng thấp thiết kế của pin (hibernation)   // POWER_UNIT   // 0-5% of LAST_FULL_CHARGE_CAPACITY, @Kn45nb Cần fun hoặc logic cập nhật sau khi update LAST_FULL_CHARGE_CAPACITY
const   uint32_t    CAPACITY_GRANULARITY_1      = 0x1;                  // Độ tinh mịch (phân giải) dung lượng trên ngưỡng  // POWER_UNIT   // <=1% of DESIGN_CAPACITY
const   uint32_t    CAPACITY_GRANULARITY_2      = 0x1;                  // Độ tinh mịch (phân giải) dung lượng dưới ngưỡng  // POWER_UNIT   // <=75mW, (exp: 0.25% of 25000mWh)
        uint32_t    CYCLE_COUNT                 = 0x0;                  // Số lần sạc của pin                               // Times        // !set: 0xFFFFFFFF
/*cont*/uint32_t    MEASUREMENT_ACCURACY        = 0x1;                  // Độ chính xác đo lường                            // PPM          // >95000 (95%), 1%=1000
/*cont*/uint32_t    MAX_SAMPLING_TIME           = 0x1;                  // Thời gian lấy mẫu tối đa của _BST                // ms           //
/*cont*/uint32_t    MIN_SAMPLING_TIME           = 0x1;                  // Thời gian lấy mẫu tối thiểu của _BST             // ms           //
/*cont*/uint32_t    MAX_AVERAGING_INTERVAL      = 0x1;                  // Thời gian trung bình max của cảm biến PIN        // ms           //
/*cont*/uint32_t    MIN_AVERAGING_INTERVAL      = 0x1;                  // Thời gian trung bình min của cảm biến PIN        // ms           //
const   char        MODEL_NUMBER[]              = "Notebook";           // Mã số model của pin                              // N/a          // !NULL
const   char        SERIAL_NUMBER[]             = "0001";               // Số serial duy nhất của pin                       // N/a          // !NULL
const   char        BATTEY_TYPE[]               = "LION";               // Loại pin                                         // N/a          //
const   char        OEM_INFORMATION[]           = "MAKE BY @Kn45nb";    // Thông tin OEM                                    // N/a          //


// Package: _BST
        uint32_t    BATTERY_STATE               = 0x1;                  // Trạng thái pin                                   // N/a          // 0x0000: Charging, 0x0001: Discharging, ...
        uint32_t    BATTERY_PRESENT_RATE        = 0x1;                  // Tốc độ Sạc/xả (Điện áp giữa 2 đầu nguồn điện)    // POWER_UNIT   // !set: 0x0 || 0xFFFFFFFF, (Âm sạc, dương xả) @Kn45nb $Do that
        uint32_t    BATTEY_REMAINING_CAPACITY   = 0x1;                  // Dung lượng còn lại của pin                       // POWER_UNIT   // !set: 0x0 || 0xFFFFFFFF
        uint32_t    BATTERY_VOLTAGE             = 0x4A38;               // Điện áp hiện tại của pin                         // mV           // 
        // Note: Có thể sử dụng các công cụ như Microsoft ASL Compiler để kiểm tra tính tuân thủ của các bảng ACPI. @Kn45nb


// Package: _BIF (old version for Legacy. Don't carefull that 🥲)


// Package: _BTP (Checkpoint charging using cơ chế Điểm ngắt pin. Cần dùng trong powercfg/batteryreport @Kn45nb)


// Package: _SUN (Option: Số đơn vị khe cắm, nếu chơi thứ tự pin thì không cần @Kn45nb)


// Package: _STA (Sau làm Funsion Shutdows PSU/PIN thì mới động tới @Kn45nb)



/*============================================================================================================================================================================
Methor
============================================================================================================================================================================*/
void _BST()
{

}


void _BCT()     // Trả về thời gian sạc ước tính của pin
{

}

void _BIF()     //Trả về thông tin tĩnh về pin (nói cách khác, số kiểu máy, số sê-ri, điện áp thiết kế, v.v.)
{

}

void _BIX()     // Trả về thông tin tĩnh mở rộng về pin (nói cách khác, số kiểu máy, số sê-ri, điện áp thiết kế, v.v.)
{

}

void _BMA()     // Đặt khoảng thời gian trung bình của phép đo dung lượng pin, tính bằng mili giây
{

}

void _BMC()     // Kiểm soát hiệu chuẩn và sạc
{

}

void _BMD()     // Trả về thông tin pin liên quan đến hiệu chuẩn lại pin và điều khiển sạc
{

}

void _BMS()     // Đặt sampthời gian đo dung lượng pin, tính bằng mili giây
{

}

void _BPC()     // Trả về các biến tĩnh được liên kết với các đặc tính nguồn hệ thống trên đường dẫn của pin và cài đặt hỗ trợ ngưỡng nguồn
{

}

void _BPS()     // Trả về khả năng cung cấp năng lượng của pin tại thời điểm hiện tại
{

}

void _BPT()     // Phương pháp điều khiển để đặt Ngưỡng nguồn pin
{

}

void _GMT()     // Trả về trạng thái pin hiện tại (nói cách khác, thông tin động về pin, chẳng hạn như pin hiện đang sạc hay xả, ước tính dung lượng pin còn lại, v.v.)
{

}

void _BTH()     // Trả về trạng thái pin hiện tại (nói cách khác, thông tin động về pin, chẳng hạn như pin hiện đang sạc hay xả, ước tính dung lượng pin còn lại, v.v.)
{

}

void _BTM()     // Trả về thời gian chạy ước tính của pin ở tốc độ tiêu hao trung bình hiện tại hoặc thời gian chạy ở một tốc độ cụ thể
{

}

void _BTP()     // Đặt điểm Ngắt pin, điểm này tạo ra SCI khi dung lượng pin đạt đến điểm được chỉ định
{

}

void _OSC()     // Khả năng vận chuyển OSPM cho pin
{

}

void _PCL()     // Danh sách các con trỏ đến các đối tượng thiết bị đại diện cho các thiết bị chạy bằng pin (https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/10_Power_Source_and_Power_Meter_Devices/Power_Source_and_Power_Meter_Devices.html#pcl-power-consumer-list)
{

}

void _STA()     // Trả về trạng thái chung của pin (https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/06_Device_Configuration/Device_Configuration.html#sta-device-status)
{

}



/*============================================================================================================================================================================
Sub-Funsion
============================================================================================================================================================================*/
void blink(uint16_t TIME_BLINK)
{
    tight_loop_contents();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(TIME_BLINK);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(TIME_BLINK);
}


/*============================================================================================================================================================================
Main Funsion
============================================================================================================================================================================*/
int main()
{
    stdio_init_all();

    if (cyw43_arch_init())
    {
        // printf("Wi-Fi init failed\n");
        return -1;
    }

    i2c_init(I2C_PORT_0, 100*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);



    // https://github.com/raspberrypi/pico-examples/tree/master/i2c

    
    
    
    
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


    blink(1000);
}
