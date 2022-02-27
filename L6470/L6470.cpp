/*
 * L6470.c
 *
 *  Created on: Jun 1, 2021
 *  Author: Deng Hongchao
 *  Based on "SPI_CON.c" from Dr. Sukekawa
 */


#include "L6470.h"


void STEPPING_PUMP_INIT(byte dev_id)
{
	L6470_init(dev_id);

	L6470_resetdevice(dev_id); delay(10);

	//L6470_setparam(dev_id, KVAL_HOLD, 0x0F); delay(10); // address 0x09 , motor voltage when stop
	//L6470_setparam(dev_id, KVAL_RUN, 0x0F); delay(10); // address 0x0A , motor voltage when run in a certain speed
	//L6470_setparam(dev_id, KVAL_ACC, 0x0F); delay(10); // address 0x0B , voltage when accelerate
	//L6470_setparam(dev_id, KVAL_DEC, 0x0F); delay(10); // address 0x0C , voltage when decelerate

	L6470_setparam(dev_id, OCD_TH, 0x01); delay(10); // address 0x13, overcurrent threshold 750mA
	L6470_setparam(dev_id, STEP_MODE, 7); delay(10); // address 0x16, STEP_SEL = 111, 1/128 microstep, SYNC_EN is low, no frequency output
	L6470_setparam(dev_id, ACCELERATE, 50); delay(10);
	//L6470_setparam(dev_id, MAX_SPEED, 30); delay(10);

	//unsigned long param_test;
	//Serial.println("finish init");
	//param_test = L6470_getparam(dev_id, KVAL_DEC);
	//Serial.print("KVAL_DEC value is: ");
	//Serial.print(param_test);

	return;
}

void STEPPING_PUMP_SETUP(byte dev_id)
{
	unsigned long cur_pos, cur_mark, status_data;
	L6470_move(dev_id, REV, 1000);

	while (L6470_isbusy(dev_id));
	L6470_gountil(dev_id, 0, FWD, 200);

	while (L6470_isbusy(dev_id));
	L6470_resetpos(dev_id);
	L6470_move(dev_id, REV, 192000);

	while (L6470_isbusy(dev_id));
	cur_pos = L6470_getparam(dev_id, ABS_POS);
	L6470_setparam(dev_id, MARK, cur_pos);
	L6470_hardhiz(dev_id);

	return;
}

// Separate data in each byte of a long-type value, and store in an array in big-endian format
static void tobyte(byte length, unsigned long src, byte* dst) {
	byte i;
	for (i = 0; i < length; i++)
		*(dst + i) = (src >> (length - i - 1) * 8) & 0xFF; // >> right shift, every time 8 bits
	return;
}

// Combine values in an array into a long-type data in big-endian format
static unsigned long tolong(byte length, byte* value) { // static function can't be called in other files (normal function is extern)
	unsigned long ret = 0;
	byte i;
	//Serial.println("tolong start");
	for (i = 0; i < length; i++) {
		//ret |= *(value + i) <<( (length - i - 1) * 8);
		ret = ret << 8;
		ret += *(value + i);
		//ret |= (unsigned long)*(value + i)<<(length - i - 1) * 8;
		//Serial.print("loop ");
		//Serial.print(i);
		//Serial.print(" array value ");
		//Serial.print(*(value + i));
		//Serial.print(" ret added: ");
		//Serial.print(((unsigned long)*(value + i)) << (length - i - 1) * 8);
		//Serial.print(" ret value: ");
		//Serial.println(ret);
	}

	return ret; // copy the data from array value[] to unsigned long ret
}

void L6470_init(byte dev_id) {
	unsigned long status_data;

	status_data = 0;
	status_data = L6470_getstatus(dev_id);
	
	L6470_hardstop(dev_id);  // Exit from hi-Z state, but no motion is performed.
	L6470_resetdevice(dev_id); // Reset the L6470 to power-up conditions.
	status_data = L6470_getstatus(dev_id);  // Reset any warning flags, and exit from any error state.
	L6470_hardstop(dev_id);  // Exit from hi-Z state, but no motion is performed.
	status_data = L6470_getstatus(dev_id);  // Reset any warning flags, and exit from any error state.
	//Serial.print("status_data is: ");
	//Serial.println(status_data,HEX);

	unsigned long configvalue;
	configvalue = L6470_getparam(dev_id, CONFIG);
	//Serial.print("config_data is: ");
	//Serial.println(configvalue, HEX);
}

