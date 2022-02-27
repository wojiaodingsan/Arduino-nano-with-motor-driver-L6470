/*
 * L6470.h
 *
 *  Created on: Jun 1, 2021
 *  Author: Deng Hongchao
 *  Based on "SPI_CON.h" from Dr. Sukekawa
 */

#ifndef L6470_CON_H_
#define L6470_CON_H_

#include<Arduino.h> // For access to hardware pins
#include<HardwareSerial.h> // for Serial Logging
#include<SPI.h>

//#define CHAIN_LENGTH 2
#define SLAVE_NUM 2

void STEPPING_PUMP_INIT(byte dev_id);
void STEPPING_PUMP_SETUP(byte dev_id);

// Registers map
// --- These addresses can be used as 'param' for setparam() and getparam() functions.
#define ABS_POS		0x01
#define EL_POS		0x02
#define MARK        0x03
#define SPEED       0x04
#define ACCELERATE  0x05
#define DECELERATE  0x06
#define MAX_SPEED   0x07
#define MIN_SPEED   0x08
#define FS_SPD      0x15
#define KVAL_HOLD   0x09
#define KVAL_RUN    0x0A
#define KVAL_ACC    0x0B
#define KVAL_DEC    0x0C
#define INT_SPD     0x0D
#define ST_SLP      0x0E
#define FN_SLP_ACC  0x0F
#define FN_SLP_DEC  0x10
#define K_THERM     0x11
#define ADC_OUT     0x12
#define OCD_TH      0x13
#define STALL_TH    0x14
#define STEP_MODE   0x16
#define ALARM_EN    0x17
#define CONFIG      0x18
#define STATUS      0x19

// Application commands
#define NOP          0x00
#define SETPARAM     0x00
#define GETPARAM     0x20
#define RUN          0x50
#define STEPCLOCK    0x58
#define MOVE         0x40
#define GOTO         0x60
#define GOTO_DIR     0x68
#define GOUNTIL      0x82
#define RELEASESW    0x92
#define GOHOME       0x70
#define GOMARK       0x78
#define RESETPOS     0xD8
#define RESETDEVICE  0xC0
#define SOFTSTOP     0xB0
#define HARDSTOP     0xB8
#define SOFTHIZ      0xA0
#define HARDHIZ      0xA8
#define GETSTATUS    0xD0

// Constants for 'dir'
#define FWD  0x01
#define REV  0x00

// Constants for 'act'
#define ACT_RESET  0x00
#define ACT_COPY   0x01

// EL_POS register
#define EL_POS_STEP_MSK       0xC0
#define EL_POS_MICROSTEP_MSK  0x3F

// MIN_SPEED register
#define MIN_SPEED_LSPD_OPT_MSK   0x1000
#define MIN_SPEED_MIN_SPEED_MSK  0x0FFF

// STEP_MODE register
#define STEP_MODE_SYNC_EN_MSK  0x80
#define SYNC_EN  0x80

#define STEP_MODE_SYNC_SEL_MSK  0x70
#define SYNC_SEL_1_2  0x00
#define SYNC_SEL_1    0x10
#define SYNC_SEL_2    0x20
#define SYNC_SEL_4    0x30
#define SYNC_SEL_8    0x40
#define SYNC_SEL_16   0x50
#define SYNC_SEL_32   0x60
#define SYNC_SEL_64   0x70

#define STEP_MODE_STEP_SEL_MSK  0x07
#define STEP_SEL_1    0x00
#define STEP_SEL_2    0x01
#define STEP_SEL_4    0x02
#define STEP_SEL_8    0x03
#define STEP_SEL_16   0x04
#define STEP_SEL_32   0x05
#define STEP_SEL_64   0x06
#define STEP_SEL_128  0x07

// ALARM_EN register
#define ALARM_EN_OVERCURRENT        0x01
#define ALARM_EN_THERMAL_SHUTDOWN   0x02
#define ALARM_EN_THERMAL_WARNING    0x04
#define ALARM_EN_UNDER_VOLTAGE      0x08
#define ALARM_EN_STALL_DETECT_A     0x10
#define ALARM_EN_STALL_DETECT_B     0x20
#define ALARM_EN_SWITCH_TURN_ON     0x40
#define ALARM_EN_WRONG_NOTPERF_CMD  0x80

// CONFIG register
#define CONFIG_OSC_SEL_MSK  0x0007
#define CONFIG_EXT_CLK_MSK  0x0008

#define CONFIG_SW_MODE_MSK  0x0010
#define SW_MODE_HARDSTOP  0x0000
#define SW_MODE_USER      0x0010

#define CONFIG_EN_VSCOMP_MSK  0x0020
#define EN_VSCOMP_DISABLED  0x0000
#define EN_VSCOMP_ENABLED   0x0020

#define CONFIG_OC_SD_MSK  0x0080
#define OC_SD_BRIDGE_SHUT      0x0080
#define OC_SD_BRIDGE_NOT_SHUT  0x0000

#define CONFIG_POW_SR_MSK  0x0300
#define POW_SR_180  0x0000
#define POW_SR_290  0x0200
#define POW_SR_530  0x0300

#define CONFIG_F_PWM_DEC_MSK  0x1C00
#define F_PWM_DEC_0_625  0x0000
#define F_PWM_DEC_0_75   0x0400
#define F_PWM_DEC_0_875  0x0800
#define F_PWM_DEC_1      0x0C00
#define F_PWM_DEC_1_25   0x1000
#define F_PWM_DEC_1_5    0x1400
#define F_PWM_DEC_1_75   0x1800
#define F_PWM_DEC_2      0x1C00

