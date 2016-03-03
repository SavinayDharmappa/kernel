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

#ifdef CONFIG_MICROKERNEL

/*
 * Microkernel version of hello world demo has two tasks that utilize
 * semaphores and sleeps to take turns printing a greeting message at
 * a controlled rate.
 */

#include <zephyr.h>
#include <gpio.h>
#include <i2c.h>
#include <pinmux.h> /* configure the IO13 pin for output */

/* specify delay between greetings (in ms); compute equivalent in ticks */

#define SLEEPTIME  500
#define SLEEPTICKS (SLEEPTIME * sys_clock_ticks_per_sec / 1000)

struct device *gpio;
struct device *gpio_sus;
struct device *i2c;
struct device *pinmux;

/*
 *
 * @param taskname    task identification string
 * @param mySem       task's own semaphore
 * @param otherSem    other task's semaphore
 *
 */
void helloLoop(const char *taskname, ksem_t mySem, ksem_t otherSem)
{
	while (1) {
		task_sem_take(mySem, TICKS_UNLIMITED);;

		if (gpio) {
			if (mySem == TASKASEM) {
				gpio_pin_write(gpio, 6, 1);
				gpio_pin_write(gpio, 8, 0);
				gpio_pin_write(gpio_sus, 5, 0);
			} else if (mySem == TASKBSEM) {
				gpio_pin_write(gpio, 8, 1);
				gpio_pin_write(gpio, 6, 0);
				gpio_pin_write(gpio_sus, 5, 1);
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
	int rc1;
	int rc2;
	gpio = device_get_binding(CONFIG_GPIO_PCAL9535A_1_DEV_NAME);
	gpio_sus = device_get_binding(CONFIG_GPIO_SCH_1_DEV_NAME);
	i2c = device_get_binding(CONFIG_GPIO_PCAL9535A_1_I2C_MASTER_DEV_NAME);
	pinmux = device_get_binding(PINMUX_NAME);
	if (!i2c) {
		PRINT("I2C not found!!\n");
	} else {
		rc1 = i2c_configure(i2c, (I2C_SPEED_FAST << 1) | I2C_MODE_MASTER);
		if (rc1 != DEV_OK) {
			PRINT("I2C configuration error: %d\n", rc1);
		}
	}
	if (!gpio) {
		PRINT("GPIO not found!!\n");
	} else {
		rc1 = gpio_pin_configure(gpio, 6, GPIO_DIR_OUT);
		rc2 = gpio_pin_configure(gpio, 8, GPIO_DIR_OUT);
		if (rc1 != DEV_OK ||
		    rc2 != DEV_OK) {
			PRINT("GPIO config error %d, %d!!\n", rc1, rc2);
		}
	}
	if (!gpio_sus) {
		PRINT("Legacy GPIO not found!!\n");
	} else {
		rc1 = gpio_pin_configure(gpio_sus, 5, GPIO_DIR_OUT);
		if (rc1 != DEV_OK) {
			PRINT("Legacy GPIO config error %d!!\n", rc1);
		}
	}
	if (!pinmux) {
		PRINT("Pinmux device not found!\n");
	} else {
		/*
		 * By default pin D13 (pin 5 on Legacy GPIO_SUS
		 * is configured for input. To blink it, it needs to
		 * be configured for output
		 */
		rc1 = pinmux_pin_set(pinmux, 13, PINMUX_FUNC_A);
		if (rc1 != DEV_OK) {
			PRINT("Pinmux config error %d!!\n", rc1);
		}
	}

	/* taskA gives its own semaphore, allowing it to say hello right away */
	task_sem_give(TASKASEM);

	/* invoke routine that allows task to ping-pong hello messages with taskB */
	helloLoop(__FUNCTION__, TASKASEM, TASKBSEM);
}

void taskB(void)
{
	/* invoke routine that allows task to ping-pong hello messages with taskA */
	helloLoop(__FUNCTION__, TASKBSEM, TASKASEM);
}

#else /*  CONFIG_NANOKERNEL */

/*
 * Nanokernel version of hello world demo has a task and a fiber that utilize
 * semaphores and timers to take turns printing a greeting message at
 * a controlled rate.
 */

#include <nanokernel.h>
#include <arch/cpu.h>

/* specify delay between greetings (in ms); compute equivalent in ticks */

#define SLEEPTIME  500
#define SLEEPTICKS (SLEEPTIME * sys_clock_ticks_per_sec / 1000)

#define STACKSIZE 2000

char __stack fiberStack[STACKSIZE];

struct nano_sem nanoSemTask;
struct nano_sem nanoSemFiber;

void fiberEntry(void)
{
	struct nano_timer timer;
	uint32_t data[2] = {0, 0};

	nano_sem_init(&nanoSemFiber);
	nano_timer_init(&timer, data);

	while (1) {
		/* wait for task to let us have a turn */
		nano_fiber_sem_take_wait(&nanoSemFiber);

		/* say "hello" */
		PRINT("%s: Hello World!\n", __FUNCTION__);

		/* wait a while, then let task have a turn */
		nano_fiber_timer_start(&timer, SLEEPTICKS);
		nano_fiber_timer_wait(&timer);
		nano_fiber_sem_give(&nanoSemTask);
	}
}

void main(void)
{
	struct nano_timer timer;
	uint32_t data[2] = {0, 0};

	task_fiber_start(&fiberStack[0], STACKSIZE,
			(nano_fiber_entry_t) fiberEntry, 0, 0, 7, 0);

	nano_sem_init(&nanoSemTask);
	nano_timer_init(&timer, data);

	while (1) {
		/* say "hello" */
		PRINT("%s: Hello Screen!\n", __FUNCTION__);

		/* wait a while, then let fiber have a turn */
		nano_task_timer_start(&timer, SLEEPTICKS);
		nano_task_timer_wait(&timer);
		nano_task_sem_give(&nanoSemFiber);

		/* now wait for fiber to let us have a turn */
		nano_task_sem_take_wait(&nanoSemTask);
	}
}

#endif /* CONFIG_MICROKERNEL ||  CONFIG_NANOKERNEL */
