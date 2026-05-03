/*=============================================================
Information
===============================================================
Project:    DEMO_2
SDK: 		v2.2.0
Toolchain:	15.2Rel1
Ninja:		v1.13.2
CMake:		v4.3.2
Board:		pico W
Author:		@Kn45nb
Date:		03/05/2026
License:	MIT License
=============================================================*/



/*=============================================================
Include	    Lib
=============================================================*/
#include    <stdio.h>
#include    "hardware/uart.h"
#include    "pico/stdlib.h"
#include    "hardware/spi.h"
#include    "hardware/i2c.h"
#include    "hardware/pio.h"
#include    "hardware/uart.h"
#include    "blink.pio.h"



/*=============================================================
Define  Variables   Values
=============================================================*/
#define SPI_PORT    spi0
#define PIN_MISO    16
#define PIN_CS      17
#define PIN_SCK     18
#define PIN_MOSI    19

#define I2C_PORT    i2c0
#define I2C_SDA     8
#define I2C_SCL     9

#define UART_ID uart1
#define BAUD_RATE   115200
#define UART_TX_PIN 4
#define UART_RX_PIN 5



/*=============================================================
Const/Var   Type        Variables                   Values                  Description                                         Unit            Note
=============================================================*/
// Commant of Master
            uint32_t    CMD                         = 0x0;                  // Command of Master                                // N/a          //
// Package: _BIX
const       uint32_t    REVISION                    = 0x0;                  // Version of the data structure _BIX               // N/a          // Basic: 0x0 (?const)
const       uint32_t    POWER_UNIT                  = 0x0;                  // Biến giá trị đơn vị                              // N/a          // 0: mWh, 1: mAh
const       uint32_t    DESIGN_CAPACITY             = 0x186A0;              // Dung lượng thiết kế của pin                      // POWER_UNIT   // !set: 0x0 || 0xFFFFFFFF
            uint32_t    LAST_FULL_CHARGE_CAPACITY   = 0x186A0;              // Dung lượng sạc đầy cuối cùng của pin             // POWER_UNIT   // !set: 0x0 || 0xFFFFFFFF, max: DESIGN_CAPACITY, @Kn45nb Cần một bộ logic i++, khởi tạo là DESIGN_CAPACITY trừ dần mỗi lần time full.
const       uint32_t    BATTERY_TECHNOLOGY          = 0x1;                  // Công nghệ tái tạo pin (khả năng sạc)             // N/a          // 0: No, 1: Yes
const       uint32_t    DESIGN_VOLTAGE              = 0x4A38;               // Điện áp thiết kế của pin                         // mV           // !set: 0x0 || 0xFFFFFFFF, Basic voltage: 19V/12V
            uint32_t    DESIGN_CAPACITY_OF_WARNING  = 0x2710;               // Dung lượng cảnh báo thiết kế của pin             // POWER_UNIT   // Windows ignores this value (Nhưng Bios thì rất hay đọc)
            uint32_t    DESIGN_CAPACITY_OF_LOW      = 0x1388;               // Dung lượng thấp thiết kế của pin (hibernation)   // POWER_UNIT   // 0-5% of LAST_FULL_CHARGE_CAPACITY, @Kn45nb Cần fun hoặc logic cập nhật sau khi update LAST_FULL_CHARGE_CAPACITY
const       uint32_t    CAPACITY_GRANULARITY_1      = 0x1;                  // Độ tinh mịch (phân giải) dung lượng trên ngưỡng  // POWER_UNIT   // <=1% of DESIGN_CAPACITY
const       uint32_t    CAPACITY_GRANULARITY_2      = 0x1;                  // Độ tinh mịch (phân giải) dung lượng dưới ngưỡng  // POWER_UNIT   // <=75mW, (exp: 0.25% of 25000mWh)
            uint32_t    CYCLE_COUNT                 = 0x0;                  // Số lần sạc của pin                               // Times        // !set: 0xFFFFFFFF
const       uint32_t    MEASUREMENT_ACCURACY        = 0x1;                  // Độ chính xác đo lường                            // PPM          // >95000 (95%), 1%=1000
const       uint32_t    MAX_SAMPLING_TIME           = 0x1;                  // Thời gian lấy mẫu tối đa của _BST                // ms           //
const       uint32_t    MIN_SAMPLING_TIME           = 0x1;                  // Thời gian lấy mẫu tối thiểu của _BST             // ms           //
const       uint32_t    MAX_AVERAGING_INTERVAL      = 0x1;                  // Thời gian trung bình max của cảm biến PIN        // ms           //
const       uint32_t    MIN_AVERAGING_INTERVAL      = 0x1;                  // Thời gian trung bình min của cảm biến PIN        // ms           //
const       char        MODEL_NUMBER[]              = "Notebook";           // Mã số model của pin                              // N/a          // !NULL
const       char        SERIAL_NUMBER[]             = "0001";               // Số serial duy nhất của pin                       // N/a          // !NULL
const       char        BATTEY_TYPE[]               = "LION";               // Loại pin                                         // N/a          //
const       char        OEM_INFORMATION[]           = "MAKE BY @Kn45nb";    // Thông tin OEM                                    // N/a          //


// Package: _BST
            uint32_t    BATTERY_STATE               = 0x1;                  // Trạng thái pin                                   // N/a          // 0x0000: Charging, 0x0001: Discharging, ...
            uint32_t    BATTERY_PRESENT_RATE        = 0x1;                  // Tốc độ Sạc/xả (Điện áp giữa 2 đầu nguồn điện)    // POWER_UNIT   // !set: 0x0 || 0xFFFFFFFF, (Âm sạc, dương xả) @Kn45nb $Do that
            uint32_t    BATTEY_REMAINING_CAPACITY   = 0x1;                  // Dung lượng còn lại của pin                       // POWER_UNIT   // !set: 0x0 || 0xFFFFFFFF
            uint32_t    BATTERY_VOLTAGE             = 0x4A38;               // Điện áp hiện tại của pin                         // mV           // 

// Package: _BIF (old version for Legacy. Don't carefull that 🥲)


// Package: _BTP (Checkpoint charging using cơ chế Điểm ngắt pin. Cần dùng trong powercfg/batteryreport @Kn45nb)


// Package: _SUN (Option: Số đơn vị khe cắm, nếu chơi thứ tự pin thì không cần @Kn45nb)


// Package: _STA (Sau làm Funsion Shutdows PSU/PIN thì mới động tới @Kn45nb)


/*=============================================================
Method
=============================================================*/
void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq)
{
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    printf("Blinking pin %d at %d Hz\n", pin, freq);

    // PIO counter program takes 3 more cycles in total than we pass as
    // input (wait for n + 1; mov; jmp)
    pio->txf[sm] = (125000000 / (2 * freq)) - 3;
}

/*=============================================================
Main
=============================================================*/

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // PIO Blinking example
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &blink_program);
    printf("Loaded program at %d\n", offset);
    
    #ifdef PICO_DEFAULT_LED_PIN
    blink_pin_forever(pio, 0, offset, PICO_DEFAULT_LED_PIN, 3);
    #else
    blink_pin_forever(pio, 0, offset, 6, 3);
    #endif
    // For more pio examples see https://github.com/raspberrypi/pico-examples/tree/master/pio

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    
    // Send out a string, with CR/LF conversions
    uart_puts(UART_ID, " Hello, UART!\n");
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