#define CONFIG_F_PWM_INT_MSK  0xE000
#define F_PWM_INT_1  0x0000
#define F_PWM_INT_2  0x2000
#define F_PWM_INT_3  0x4000
#define F_PWM_INT_4  0x6000
#define F_PWM_INT_5  0x8000
#define F_PWM_INT_6  0xA000
#define F_PWM_INT_7  0xC000

// STATUS register
#define STATUS_HIZ_MSK     0x0001
#define STATUS_BUSY_MSK    0x0002
#define STATUS_SW_F_MSK    0x0004
#define STATUS_SW_EVN_MSK  0x0008
#define STATUS_DIR_MSK     0x0010

#define STATUS_MOT_STATUS_MSK  0x0060 // the status is bit5 and 6, so the MOT_STATUS_STOP to MOT_STATUS_CONST_SPD were wrong 
#define MOT_STATUS_STOP       0x0000
#define MOT_STATUS_ACC        0x0020
#define MOT_STATUS_DEC        0x0040
#define MOT_STATUS_CONST_SPD  0x0060

#define STATUS_NOTPERF_CMD_MSK  0x0080
#define STATUS_WRONG_CMD_MSK    0x0100
#define STATUS_UVLO_MSK         0x0200
#define STATUS_TH_WRN_MSK       0x0400
#define STATUS_TH_SD_MSK        0x0800
#define STATUS_OCD_MSK          0x1000
#define STATUS_STEP_LOSS_A_MSK  0x2000
#define STATUS_STEP_LOSS_B_MSK  0x4000
#define STATUS_SCK_MOD_MSK      0x8000

// Multipliers
#define TO_SPEED      67.1089     // [step/s] to [step/tick] for SPEED register
#define TO_ACC         0.0687195  // [step/s2] to [step/tick2] for ACC register
#define TO_DEC         0.0687195  // [step/s2] to [step/tick2] for DEC register
#define TO_MAX_SPEED   0.0655360  // [step/s] to [step/tick] for MAX_SPEED register
#define TO_MIN_SPEED   4.19430    // [step/s] to [step/tick] for MIN_SPEED register
#define TO_FS_SPD    0.0655360  // [step/s] to [step/tick] for FS_SPED register
#define TO_INT_SPD   4.19430    // [step/s] to [step/tick] for INT_SPEED register

// *** You MUST specify the base address of SPI by this function before any further processes.
void L6470_init(byte dev_id);

void L6470_read(byte dev_id, byte length, byte *data);         // Read one byte data
void L6470_write(byte dev_id, byte length, const byte *data);  // Write one byte data

int L6470_isbusy(byte dev_id);      // 1 if system is in busy state. access through busy pin
int L6470_isbusy2(byte dev_id);     // 1 if system is in busy state. access through SPI, status
int L6470_iserrorcmd(byte dev_id);  // 1 if WRONG_CMD or NOTPERF_CMD bits are high.
int L6470_ishiz(byte dev_id);       // 1 if the bridges are in high impedance state.
int L6470_ismotorrunning(byte dev_id);  // 1 if motor is running, i.e. busy or motor status is not stopped.

void L6470_nop(byte dev_id);
int L6470_setparam(byte dev_id, byte param, unsigned long value);
unsigned long L6470_getparam(byte dev_id, byte param);
void L6470_run(byte dev_id, byte dir, unsigned long speed);     // dir={FWD,REV}, speed in step/tick
void L6470_run_step(byte dev_id, byte dir, unsigned long step_s); // dir={FWD,REV}, step_s*TO_SPEED converted to step/tick
void L6470_stepclock(byte dev_id, byte dir);             // dir={FWD,REV}
void L6470_move(byte dev_id, byte dir, unsigned long n_step);  // dir={FWD,REV}, n_step=number of microsteps (in agreement with STEP_MODE)
void L6470_goto(byte dev_id, unsigned long abs_pos);             // abs_pos is in agreement with STEP_MODE
void L6470_goto_dir(byte dev_id, byte dir, unsigned long abs_pos);          // dir={FWD,REV}, abs_pos is in agreement with STEP_MODE
void L6470_gountil(byte dev_id, byte act, byte dir, float step_s);  // dir={FWD,REV}, step_s=speed in full step/s
void L6470_releasesw(byte dev_id, byte act, byte dir);  // dir={FWD,REV}
void L6470_gohome(byte dev_id);
void L6470_gomark(byte dev_id);
void L6470_resetpos(byte dev_id);
void L6470_resetdevice(byte dev_id);
void L6470_softstop(byte dev_id);
void L6470_hardstop(byte dev_id);
void L6470_softhiz(byte dev_id);
void L6470_hardhiz(byte dev_id);
unsigned long L6470_getstatus(byte dev_id);

// Conversion functions
long to_speed(float step_s);
long to_acc(float step_s2);
long to_dec(float step_s2);
long to_max_speed(float step_s);
long to_min_speed(float step_s);
long to_fs_spd(float step_s);
long to_int_speed(float step_s);


// Setting important parameters

void SetACC(byte dev_id, unsigned long value);
void SetDEC(byte dev_id, unsigned long value);
void SetMAXSPEED(byte dev_id, unsigned long value);
void SetMINSPEED(byte dev_id, unsigned long value);
void SetFSSPD(byte dev_id, unsigned long value);


void SetACC_step(byte dev_id, unsigned long value);
void SetDEC_step(byte dev_id, unsigned long value);
void SetMAXSPEED_step(byte dev_id, unsigned long value);
void SetMINSPEED_step(byte dev_id, unsigned long value);
void SetFSSPD_step(byte dev_id, unsigned long value);

void SetSTEPMODE(byte dev_id, byte value); // set the step_mode, only lower 3 bit is useful

#endif /* SPI_CON_H_ */
