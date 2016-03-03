/* hello.c - Hello World demo */

/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1) Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3) Neither the name of Wind River Systems nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(CONFIG_STDOUT_CONSOLE)
#include <stdio.h>
#define PRINT           printf
#else
#include <misc/printk.h>
#define PRINT           printk
#endif

/*
 * Microkernel version of hello world demo has two tasks that utilize
 * semaphores and sleeps to take turns printing a greeting message at
 * a controlled rate. In addition it PWM controls header IO11 on the
 * Galileo Gen2 board, as well as GPIO set/un-set header pin IO7. Use
 * header pin IO8 as input to test GPIO input.
 */

#include <zephyr.h>
#include <gpio.h>
#include <i2c.h>
#include <pwm.h>

/* specify delay between greetings (in ms); compute equivalent in ticks */

#define SLEEPTIME  2000
#define SLEEPTICKS (SLEEPTIME * sys_clock_ticks_per_sec / 1000)

struct device *exp0;
struct device *exp1;
struct device *exp2;
struct device *i2c;
struct device *pwm;

uint8_t line0[20];
uint8_t line1[20];

/*
 * Currently the I2C routines hangs up when trying to access an invalid I2C
 * address instead of returning an error. Therefore to include LCD output
 * set lcdPresent to '1'
 */
int lcdPresent = 0;

void lcdColorSet (uint8_t red, uint8_t green, uint8_t blue)
{
	uint16_t i2c_addr = 0x0062;

	uint8_t buf[] = {0, 0};

	if (!lcdPresent)
		return;

	buf[0] = 0; buf[1] = 0;
	i2c_write(i2c, buf, sizeof(buf), i2c_addr);
	buf[0] = 1; buf[1] = 0;
	i2c_write(i2c, buf, sizeof(buf), i2c_addr);
	buf[0] = 8; buf[1] = 0xaa;
	i2c_write(i2c, buf, sizeof(buf), i2c_addr);

	buf[0] = 0; buf[1] = 0;
	i2c_write(i2c, buf, sizeof(buf), i2c_addr);
	buf[0] = 1; buf[1] = 0;
	i2c_write(i2c, buf, sizeof(buf), i2c_addr);
	buf[0] = 8; buf[1] = 0xaa;
	i2c_write(i2c, buf, sizeof(buf), i2c_addr);

	buf[0] = 4; buf[1] = red;
	i2c_write(i2c, buf, sizeof(buf), i2c_addr);
	buf[0] = 3; buf[1] = green;
	i2c_write(i2c, buf, sizeof(buf), i2c_addr);
	buf[0] = 2; buf[1] = blue;
	i2c_write(i2c, buf, sizeof(buf), i2c_addr);

}

void lcdCursorSet (uint8_t row, uint8_t column)
{
	uint16_t i2c_addr = 0x003e;
	uint8_t buf[] = {0, 0};

	if (!lcdPresent)
		return;

	column = (row == 0 ? (column | 0x80) : (column | 0xc0));
	buf[1] = column;
	i2c_write(i2c, buf, sizeof(buf), i2c_addr);
}

void lcdInit ()
{
	uint16_t i2c_addr = 0x003e;
	uint8_t buf[] = {0, 0};
	int status = 0;

	if (!lcdPresent)
		return;

	PRINT("Initialize LCD ... ");

	buf[1] = 0x28;      /* 2 line display */
	status |= i2c_write(i2c, buf, sizeof(buf), i2c_addr);

	buf[1] = 0xf;       /* Cursor on, blinking on */
	status |= i2c_write(i2c, buf, sizeof(buf), i2c_addr);
#if 0
	buf[1] = 0xc;       /* Cursor off, blinking off */
	status |= i2c_write(i2c, buf, sizeof(buf), i2c_addr);
#endif
	PRINT("done\n");
}