void L6470_read(byte dev_id, byte length, byte* data) {
	byte i;
	for (i = 0; i < length; i++) {
		digitalWrite(dev_id, LOW);
		data[i] = SPI.transfer(0x00);
		digitalWrite(dev_id, HIGH);
		//Serial.print("loop ");
		//Serial.print(i);
		//Serial.print(" data[i] value ");
		//Serial.println(data[i]);
		//Serial.print(" ret added: ");
		//Serial.print((*(value + i)) * (1 << ((length - i - 1) * 8)));
		//Serial.print(" ret value: ");
		//Serial.println(ret);
	}

}

void L6470_write(byte dev_id, byte length, const byte* data) {
	byte response;
	byte i;
	unsigned long return_code;
	for (i = 0; i < length; i++) {
		digitalWrite(dev_id, LOW);
		response =	SPI.transfer(data[i]);
		digitalWrite(dev_id, HIGH);
		return_code = return_code << i * 8;
		return_code = return_code | response;
	}
}


int L6470_isbusy2(byte dev_id) {
	int busy_flag = digitalRead(dev_id);
	return !busy_flag;
}

int L6470_isbusy(byte dev_id) {
	int busy_flag = L6470_getparam(dev_id, STATUS) & STATUS_BUSY_MSK;
	return !busy_flag;
}

int L6470_iserrorcmd(byte dev_id) {
	//	return L6470_getparam(dev_id, STATUS) &
	unsigned int errorcmd = L6470_getstatus(dev_id) &
		(STATUS_NOTPERF_CMD_MSK | STATUS_WRONG_CMD_MSK);
	return errorcmd;
}

int L6470_ishiz(byte dev_id) {
	return L6470_getparam(dev_id, STATUS) & STATUS_HIZ_MSK;
}

int L6470_ismotorrunning(byte dev_id) {
	unsigned long mot_status = L6470_getparam(dev_id, STATUS) & STATUS_MOT_STATUS_MSK;
	return L6470_isbusy(dev_id) || (mot_status != MOT_STATUS_STOP);
}

void L6470_nop(byte dev_id) {
	byte cmd[] = { NOP };
	L6470_write(dev_id, 1, cmd);
}

int L6470_setparam(byte dev_id, byte param, unsigned long value) {
	byte cmd[] = { SETPARAM | param }, prm[3], n_byte; // define byte cmd[], byte prm[3] and byte n_byte;

	switch (param) { //all address are readable. WH: writeable only high impedance; WS: writable only motor stopped; WR: always writable
		
	case ABS_POS: // 22-bit register(s), WS
		value &= 0x3FFFFF;
		n_byte = 3;
		break;
		
	case EL_POS: // 9-bit register(s), WS
		value &= 0x01FF;  // Masking
		n_byte = 2;
		break;

	case MARK: // 22 bit, WR
		value &= 0x3FFFFF;  // Masking
		n_byte = 3;
		break;

	case ACCELERATE: // 12-bit register(s), WS
		value &= 0x0FFF;
		n_byte = 2;
		break;

	case DECELERATE: // 12-bit, WS
		value &= 0x0FFF;  // Masking
		n_byte = 2;
		break;
		
	case MAX_SPEED: // 10-bit register(s), WR
		value &= 0x03FF;
		n_byte = 2;
		break;
		
	case MIN_SPEED: // 13-bit register(s), WS
		value &= 0x1FFF;  // Masking
		n_byte = 2;
		break;

	case FS_SPD: // 10 bit, WR
		value &= 0x03FF;  // Masking
		n_byte = 2;
		break;

		
	case KVAL_HOLD: // 8-bit register(s), WR
		value &= 0xFF;
		n_byte = 1;
		break;

	case KVAL_RUN: // 8 bit, WR
		value &= 0xFF;
		n_byte = 1;
		break;

	case KVAL_ACC: // 8 bit, WR
		value &= 0xFF;
		n_byte = 1;
		break;

	case KVAL_DEC: // 8bit, WR
		value &= 0xFF;
		n_byte = 1;
		break;

	case INT_SPD: // 14 bit, WH
		value &= 0x03FFF;
		n_byte = 2;
		break;

	case ST_SLP: // 8 bit, WH
		value &= 0xFF;
		n_byte = 1;
		break;

	case FN_SLP_ACC: // 8 bit, WH
		value &= 0xFF;
		n_byte = 1;
		break;

	case FN_SLP_DEC: //8 bit, WH
		value &= 0xFF;
		n_byte = 1;
		break;
	
	case K_THERM: // 4-bit register(s), WR
		value = 0x0F;
		n_byte = 1;
		break;

	case OCD_TH: // 4 bit, WR
		value &= 0x0F;  // Masking
		n_byte = 1;
		break;
		
	case STALL_TH: // 7-bit register(s), WR
		value &= 0x7F;  // Masking
		n_byte = 1;
		break;

	case STEP_MODE: // 8 bit, WH
		value &= 0xFF;
		n_byte = 1;
		break;

	case ALARM_EN: //8 bit, WS
		value &= 0xFF;  // Masking
		n_byte = 1;
		break;
				
	case CONFIG: // 16-bit register(s), WH
		value &= 0xFFFF;  // Masking
		n_byte = 2;
		break;
		
	default: // Undefined register
		return -1;
	}

	while (L6470_isbusy(dev_id)) {
		// Do nothing while BUSY -- keep waiting
	}

	tobyte(n_byte, value, prm);
	L6470_write(dev_id, 1, cmd); // because the input of L6470 function need (byte,byte,byte arr), so he set the cmd to be "byte cmd[]="
	L6470_write(dev_id, n_byte, prm);

	return L6470_iserrorcmd(dev_id);
}

