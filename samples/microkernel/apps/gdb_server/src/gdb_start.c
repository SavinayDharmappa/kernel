/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>

#ifdef CONFIG_MICROKERNEL

/*
 * Microkernel version of hello world demo has two tasks that utilize
 * semaphores and sleeps to take turns printing a greeting message at
 * a controlled rate.
 */

#include <zephyr.h>

/* specify delay between greetings (in ms); compute equivalent in ticks */

#define SLEEPTIME  50
#define SLEEPTICKS (SLEEPTIME * sys_clock_ticks_per_sec / 1000)

void gdbStart(void)
{
	while (1) {
		/* wait a while, then let other task have a turn */
		task_sleep(SLEEPTICKS);
	}
}

#else /*  CONFIG_NANOKERNEL */

/*
 * Nanokernel version of hello world demo has a task and a fiber that utilize
 * semaphores and timers to take turns printing a greeting message at
 * a controlled rate.
 */

#include <nanokernel.h>

/* specify delay between greetings (in ms); compute equivalent in ticks */

#define SLEEPTIME  50
#define SLEEPTICKS (SLEEPTIME * sys_clock_ticks_per_sec / 1000)

void main(void)
{
	struct nano_timer timer;
    uint32_t data[2] = {0, 0};

    nano_timer_init(&timer, data);

	while (1) {
		nano_task_timer_start(&timer, SLEEPTICKS);
		nano_task_timer_wait(&timer);
	}
}

#endif /* CONFIG_MICROKERNEL ||  CONFIG_NANOKERNEL */