void lcdClear ()
{
	uint16_t i2c_addr = 0x003e;
	uint8_t buf[] = {0, 1};

	if (!lcdPresent)
		return;

	i2c_write(i2c, buf, sizeof(buf), i2c_addr);
}


void lcdPrint(uint8_t row, uint8_t column, char * string, uint8_t len)
{
	int i;
	uint8_t buf[] = {0x40, 0};

	if (!lcdPresent)
		return;

	lcdCursorSet(row, column);

	for (i = 0; i < len; i++) {
		buf[1] = string[i];
		i2c_write(i2c, buf, sizeof(buf), 0x003e);
	}
}

/*
 *
 * @param taskname    task identification string
 * @param mySem       task's own semaphore
 * @param otherSem    other task's semaphore
 *
 */
void helloLoop(const char *taskname, ksem_t mySem, ksem_t otherSem)
{
	int lo_data;
	int percentage;
	uint32_t value = 0;
	char string[20];

	while (1) {
		task_sem_take(mySem, TICKS_UNLIMITED);

		if (exp0) {
			if (mySem == TASKASEM) {
				PRINT("******** START PWM TASKA ***********\n");
				lcdPrint(0,0, "TASKA : Start       ", 20);

				PRINT("Pin IO11: 100 percent on\n");
				lcdPrint(1,0, "IO11 : 100 %        ", 20);

				pwm_pin_set_duty_cycle(pwm, 9, 100);
				task_sleep(500);
				PRINT("Pin IO11: 99 percent on\n");
				lcdPrint(1,0, "IO11 : 99 %         ", 20);
				pwm_pin_set_duty_cycle(pwm, 9, 99);
				task_sleep(500);
				PRINT("Pin IO11: 0 percent on\n");
				lcdPrint(1,0, "IO11 : 0 %          ", 20);
				pwm_pin_set_duty_cycle(pwm, 9, 0);
				task_sleep(500);

				PRINT("Pin IO11: Cycle of 0 -> 100 percent \n");

				for (percentage = 0; percentage <= 100; percentage++) {
					pwm_pin_set_duty_cycle(pwm, 9, percentage);
					snprintf(string, 20, "IO11 : %d %%      ", percentage);
					lcdPrint(1,0, string, 20);
					task_sleep(10);
				}

				PRINT("******** END OF PWM TASKA **********\n");
				lcdPrint(0,0, "TASKA : Stop        ", 20);
			}
			else if (mySem == TASKBSEM) {
				PRINT("******** START PWM TASKB ***********\n");
				PRINT("Pin IO11 on/of inverse cycle\n");
				for (lo_data = 0; lo_data < 4096; lo_data++) {
					if (lo_data > 100)
						lo_data += 10;
					if (lo_data > 500)
						lo_data += 10;
					if (lo_data > 1000)
						lo_data += 10;
					if (lo_data > 1500)
						lo_data += 10;
					if (lo_data > 2000)
						lo_data += 10;
					if (lo_data > 2500)
						lo_data += 20;
					if (lo_data > 3000)
						lo_data += 20;
					if (lo_data > 3500)
						lo_data += 20;

					pwm_pin_set_values (pwm, 9, 4096 - lo_data, lo_data);
					task_sleep(10);
				}
				PRINT("******** END OF PWM TASKB **********\n");
			}
		}

		if (exp1) {
			if (mySem == TASKASEM) {
				PRINT("Pin IO7 on\n");
				gpio_pin_write(exp1, 6, 1);
				gpio_pin_read(exp1, 8, &value);
				PRINT("Pin IO8 read = %d\n", value);
			}
			else if (mySem == TASKBSEM) {
				PRINT("Pin IO7 off\n");
				gpio_pin_write(exp1, 6, 0);
				gpio_pin_read(exp1, 8, &value);
				PRINT("Pin IO8 read = %d\n", value);
			}
		}

		/* say "hello" */

		PRINT("%s: Hello World!\n", taskname);

		/* wait a while, then let other task have a turn */

		task_sleep(SLEEPTICKS);
		task_sem_give(otherSem);
	}
}