unsigned long L6470_getparam(byte dev_id, byte param) {
	byte cmd[] = { GETPARAM | param }, value[3], n_byte;

	switch (param) {
		// 3-byte register(s)
	case ABS_POS:
		n_byte = 3;
		break;
	case EL_POS:
		n_byte = 2;
		break;
	case MARK:
		n_byte = 3;
		break;
	case SPEED:
		n_byte = 3;
		break;
	case ACCELERATE:// 2-byte register(s)
		n_byte = 2;
		break;
	case DECELERATE:
		n_byte = 2;
		break;
	case MAX_SPEED:
		n_byte = 2;
		break;
	case MIN_SPEED:
		n_byte = 2;
		break;
	case FS_SPD:
		n_byte = 2;
		break;
	case KVAL_HOLD:
		n_byte = 1;
		break;
	case KVAL_RUN:
		n_byte = 1;
		break;
	case KVAL_ACC:
		n_byte = 1;
		break;
	case KVAL_DEC:
		n_byte = 1;
		break;
	case INT_SPD:
		n_byte = 2;
		break;
	case ST_SLP:
		n_byte = 1;
		break;
	case FN_SLP_ACC:
		n_byte = 1;
		break;
	case FN_SLP_DEC:
		n_byte = 1;
		break;
	case K_THERM:
		n_byte = 1;
		break;
	case ADC_OUT:
		n_byte = 1;
		break;
	case OCD_TH:
		n_byte = 1;
		break;
	case STALL_TH:
		n_byte = 1;
		break;
	case STEP_MODE:
		n_byte = 1;
		break;
	case ALARM_EN:
		n_byte = 1;
		break;
	case CONFIG:
		n_byte = 2;
		break;
	case STATUS:
		n_byte = 2;
		break;
	default: // Undefined register
		n_byte = 1;
		break;
	}

	L6470_write(dev_id, 1, cmd);
	L6470_read(dev_id, n_byte, value);
	return tolong(n_byte, value);
}

void L6470_run(byte dev_id, byte dir, unsigned long speed) {
	byte cmd[] = { RUN | dir }, prm[3];
	tobyte(3, speed, prm);
	L6470_write(dev_id, 1, cmd);
	L6470_write(dev_id, 3, prm);
}

