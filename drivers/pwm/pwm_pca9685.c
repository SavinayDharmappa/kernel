/*
 * Copyright (c) 2015 Intel Corporation.
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

/**
 * @file Driver for PCA9685 I2C-based PWM driver.
 */

#include <nanokernel.h>

#include <i2c.h>
#include <pwm.h>

#include "pwm_pca9685.h"

#define REG_MODE1		0x00
#define REG_MODE2		0x01

#define REG_LED_ON_L(n)		((4 * n) + 0x06)
#define	REG_LED_ON_H(n)		((4 * n) + 0x07)
#define REG_LED_OFF_L(n)	((4 * n) + 0x08)
#define REG_LED_OFF_H(n)	((4 * n) + 0x09)

#define REG_ALL_LED_ON_L	0xFA
#define REG_ALL_LED_ON_H	0xFB
#define REG_ALL_LED_OFF_L	0xFC
#define REG_ALL_LED_OFF_H	0xFD
#define REG_PRE_SCALE		0xFE

/* Maximum PWM outputs */
#define MAX_PWM_OUT		16

/* How many ticks per one period */
#define PWM_ONE_PERIOD_TICKS	4096

/*
 * This table uses pre-calculated values for the fraction 40.96 times
 * the precentage into the index, providing a scaled range up to  100% = 4096
 * This negates the need for floating point calculations and also increase
 * speed. Alternatively less accurate scalar of 40 or 41 could be chosen as the
 * fraction, instead of the actual 40.96
 */
uint16_t percent_range[101] =
            { 0x0000, 0x0028, 0x0051, 0x007a, 0x00a3, 0x00cc, 0x00f5, 0x011e,
              0x0147, 0x0170, 0x0199, 0x01c2, 0x01eb, 0x0214, 0x023d, 0x0266,
              0x028f, 0x02b8, 0x02e1, 0x030a, 0x0333, 0x035c, 0x0385, 0x03ae,
              0x03d7, 0x03ff, 0x0428, 0x0451, 0x047a, 0x04a3, 0x04cc, 0x04f5,
              0x051e, 0x0547, 0x0570, 0x0599, 0x05c2, 0x05eb, 0x0614, 0x063d,
              0x0666, 0x068f, 0x06b8, 0x06e1, 0x070a, 0x0733, 0x075c, 0x0785,
              0x07ae, 0x07d7, 0x07ff, 0x0828, 0x0851, 0x087a, 0x08a3, 0x08cc,
              0x08f5, 0x091e, 0x0947, 0x0970, 0x0999, 0x09c2, 0x09eb, 0x0a14,
              0x0a3d, 0x0a66, 0x0a8f, 0x0ab8, 0x0ae1, 0x0b0a, 0x0b33, 0x0b5c,
              0x0b85, 0x0bae, 0x0bd7, 0x0bff, 0x0c28, 0x0c51, 0x0c7a, 0x0ca3,
              0x0ccc, 0x0cf5, 0x0d1e, 0x0d47, 0x0d70, 0x0d99, 0x0dc2, 0x0deb,
              0x0e14, 0x0e3d, 0x0e66, 0x0e8f, 0x0eb8, 0x0ee1, 0x0f0a, 0x0f33,
              0x0f5c, 0x0f85, 0x0fae, 0x0fd7, 0x1000 };

/**
 * @brief Check to see if a I2C master is identified for communication.
 *
 * @param dev Device struct.
 * @return 1 if I2C master is identified, 0 if not.
 */
static inline int _has_i2c_master(struct device *dev)
{
	struct pwm_pca9685_drv_data * const drv_data =
		(struct pwm_pca9685_drv_data * const)dev->driver_data;
	struct device * const i2c_master = drv_data->i2c_master;

	if (i2c_master)
		return 1;
	else
		return 0;
}

static int pwm_pca9685_configure(struct device *dev, int access_op,
				 uint32_t pwm, int flags)
{
    const struct pwm_pca9685_config * const config =
        dev->config->config_info;
    struct pwm_pca9685_drv_data * const drv_data =
        (struct pwm_pca9685_drv_data * const)dev->driver_data;
    uint8_t buf[] = {0, 0};
    int ret;

	ARG_UNUSED(access_op);
	ARG_UNUSED(pwm);
	ARG_UNUSED(flags);

    /* MODE1 register */

    buf[0] = REG_MODE1;
    buf[1] = (1 << 5); /* register addr auto increment */

    if (i2c_configure(drv_data->i2c_master,
              (I2C_MODE_MASTER | (I2C_SPEED_FAST << 1)))) {
        return DEV_NOT_CONFIG;
    }

    ret = i2c_write(drv_data->i2c_master, buf, 2,
							config->i2c_slave_addr);
    if (ret != DEV_OK) {
        return DEV_NOT_CONFIG;
    }

	return DEV_OK;
}