void taskA(void)
{
	int rc1, rc2;

	/* assign device pointers */

	exp0 = device_get_binding(CONFIG_GPIO_PCAL9535A_0_DEV_NAME);
	exp1 = device_get_binding(CONFIG_GPIO_PCAL9535A_1_DEV_NAME);
	exp2 = device_get_binding(CONFIG_GPIO_PCAL9535A_2_DEV_NAME);
	i2c  = device_get_binding("I2C0");
	pwm  = device_get_binding(CONFIG_PWM_PCA9685_0_DEV_NAME);

	if (!i2c)
		PRINT("I2C not found!!\n");
	else {
		rc1 = i2c_configure(i2c, (I2C_SPEED_FAST << 1) | I2C_MODE_MASTER);
		if (rc1 != DEV_OK)
			PRINT("I2C configuration error: %d\n", rc1);
	}

	if (!exp0)
		PRINT("EXP0 not found!!\n");
	else {
		rc1 = gpio_pin_configure(exp0, 8, GPIO_DIR_OUT | GPIO_PUD_PULL_UP);
		if (rc1 != DEV_OK)
			PRINT("EXP0 config error %d !!\n", rc1);
		else {
			/*
			 * This sets LVL_B_OE4_N on EXP0 P1_0 to 0
			 * set P1_0 output to 0, this goes to the output enable buffer
			 * chip that is inverse true, (0 becomes a 1) to enable output
			 * of buffer IO11
			 */

			gpio_pin_write(exp0, 8, 0);
		}
	}

	if (!exp1)
		PRINT("EXP1 not found!!\n");
	else {
		/*
		 * Use IO7 as output to drive LED
		 * Use IO8 as input to read
		 */

		rc1 = gpio_pin_configure(exp1, 6, GPIO_DIR_OUT);
		rc2 = gpio_pin_configure(exp1, 8, GPIO_DIR_IN);

		if (rc1 != DEV_OK || rc2 != DEV_OK)
			PRINT("GPIO config error %d, %d!!\n", rc1, rc2);
	}

	if (!exp2)
		PRINT("EXP2 not found!!\n");
	else {
		/*
		 * NXP_PCA95XX_P1_4 = 12
		 */

		rc1 = gpio_pin_configure(exp2, 12, GPIO_DIR_OUT);

		if (rc1 != DEV_OK)
			PRINT("GPIO config error %d, %d!!\n", rc1);

		gpio_pin_write(exp2, 12, 0);
	}

	if (!pwm)
		PRINT("PWM not found!!\n");
	else {
		rc1 = pwm_pin_configure(pwm, 0, 0);
		if (rc1 != DEV_OK) {
			PRINT("EXP0 config error %d !!\n", rc1);
		}
		else {
			/*
			 * Set PWM led 8 pin to full on
			 * Then PWM control led 9 pin
			 * This needs PCAL EXP0 pin 8 to be set as output and low
			 */
			pwm_pin_set_values(pwm, 8, 4096, 0);
			pwm_pin_configure(pwm, 8, 0);
		}
	}

	if (i2c) {
		lcdInit();
		lcdColorSet(0, 100, 200);
		lcdPrint(0,0, "ABCDEFGHIJKLMNOPQRS",19);
		lcdPrint(1,0, "abcdefghijklmnopqrs",19);

		task_sleep(100);

	}

	/* taskA gives its own semaphore, allowing it to say hello right away */
	task_sem_give(TASKASEM);

	/* invoke routine that allows task to ping-pong hello messages with taskB */
	helloLoop(__FUNCTION__, TASKASEM, TASKBSEM);
}

void taskB(void)
{
	/*
	  invoke routine that allows task to ping-pong hello
	  messages with taskA
	*/
	helloLoop(__FUNCTION__, TASKBSEM, TASKASEM);
}