void L6470_run_step(byte dev_id, byte dir, unsigned long step_s) {
	byte cmd[] = { RUN | dir }, prm[3];
	unsigned long speed = (unsigned long)(step_s * TO_SPEED);
	//Serial.print("the actual to speed is: ");
	//Serial.println(speed);
	//spd &= 0x0FFFFF;  // Masking  Dingsan: No need to masking here
	tobyte(3, speed, prm);
	L6470_write(dev_id, 1, cmd);
	L6470_write(dev_id, 3, prm);
	unsigned long value = tolong(3, prm);
	//Serial.print("the value put into is: ");
	//Serial.println(value);
}

void L6470_stepclock(byte dev_id, byte dir) {
	byte cmd[] = { STEPCLOCK | dir };
	while (L6470_isbusy(dev_id)) {
		// Do nothing while BUSY -- keep waiting
	}
	L6470_write(dev_id, 1, cmd);
}

void L6470_move(byte dev_id, byte dir, unsigned long n_step) {
	byte cmd[] = { MOVE | dir };
	byte prm[3];
	unsigned long status_data;
	n_step &= 0x3FFFFF;  // Masking
	tobyte(3, n_step, prm);
	while (L6470_isbusy(dev_id)) {
		// Do nothing while BUSY -- keep waiting
	}
	status_data = L6470_getstatus(dev_id);

	L6470_write(dev_id, 1, cmd);
	L6470_write(dev_id, 3, prm);

	Serial.print("the step to move is: ");
	Serial.println(n_step);

}

void L6470_goto(byte dev_id, unsigned long abs_pos) {
	byte cmd[] = { GOTO };
	byte prm[3];
	abs_pos &= 0x3FFFFF;  // Masking
	tobyte(3, abs_pos, prm);
	while (L6470_isbusy(dev_id)) {
		// Do nothing while BUSY -- keep waiting
	}
	L6470_write(dev_id, 1, cmd);
	L6470_write(dev_id, 3, prm);
}

void L6470_goto_dir(byte dev_id, byte dir, unsigned long abs_pos) {
	byte cmd[] = { GOTO_DIR | dir }, prm[3];
	abs_pos &= 0x3FFFFF;  // Masking
	tobyte(3, abs_pos, prm);
	while (L6470_isbusy(dev_id)) {
		// Do nothing while BUSY -- keep waiting
	}
	L6470_write(dev_id, 1, cmd);
	L6470_write(dev_id, 3, prm);
}

void L6470_gountil(byte dev_id, byte act, byte dir, float step_s) {
	byte cmd[] = { GOUNTIL | (act << 3) | dir }, prm[3];
	unsigned long spd = (unsigned long)(step_s * TO_SPEED);
	spd &= 0x0FFFFF;  // Masking
	tobyte(3, spd, prm);
	L6470_write(dev_id, 1, cmd);
	L6470_write(dev_id, 3, prm);
}

void L6470_releasesw(byte dev_id, byte act, byte dir) {
	byte cmd[] = { RELEASESW | (act << 3) | dir };
	L6470_write(dev_id, 1, cmd);
}

void L6470_gohome(byte dev_id) {
	byte cmd[] = { GOHOME };
	while (L6470_isbusy(dev_id)) {
		// Do nothing while BUSY -- keep waiting
	}
	L6470_write(dev_id, 1, cmd);
}

void L6470_gomark(byte dev_id) {
	byte cmd[] = { GOMARK };
	while (L6470_isbusy(dev_id)) {
		// Do nothing while BUSY -- keep waiting
	}
	L6470_write(dev_id, 1, cmd);
}

void L6470_resetpos(byte dev_id) {
	byte cmd[] = { RESETPOS };
	L6470_write(dev_id, 1, cmd);
}

void L6470_resetdevice(byte dev_id) {
	byte cmd[] = { RESETDEVICE };
	L6470_write(dev_id, 1, cmd);
}

void L6470_softstop(byte dev_id) {
	byte cmd[] = { SOFTSTOP };
	L6470_write(dev_id, 1, cmd);
}

void L6470_hardstop(byte dev_id) {
	byte cmd[] = { HARDSTOP };
	L6470_write(dev_id, 1, cmd);
}

void L6470_softhiz(byte dev_id) {
	byte cmd[] = { SOFTHIZ };
	L6470_write(dev_id, 1, cmd);
}