static int pwm_pca9685_set_values(struct device *dev, int access_op,
				  uint32_t pwm, uint32_t on, uint32_t off)
{
	const struct pwm_pca9685_config * const config =
		dev->config->config_info;
	struct pwm_pca9685_drv_data * const drv_data =
		(struct pwm_pca9685_drv_data * const)dev->driver_data;
	struct device * const i2c_master = drv_data->i2c_master;
	uint16_t i2c_addr = config->i2c_slave_addr;
	uint8_t buf[] = { 0, 0, 0, 0, 0};

	if (!_has_i2c_master(dev)) {
		return DEV_INVALID_CONF;
	}

	switch (access_op) {
	case PWM_ACCESS_BY_PIN:
		if (pwm > MAX_PWM_OUT) {
			return DEV_INVALID_CONF;
		}
		buf[0] = REG_LED_ON_L(pwm);
		break;
	case PWM_ACCESS_ALL:
		buf[0] = REG_ALL_LED_ON_L;
		break;
	default:
		return DEV_INVALID_OP;
	}

	/* If either ON and/or OFF > max ticks, treat PWM as 100%.
	 * If OFF value == 0, treat it as 0%.
	 * Otherwise, populate registers accordingly.
	 */
	if ((on >= PWM_ONE_PERIOD_TICKS) || (off >= PWM_ONE_PERIOD_TICKS)) {
		buf[1] = 0x0;
		buf[2] = (1 << 4);
		buf[3] = 0x0;
		buf[4] = 0x0;
	} else if (off == 0) {
		buf[1] = 0x0;
		buf[2] = 0x0;
		buf[3] = 0x0;
		buf[4] = (1 << 4);
	} else {
		buf[1] = (on & 0xFF);
		buf[2] = ((on >> 8) & 0x0F);
		buf[3] = (off & 0xFF);
		buf[4] = ((off >> 8) & 0x0F);
	}

	return i2c_write(i2c_master, buf, sizeof(buf), i2c_addr);
}

/**
 * Duty cycle describes the percentage of time a signal is turned
 * to the ON state.
 */
static int pwm_pca9685_set_duty_cycle(struct device *dev, int access_op,
				      uint32_t pwm, uint8_t duty)
{
	uint32_t on, off, phase;

	phase = 0;     /* Hard coded until API changes */

	if (duty == 0) {
		/* Turn off PWM */
		on = 0;
		off = 0;
	} else if (duty >= 100) {
		/* Force PWM to be 100% */
		on = PWM_ONE_PERIOD_TICKS + 1;
		off = PWM_ONE_PERIOD_TICKS + 1;
	} else {
		off = percent_range[duty] + phase;
		on = phase;
	}

	return pwm_pca9685_set_values(dev, access_op, pwm, on, off);
}

/**
 * Phase describes number of clock ticks of delay before the start of the
 * pulse.  This functionality is not a supported.
 */
static int pwm_pca9685_set_phase(struct device *dev, int access_op,
				  uint32_t pwm, uint8_t phase)
{
	if (phase != 0)
		return DEV_INVALID_OP;
	else
		return DEV_OK;
}

static int pwm_pca9685_suspend(struct device *dev)
{
	if (!_has_i2c_master(dev)) {
		return DEV_INVALID_CONF;
	}

	return DEV_INVALID_OP;
}

static int pwm_pca9685_resume(struct device *dev)
{
	if (!_has_i2c_master(dev)) {
		return DEV_INVALID_CONF;
	}

	return DEV_INVALID_OP;
}

static struct pwm_driver_api pwm_pca9685_drv_api_funcs = {
	.config = pwm_pca9685_configure,
	.set_values = pwm_pca9685_set_values,
	.set_duty_cycle = pwm_pca9685_set_duty_cycle,
	.set_phase = pwm_pca9685_set_phase,
	.suspend = pwm_pca9685_suspend,
	.resume = pwm_pca9685_resume,
};

/**
 * @brief Initialization function of PCA9685
 *
 * @param dev Device struct
 * @return DEV_OK if successful, failed otherwise.
 */
int pwm_pca9685_init(struct device *dev)
{
	const struct pwm_pca9685_config * const config =
		dev->config->config_info;
	struct pwm_pca9685_drv_data * const drv_data =
		(struct pwm_pca9685_drv_data * const)dev->driver_data;
	struct device *i2c_master;

	dev->driver_api = &pwm_pca9685_drv_api_funcs;

	/* Find out the device struct of the I2C master */
	i2c_master = device_get_binding((char *)config->i2c_master_dev_name);
	if (!i2c_master) {
		return DEV_INVALID_CONF;
	}
	drv_data->i2c_master = i2c_master;

	return DEV_OK;
}

/* Initialization for PWM_PCA9685_0 */
#ifdef CONFIG_PWM_PCA9685_0
#include <device.h>
#include <init.h>

static struct pwm_pca9685_config pwm_pca9685_0_cfg = {
	.i2c_master_dev_name = CONFIG_PWM_PCA9685_0_I2C_MASTER_DEV_NAME,
	.i2c_slave_addr = CONFIG_PWM_PCA9685_0_I2C_ADDR,
};

static struct pwm_pca9685_drv_data pwm_pca9685_0_drvdata;

/* This has to init after I2C master */
DEVICE_INIT(pwm_pca9685_0, CONFIG_PWM_PCA9685_0_DEV_NAME,
			pwm_pca9685_init,
			&pwm_pca9685_0_drvdata, &pwm_pca9685_0_cfg,
			SECONDARY, CONFIG_PWM_PCA9685_INIT_PRIORITY);

#endif /* CONFIG_PWM_PCA9685_0 */
