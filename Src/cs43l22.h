
/* STM32F407G-Discovery board 
 * Audio_SDA PB9  -> SDA
 * Audio_SCL PB6  -> SCL
 * I2S_MCK   PC7  -> MCLK
 * I2S_SCK   PC10 -> SCLK
 * I2S_SD    PC12 -> SDIN
 * I2S_WS    PA4  -> LRCK
 * Audio_RST PD4  -> !RESET
 * 
 * I2C Address 0x94
 *
 */

#include "i2c.h"
#include "gpio.h"

#define CODEC_I2C_INSTANCE      hi2c1.Instance
#define CODEC_FLAG_TIMEOUT      ((uint32_t)0x1000)
#define CODEC_BOARD_AD0_PIN     0
#define CODEC_I2C_ADDRESS       0x94
#define CODEC_I2C_WRITE(x,y)    HAL_I2C_Master_Transmit(&hi2c1, CODEC_I2C_ADDRESS, x, y, HAL_MAX_DELAY) // x: ptr to buffer, y: size
#define CODEC_I2C_READ(x,y)     HAL_I2C_Master_Receive(&hi2c1, CODEC_I2C_ADDRESS, x, y, HAL_MAX_DELAY);

typedef struct {
  uint8_t reserbed_0[1];
  uint8_t chip_id_reg;        // Chip id and revision. Addr 01h (read only)
  uint8_t power_ctrl1_reg;    // Power control 1. Addr 02h
  uint8_t reserved;     
  uint8_t power_ctrl2_reg;    // Power control 2. Addr 04h
  uint8_t clk_ctrl_reg;       // Clocking control. Addr 05h
  uint8_t iface_ctrl1_reg;    // Interface control 1. Addr 06h
  uint8_t iface_ctrl2_reg;    // Interface control 2. Addr 07h
  uint8_t pass_a_reg;         // Passthrough a. Addr 08h
  uint8_t pass_b_reg;         // Passthrough b. Addr 09h
  uint8_t zc_sr_set_reg;      // Analog ZC and SR settings. Addr 0Ah
  uint8_t reserved_1[1];
  uint8_t pass_gang_ctrl_reg; // Passthrough gang control. Addr 0Ch
  uint8_t pbck_ctrl1_reg;     // Playback control 1. Addr 0Dh
  uint8_t misc_ctrl_reg;      // Misc. controls. Addr 0Eh
  uint8_t pbck_ctrl2_reg;     // Playback control 1. Addr 0Fh
  uint8_t reserved_2[4];
  uint8_t pass_a_vol_reg;     // Passthrough a volume. Addr 14h
  uint8_t pass_b_vol_reg;     // Passthrough b volume. Addr 15h
  uint8_t reserved_3[4];
  uint8_t pcm_a_vol_reg;      // PCM a volume. Addr 1Ah
  uint8_t pcm_b_vol_reg;      // PCM b volume. Addr 1Bh
  uint8_t beep_freq_reg;      // Beep frequency and on time. Addr 1Ch
  uint8_t beep_vol_reg;       // Beep volume and off time. Addr 1Dh
  uint8_t beep_tone_config_reg;       // Beep and tone configuration. Addr 1Eh
  uint8_t tone_ctrl_reg;              // Tone control. Addr 1Fh
  uint8_t master_vol_a_reg;           // Master volume control a. Addr 20h
  uint8_t master_vol_b_reg;           // Master volume control b. Addr 21ha
  uint8_t headphone_vol_ctrl_a_reg;   // Headphone volume control a. Addr 22h
  uint8_t headphone_vol_ctrl_b_reg;   // Headphone volume control b. Addr 23h
  uint8_t speaker_vol_ctrl_a_reg;     // Speaker volume control a. Addr 24h
  uint8_t speaker_vol_ctrl_b_reg;     // Speaker volume control b. Addr 25h
  uint8_t pcm_ch_swap_reg;            // PCM Channel swap. Addr 26h
  uint8_t lim_ctrl1_reg;              // Limiter control 1, min max thresholds. Addr 27h
  uint8_t lim_ctrl2_reg;              // Limiter control 2, release rate. Addr 28h
  uint8_t lim_atk_rate_reg;           // Limiter attack rate. Addr 29h
  uint8_t reserved_4[4];
  uint8_t status_reg;                 // Status register. Addr 2Eh (read only)
  uint8_t battery_comp_reg;           // Battery compensation. Addr 2Fh
  uint8_t vp_battery_lvl_reg;         // VP Battery level. Addr 30h (read only)
  uint8_t speaker_status_reg;         // Speaker status. Addr 31h (read only)
  uint8_t reserved_5[2];
  uint8_t chg_pump_freq_reg;          // Charge pump frequency. Addr 34h
  // TODO // get the rest of the regs
  // TODO // align structure to how regs are laid out on hw? or have pointers?

} CS43L22_codec_regs_t;

void codec_init();
void codec_deinit();
void codec_generate_beep();
void codec_reset();
uint8_t codec_read_register(uint8_t register_addr);
void codec_write_register(uint8_t register_addr, uint8_t data);
void codec_read_all_registers();

extern volatile CS43L22_codec_regs_t codec_regs;