void L6470_hardhiz(byte dev_id) {
	byte cmd[] = { HARDHIZ };
	L6470_write(dev_id, 1, cmd);
}

unsigned long L6470_getstatus(byte dev_id) { //reset STATUS register warning flags. Forces system to exit from any error state
	byte cmd[] = { GETSTATUS };
	byte value[2];
	L6470_write(dev_id, 1, cmd);
	L6470_read(dev_id, 2, value);
	return tolong(2, value);
}

long to_speed(float step_s) {
	return (long)(step_s * TO_SPEED);
}

long to_acc(float step_s2) {
	return (long)(step_s2 * TO_ACC);
}

long to_dec(float step_s2) {
	return (long)(step_s2 * TO_DEC);
}

long to_max_speed(float step_s) {
	return (long)(step_s * TO_MAX_SPEED);
}

long to_min_speed(float step_s) {
	return (long)(step_s * TO_MIN_SPEED);
}

long to_fs_spd(float step_s) {
	return (long)(step_s * TO_FS_SPD - 0.5);
}

long to_int_speed(float step_s) {
	return (long)(step_s * TO_INT_SPD);
}



void SetACC(byte dev_id, unsigned long value) {
	if (value > 0xFFF) { value = 0xFFF; }
	L6470_setparam(dev_id, ACCELERATE, value);
}

void SetACC_step(byte dev_id, unsigned long value) {
	unsigned long speed = value * TO_ACC;
	if (speed > 0xFFF) { speed = 0xFFF; }
	L6470_setparam(dev_id, ACCELERATE, speed);
}

void SetDEC(byte dev_id, unsigned long value) {
	if (value > 0xFFF) { value = 0xFFF; }
	L6470_setparam(dev_id, DECELERATE, value);
}

void SetDEC_step(byte dev_id, unsigned long value) {
	unsigned long speed = value * TO_DEC;
	if (speed > 0xFFF) { speed = 0xFFF; }
	L6470_setparam(dev_id, DECELERATE, speed);
}

void SetMAXSPEED(byte dev_id, unsigned long value) {
	if (value > 0xFFF) { value = 0x3FF; }
	L6470_setparam(dev_id, MAX_SPEED, value);
}

void SetMAXSPEED_step(byte dev_id, unsigned long value) {
	unsigned long speed = value * TO_MAX_SPEED;
	if (speed > 0xFFF) { speed = 0x3FF; }
	L6470_setparam(dev_id, MAX_SPEED, speed);
}

void SetMINSPEED(byte dev_id, unsigned long value) {
	if (value > 0xFFF) { value = 0xFFF; }
	L6470_setparam(dev_id, MIN_SPEED, value);
}

void SetMINSPEED_step(byte dev_id, unsigned long value) {
	unsigned long speed = value * TO_MIN_SPEED;
	if (speed > 0xFFF) { speed = 0xFFF; }
	L6470_setparam(dev_id, MIN_SPEED, speed);
}

void SetFSSPD(byte dev_id, unsigned long value) {
	if (value > 0xFFF) { value = 0x3FF; }
	L6470_setparam(dev_id, FS_SPD, value);
}

void SetFSSPD_step(byte dev_id, unsigned long value) {
	unsigned long speed = value * TO_FS_SPD-0.5;
	if (speed > 0xFFF) { speed = 0x3FF; }
	L6470_setparam(dev_id, FS_SPD, speed);
}


void SetSTEPMODE(byte dev_id, byte value) {
	//Serial.println("start to set a new STEP_MODE");
	//byte stepmode = L6470_getparam(dev_id, STEP_MODE) & STEP_MODE_STEP_SEL_MSK;
	//Serial.print("before set the step_mode is: ");
	//Serial.println(stepmode);
	while(L6470_ismotorrunning(dev_id)){}
	L6470_hardhiz(dev_id);
	value &= 0x07; // masking for the later three bit
	//Serial.print("the value we set is: ");
	//Serial.println(value);
	L6470_setparam(dev_id, STEP_MODE, value);
	//stepmode = L6470_getparam(dev_id, STEP_MODE) & STEP_MODE_STEP_SEL_MSK;
	//Serial.print("after set the step_mode is: ");
	//Serial.println(stepmode);
	L6470_hardstop(dev_id);
}