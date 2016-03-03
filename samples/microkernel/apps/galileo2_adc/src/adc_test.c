/*
 * Copyright (c) 2016 Wind River Systems, Inc.
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

/**
 * @file
 *
 * @brief ADC demo application
 *
 * The demo reads data from ADC (channels 0 - 3) and prints the results on the
 * console and LCD
 */

#include <zephyr.h>
#include <i2c.h>
#include <adc.h>

#define LCD_I2C_ADDR 0x003e

/*
 * Currently the I2C routines hangs up when trying to access an invalid I2C
 * address instead of returning an error. Therefore to include LCD output
 * set lcd_present to '1'
 */
int lcd_present = 1;

/**
 *
 * @brief set the Grove LCD screen color
 *
 * @return N/A
 *
 * red, green and blue range from 0 to 255.
 */
void lcd_color_set (struct device *i2c, uint8_t red,
					uint8_t green, uint8_t blue)
{
	uint16_t i2c_addr = 0x0062;

	static uint8_t lcd_init_sequence[][2] = {
		{0x00, 0x00},
		{0x01, 0x00},
		{0x08, 0xaa}
	};
	uint8_t lcd_set_color[][2] = {
		{0x04, red},
		{0x03, green},
		{0x02, blue}
	};
	int i;
	int j;

	if (!lcd_present)
		return;

	for (j = 0; j < 2; j++) {
		for (i = 0; i < sizeof(lcd_init_sequence) / sizeof(uint8_t[2]);
			 i++) {
			i2c_write(i2c, lcd_init_sequence[i],
					  sizeof(lcd_init_sequence[i]),
					  i2c_addr);
		}
	}

	for (i = 0; i < sizeof(lcd_set_color) / sizeof(uint8_t[2]);
		 i++) {
		i2c_write(i2c, lcd_set_color[i],
				  sizeof(lcd_set_color[i]),
				  i2c_addr);
	}

}


/**
 *
 * @brief set the Grove LCD screen cursor
 *
 * @return N/A
 *
 * row can be 0 or 1 column - 0 to 19
 */

void lcd_cursor_set (struct device *i2c, uint8_t row, uint8_t column)
{
	uint8_t buf[] = {0, 0};

	if (!lcd_present)
		return;

	column = (row == 0 ? (column | 0x80) : (column | 0xc0));
	buf[1] = column;
	i2c_write(i2c, buf, sizeof(buf), LCD_I2C_ADDR);
}

/**
 *
 * @brief initialize the Grove LCD screen
 *
 * @return N/A
 */
void lcd_init (struct device *i2c)
{
	uint8_t buf[] = {0, 0};
	int status = 0;

	if (!lcd_present)
		return;

	PRINT("Initialize LCD ... ");

	buf[1] = 0x28;      /* 2 line display */
	status |= i2c_write(i2c, buf, sizeof(buf), LCD_I2C_ADDR);

	buf[1] = 0xf;       /* Cursor on, blinking on */
	status |= i2c_write(i2c, buf, sizeof(buf), LCD_I2C_ADDR);

	PRINT("done\n");
}

/**
 *
 * @brief clear the Grove LCD screen
 *
 * @return N/A
 */
void lcd_clear (struct device *i2c)
{
	uint8_t buf[] = {0, 1};

	if (!lcd_present)
		return;

	i2c_write(i2c, buf, sizeof(buf), LCD_I2C_ADDR);
}


void lcd_print(struct device *i2c, uint8_t row, uint8_t column,
			   char * string, uint8_t len)
{
	int i;
	uint8_t buf[] = {0x40, 0};

	if (!lcd_present)
		return;

	lcd_cursor_set(i2c, row, column);

	for (i = 0; i < len; i++) {
		buf[1] = string[i];
		i2c_write(i2c, buf, sizeof(buf), LCD_I2C_ADDR);
	}
}

/* ADC sampling structures */
uint8_t adc_channel0_data[2];
uint8_t adc_channel1_data[2];
uint8_t adc_channel2_data[2];
uint8_t adc_channel3_data[2];

struct adc_seq_entry adc_channels[] = {
	{ 10, 0, adc_channel0_data, sizeof(adc_channel0_data) },
	{ 10, 2, adc_channel1_data, sizeof(adc_channel1_data) },
	{ 10, 4, adc_channel2_data, sizeof(adc_channel2_data) },
	{ 10, 6, adc_channel3_data, sizeof(adc_channel3_data) },
};

struct adc_seq_table adc_channel_descriptor = {
	adc_channels, sizeof(adc_channels) / sizeof(struct adc_seq_entry)
};

/**
 *
 * @brief Main task function
 *
 * Initializes LCD, reads ADC, prints results
 */
void adc_task(void)
{
	int rc;
	struct device *adc;
	struct device *i2c;
	char string_buf[20] = { 0, };

	i2c  = device_get_binding("I2C0");
	if (!i2c) {
		PRINT("I2C not found!!\n");
		lcd_present = 0;
	} else {
		rc = i2c_configure(i2c, (I2C_SPEED_FAST << 1) | I2C_MODE_MASTER);
		if (rc != DEV_OK) {
			PRINT("I2C configuration error: %d\n", rc);
			lcd_present = 0;
		}
	}
	adc = device_get_binding(CONFIG_ADC_TI_ADC108S102_0_DRV_NAME);
	if (!adc) {
		PRINT("ADC not found!!\n");
	}

	lcd_init(i2c);
	lcd_color_set(i2c, 100, 100, 100);
	lcd_print(i2c, 0, 0, "ABCDEFGHIJKLMNOPQRS", 19);
	lcd_print(i2c, 1, 0, "abcdefghijklmnopqrs", 19);

	while (adc) {
		if (adc_read(adc, &adc_channel_descriptor) != DEV_OK) {
			PRINT("ADC read data error\n");
		} else {
			PRINT("Channel A0 - %02x %02x\n",
				  adc_channel0_data[1], adc_channel0_data[0]);
			PRINT("Channel A1 - %02x %02x \n",
				  adc_channel1_data[1], adc_channel1_data[0]);
			PRINT("Channel A2 - %02x %02x \n",
				  adc_channel2_data[1], adc_channel2_data[0]);
			PRINT("Channel A3 - %02x %02x \n",
				  adc_channel3_data[1], adc_channel3_data[0]);

			sprintf(string_buf, "A0:%4d  A1:%4d",
					(int)adc_channel0_data[1] << 8 | adc_channel0_data[0],
					(int)adc_channel1_data[1] << 8 | adc_channel1_data[0]);
			lcd_print(i2c, 0, 0, string_buf, 19);
			sprintf(string_buf, "A2:%4d  A3:%4d",
					(int)adc_channel2_data[1] << 8 | adc_channel2_data[0],
					(int)adc_channel3_data[1] << 8 | adc_channel3_data[0]);
			lcd_print(i2c, 1, 0, string_buf, 19);
		}
		task_sleep(50);
	}
}
