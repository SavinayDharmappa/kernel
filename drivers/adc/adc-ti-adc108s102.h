/* adc-ti-adc108s102.h - Private  */

/*
 * Copyright (c) 2015 Intel Corporation
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
#ifndef __ADC108S102_PRIV_H__
#define __ADC108S102_PRIV_H__

#include <spi.h>
#include <adc.h>

#include <misc/byteorder.h>

#define ADC108S102_CHANNELS			8
/* 8 chans maximum */
#define ADC108S102_CMD_BUFFER_SIZE		(ADC108S102_CHANNELS * 2)
/* 8 channels maximum */
#define ADC108S102_SAMPLING_BUFFER_SIZE		(ADC108S102_CHANNELS * 2)
/*
 * As the datasheet states, the channel number should be 3 bits shifted
 * to the left. Also in order to read two bytes of data, we need an extra
 * byte, blus one bit left shift to synchronize with the way ADC reads
 * the channel number
 */
#define ADC108S102_CHANNEL_CMD(_channel_)	\
	sys_cpu_to_be16((_channel_) << (3 + 9))

struct ti_adc108s102_config {
	const char *spi_port;
	uint32_t spi_config_flags;
	uint32_t spi_freq;
	uint32_t spi_slave;
};

struct ti_adc108s102_data {
	uint16_t cmd_buffer[ADC108S102_CMD_BUFFER_SIZE];
	uint16_t sampling_buffer[ADC108S102_SAMPLING_BUFFER_SIZE];
	int buflen; /* length of the command and sampling buffer */
	struct device *spi;
	struct adc_seq_table *seq_table;
	adc_callback_t cb;
};

#endif /* __ADC108S102_PRIV_H__ */
