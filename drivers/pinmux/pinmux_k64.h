/* pinmux_k64.h - Freescale K64 pinmux header */

/*
 * Copyright (c) 2015, Wind River Systems, Inc.
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
 * @file Header file for Freescale K64 pin multiplexing.
 */

#ifndef __INCLUDE_PINMUX_K64_H
#define __INCLUDE_PINMUX_K64_H


#define PINMUX_NUM_PINMUXS		32		/* # of I/O pins per port */

/* Port Control Register offsets */

#define PINMUX_CTRL_OFFSET(pin)	(pin * 4)


/*
 * Pin direction configuration, for GPIO function (PINMUX_FUNC_GPIO):
 * At reset, all GPIO pins are set as input.
 *
 * NOTE: This bit field is a software-defined construct and does not
 * intentionally match any hardware register bit field.
 */

#define PINMUX_GPIO_DIR_MASK	(0x1 << 20)		/* GPIO pin direction */
#define PINMUX_GPIO_DIR_INPUT	(0x0 << 20)		/* input GPIO pin */
#define PINMUX_GPIO_DIR_OUTPUT  (0x1 << 20)		/* output GPIO pin */

/* 
 * The following pin settings match the K64 PORT module's
 * Pin Control Register bit fields.
 */

/*
 * Pin interrupt configuration:
 * At reset, interrupts are disabled for all pins.
 */

#define PINMUX_INT_MASK			(0xF << 16)		/* interrupt configuration */
#define PINMUX_INT_DISABLE		(0x0 << 16)		/* disable interrupt */
#define PINMUX_INT_LOW			(0x8 << 16)		/* active-low interrupt */
#define PINMUX_INT_RISING		(0x9 << 16)		/* rising-edge interrupt */
#define PINMUX_INT_FALLING		(0xA << 16)		/* falling-edge interrupt */
#define PINMUX_INT_BOTH_EDGE	(0xB << 16)		/* either edge interrupt */
#define PINMUX_INT_HIGH			(0xC << 16)		/* active-high interrupt */

/*
 * Pin function identification:
 * At reset, the setting for PTA0/1/2/3/4 is function 7;
 * the remaining pins are set to function 0.
 */

#define PINMUX_ALT_MASK			(0x7 << 8)
#define PINMUX_ALT_0			(0x0 << 8)
#define PINMUX_ALT_1			(0x1 << 8)
#define PINMUX_ALT_2			(0x2 << 8)
#define PINMUX_ALT_3			(0x3 << 8)
#define PINMUX_ALT_4			(0x4 << 8)
#define PINMUX_ALT_5			(0x5 << 8)
#define PINMUX_ALT_6			(0x6 << 8)
#define PINMUX_ALT_7			PINMUX_ALT_MASK

#define PINMUX_FUNC_GPIO		PINMUX_ALT_1
#define PINMUX_FUNC_DISABLED	PINMUX_ALT_0
#define PINMUX_FUNC_ANALOG		PINMUX_ALT_0

/*
 * Pin drive strength configuration, for output:
 * At reset, the setting for PTA0/1/2/3/4/5 is high drive strength;
 * the remaining pins are set to low drive strength.
 */

#define PINMUX_DRV_STRN_MASK	(0x1 << 6)		/* drive strength select */
#define PINMUX_DRV_STRN_LOW		(0x0 << 6)		/* low drive strength */
#define PINMUX_DRV_STRN_HIGH	(0x1 << 6)		/* high drive strength */

/*
 * Pin open drain configuration, for output:
 * At reset, open drain is disabled for all pins.
 */

#define PINMUX_OPEN_DRN_MASK	(0x1 << 5)		/* open drain enable */
#define PINMUX_OPEN_DRN_DISABLE	(0x0 << 5)		/* disable open drain */
#define PINMUX_OPEN_DRN_ENABLE	(0x1 << 5)		/* enable open drain */

/*
 * Pin slew rate configuration, for output:
 * At reset, fast slew rate is set for all pins.
 */

#define PINMUX_SLEW_RATE_MASK	(0x1 << 2)	/* slew rate select */
#define PINMUX_SLEW_RATE_FAST	(0x0 << 2)	/* fast slew rate */
#define PINMUX_SLEW_RATE_SLOW	(0x1 << 2)	/* slow slew rate */

/*
 * Pin pull-up/pull-down configuration, for input:
 * At reset, the setting for PTA1/2/3/4/5 is pull-up; PTA0 is pull-down;
 * pull-up/pull-down is disabled for the remaining pins.
 */

#define PINMUX_PULL_EN_MASK		(0x1 << 1)		/* pullup/pulldown enable */
#define PINMUX_PULL_DISABLE		(0x0 << 1)		/* disable pullup/pulldown */
#define PINMUX_PULL_ENABLE		(0x1 << 1)		/* enable pullup/pulldown */

#define PINMUX_PULL_SEL_MASK	(0x1 << 0)		/* pullup/pulldown select */
#define PINMUX_PULL_DN			(0x0 << 0)		/* select pulldown */
#define PINMUX_PULL_UP			(0x1 << 0)		/* select pullup */


/*
 * Pin identification, by port and pin
 */

#define PIN_PTA0	0
#define PIN_PTA1	1
#define PIN_PTA2	2
#define PIN_PTA3	3
#define PIN_PTA4	4
#define PIN_PTA5	5
#define PIN_PTA6	6
#define PIN_PTA7	7
#define PIN_PTA8	8
#define PIN_PTA9	9
#define PIN_PTA10	10
#define PIN_PTA11	11
#define PIN_PTA12	12
#define PIN_PTA13	13
#define PIN_PTA14	14
#define PIN_PTA15	15
#define PIN_PTA16	16
#define PIN_PTA17	17
#define PIN_PTA18	18
#define PIN_PTA19	19
#define PIN_PTA20	20
#define PIN_PTA21	21
#define PIN_PTA22	22
#define PIN_PTA23	23
#define PIN_PTA24	24
#define PIN_PTA25	25
#define PIN_PTA26	26
#define PIN_PTA27	27
#define PIN_PTA28	28
#define PIN_PTA29	29
#define PIN_PTA30	30
#define PIN_PTA31	31

#define PIN_PTB0	32
#define PIN_PTB1	33
#define PIN_PTB2	34
#define PIN_PTB3	35
#define PIN_PTB4	36
#define PIN_PTB5	37
#define PIN_PTB6	38
#define PIN_PTB7	39
#define PIN_PTB8	40
#define PIN_PTB9	41
#define PIN_PTB10	42
#define PIN_PTB11	43
#define PIN_PTB12	44
#define PIN_PTB13	45
#define PIN_PTB14	46
#define PIN_PTB15	47
#define PIN_PTB16	48
#define PIN_PTB17	49
#define PIN_PTB18	50
#define PIN_PTB19	51
#define PIN_PTB20	52
#define PIN_PTB21	53
#define PIN_PTB22	54
#define PIN_PTB23	55
#define PIN_PTB24	56
#define PIN_PTB25	57
#define PIN_PTB26	58
#define PIN_PTB27	59
#define PIN_PTB28	60
#define PIN_PTB29	61
#define PIN_PTB30	62
#define PIN_PTB31	63

#define PIN_PTC0	64
#define PIN_PTC1	65
#define PIN_PTC2	66
#define PIN_PTC3	67
#define PIN_PTC4	68
#define PIN_PTC5	69
#define PIN_PTC6	70
#define PIN_PTC7	71
#define PIN_PTC8	72
#define PIN_PTC9	73
#define PIN_PTC10	74
#define PIN_PTC11	75
#define PIN_PTC12	76
#define PIN_PTC13	77
#define PIN_PTC14	78
#define PIN_PTC15	79
#define PIN_PTC16	80
#define PIN_PTC17	81
#define PIN_PTC18	82
#define PIN_PTC19	83
#define PIN_PTC20	84
#define PIN_PTC21	85
#define PIN_PTC22	86
#define PIN_PTC23	87
#define PIN_PTC24	88
#define PIN_PTC25	89
#define PIN_PTC26	90
#define PIN_PTC27	91
#define PIN_PTC28	92
#define PIN_PTC29	93
#define PIN_PTC30	94
#define PIN_PTC31	95

#define PIN_PTD0	96
#define PIN_PTD1	97
#define PIN_PTD2	98
#define PIN_PTD3	99
#define PIN_PTD4	100
#define PIN_PTD5	101
#define PIN_PTD6	102
#define PIN_PTD7	103
#define PIN_PTD8	104
#define PIN_PTD9	105
#define PIN_PTD10	106
#define PIN_PTD11	107
#define PIN_PTD12	108
#define PIN_PTD13	109
#define PIN_PTD14	110
#define PIN_PTD15	111
#define PIN_PTD16	112
#define PIN_PTD17	113
#define PIN_PTD18	114
#define PIN_PTD19	115
#define PIN_PTD20	116
#define PIN_PTD21	117
#define PIN_PTD22	118
#define PIN_PTD23	119
#define PIN_PTD24	120
#define PIN_PTD25	121
#define PIN_PTD26	122
#define PIN_PTD27	123
#define PIN_PTD28	124
#define PIN_PTD29	125
#define PIN_PTD30	126
#define PIN_PTD31	127

#define PIN_PTE0	128
#define PIN_PTE1	129
#define PIN_PTE2	130
#define PIN_PTE3	131
#define PIN_PTE4	132
#define PIN_PTE5	133
#define PIN_PTE6	134
#define PIN_PTE7	135
#define PIN_PTE8	136
#define PIN_PTE9	137
#define PIN_PTE10	138
#define PIN_PTE11	139
#define PIN_PTE12	140
#define PIN_PTE13	141
#define PIN_PTE14	142
#define PIN_PTE15	143
#define PIN_PTE16	144
#define PIN_PTE17	145
#define PIN_PTE18	146
#define PIN_PTE19	147
#define PIN_PTE20	148
#define PIN_PTE21	149
#define PIN_PTE22	150
#define PIN_PTE23	151
#define PIN_PTE24	152
#define PIN_PTE25	153
#define PIN_PTE26	154
#define PIN_PTE27	155
#define PIN_PTE28	156
#define PIN_PTE29	157
#define PIN_PTE30	158
#define PIN_PTE31	159

#endif /* __INCLUDE_PINMUX_K64_H */
