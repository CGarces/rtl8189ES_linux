/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

/*============================================================	*/
/* include files												*/
/*============================================================	*/
#include "phydm_precomp.h"

//============================================================
// Global var
//============================================================

u4Byte	OFDMSwingTable[OFDM_TABLE_SIZE] = {
	0x7f8001fe,	/* 0, +6.0dB  */
	0x788001e2,	/* 1, +5.5dB */
	0x71c001c7,	/* 2, +5.0dB*/
	0x6b8001ae,	/* 3, +4.5dB*/
	0x65400195,	/* 4, +4.0dB*/
	0x5fc0017f,	/* 5, +3.5dB*/
	0x5a400169,	/* 6, +3.0dB*/
	0x55400155,	/* 7, +2.5dB*/
	0x50800142,	/* 8, +2.0dB*/
	0x4c000130,	/* 9, +1.5dB*/
	0x47c0011f,	/* 10, +1.0dB*/
	0x43c0010f,	/* 11, +0.5dB*/
	0x40000100,	/* 12, +0dB*/
	0x3c8000f2,	/* 13, -0.5dB*/
	0x390000e4,	/* 14, -1.0dB*/
	0x35c000d7,	/* 15, -1.5dB*/
	0x32c000cb,	/* 16, -2.0dB*/
	0x300000c0,	/* 17, -2.5dB*/
	0x2d4000b5,	/* 18, -3.0dB*/
	0x2ac000ab,	/* 19, -3.5dB*/
	0x288000a2,	/* 20, -4.0dB*/
	0x26000098,	/* 21, -4.5dB*/
	0x24000090,	/* 22, -5.0dB*/
	0x22000088,	/* 23, -5.5dB*/
	0x20000080,	/* 24, -6.0dB*/
	0x1e400079,	/* 25, -6.5dB*/
	0x1c800072,	/* 26, -7.0dB*/
	0x1b00006c,	/* 27. -7.5dB*/
	0x19800066,	/* 28, -8.0dB*/
	0x18000060,	/* 29, -8.5dB*/
	0x16c0005b,	/* 30, -9.0dB*/
	0x15800056,	/* 31, -9.5dB*/
	0x14400051,	/* 32, -10.0dB*/
	0x1300004c,	/* 33, -10.5dB*/
	0x12000048,	/* 34, -11.0dB*/
	0x11000044,	/* 35, -11.5dB*/
	0x10000040,	/* 36, -12.0dB*/
};

u1Byte	CCKSwingTable_Ch1_Ch13[CCK_TABLE_SIZE][8] = {
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04},	/* 0, +0dB  */
	{0x33, 0x32, 0x2b, 0x23, 0x1a, 0x11, 0x08, 0x04},	/* 1, -0.5dB */
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},	/* 2, -1.0dB*/
	{0x2d, 0x2d, 0x27, 0x1f, 0x18, 0x0f, 0x08, 0x03},	/* 3, -1.5dB*/
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},	/* 4, -2.0dB */
	{0x28, 0x28, 0x22, 0x1c, 0x15, 0x0d, 0x07, 0x03},	/* 5, -2.5dB*/
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},	/* 6, -3.0dB*/
	{0x24, 0x23, 0x1f, 0x19, 0x13, 0x0c, 0x06, 0x03},	/* 7, -3.5dB*/
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},	/* 8, -4.0dB */
	{0x20, 0x20, 0x1b, 0x16, 0x11, 0x08, 0x05, 0x02},	/* 9, -4.5dB*/
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},	/* 10, -5.0dB */
	{0x1d, 0x1c, 0x18, 0x14, 0x0f, 0x0a, 0x05, 0x02},	/* 11, -5.5dB*/
	{0x1b, 0x1a, 0x17, 0x13, 0x0e, 0x09, 0x04, 0x02},	/* 12, -6.0dB <== default */
	{0x1a, 0x19, 0x16, 0x12, 0x0d, 0x09, 0x04, 0x02},	/* 13, -6.5dB*/
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},	/* 14, -7.0dB */
	{0x17, 0x16, 0x13, 0x10, 0x0c, 0x08, 0x04, 0x02},	/* 15, -7.5dB*/
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},	/* 16, -8.0dB */
	{0x14, 0x14, 0x11, 0x0e, 0x0b, 0x07, 0x03, 0x02},	/* 17, -8.5dB*/
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},	/* 18, -9.0dB */
	{0x12, 0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	/* 19, -9.5dB*/
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	/* 20, -10.0dB*/
	{0x10, 0x10, 0x0e, 0x0b, 0x08, 0x05, 0x03, 0x01},	/* 21, -10.5dB*/
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01},	/* 22, -11.0dB*/
	{0x0e, 0x0e, 0x0c, 0x0a, 0x08, 0x05, 0x02, 0x01},	/* 23, -11.5dB*/
	{0x0d, 0x0d, 0x0c, 0x0a, 0x07, 0x05, 0x02, 0x01},	/* 24, -12.0dB*/
	{0x0d, 0x0c, 0x0b, 0x09, 0x07, 0x04, 0x02, 0x01},	/* 25, -12.5dB*/
	{0x0c, 0x0c, 0x0a, 0x09, 0x06, 0x04, 0x02, 0x01},	/* 26, -13.0dB*/
	{0x0b, 0x0b, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x01},	/* 27, -13.5dB*/
	{0x0b, 0x0a, 0x09, 0x08, 0x06, 0x04, 0x02, 0x01},	/* 28, -14.0dB*/
	{0x0a, 0x0a, 0x09, 0x07, 0x05, 0x03, 0x02, 0x01},	/* 29, -14.5dB*/
	{0x0a, 0x09, 0x08, 0x07, 0x05, 0x03, 0x02, 0x01},	/* 30, -15.0dB*/
	{0x09, 0x09, 0x08, 0x06, 0x05, 0x03, 0x01, 0x01},	/* 31, -15.5dB*/
	{0x09, 0x08, 0x07, 0x06, 0x04, 0x03, 0x01, 0x01}	/* 32, -16.0dB*/
};


u1Byte	CCKSwingTable_Ch14[CCK_TABLE_SIZE][8] = {
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00},	/* 0, +0dB  */
	{0x33, 0x32, 0x2b, 0x19, 0x00, 0x00, 0x00, 0x00},	/* 1, -0.5dB */
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},	/* 2, -1.0dB  */
	{0x2d, 0x2d, 0x17, 0x17, 0x00, 0x00, 0x00, 0x00},	/* 3, -1.5dB*/
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},	/* 4, -2.0dB  */
	{0x28, 0x28, 0x24, 0x14, 0x00, 0x00, 0x00, 0x00},	/* 5, -2.5dB*/
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},	/* 6, -3.0dB  */
	{0x24, 0x23, 0x1f, 0x12, 0x00, 0x00, 0x00, 0x00},	/* 7, -3.5dB  */
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},	/* 8, -4.0dB  */
	{0x20, 0x20, 0x1b, 0x10, 0x00, 0x00, 0x00, 0x00},	/* 9, -4.5dB*/
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},	/* 10, -5.0dB */ 
	{0x1d, 0x1c, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},	/* 11, -5.5dB*/
	{0x1b, 0x1a, 0x17, 0x0e, 0x00, 0x00, 0x00, 0x00},	/* 12, -6.0dB  <== default*/
	{0x1a, 0x19, 0x16, 0x0d, 0x00, 0x00, 0x00, 0x00},	/* 13, -6.5dB */
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},	/* 14, -7.0dB  */
	{0x17, 0x16, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00},	/* 15, -7.5dB*/
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},	/* 16, -8.0dB  */
	{0x14, 0x14, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x00},	/* 17, -8.5dB*/
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},	/* 18, -9.0dB  */
	{0x12, 0x12, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	/* 19, -9.5dB*/
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	/* 20, -10.0dB*/
	{0x10, 0x10, 0x0e, 0x08, 0x00, 0x00, 0x00, 0x00},	/* 21, -10.5dB*/
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00},	/* 22, -11.0dB*/
	{0x0e, 0x0e, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},	/* 23, -11.5dB*/
	{0x0d, 0x0d, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},	/* 24, -12.0dB*/
	{0x0d, 0x0c, 0x0b, 0x06, 0x00, 0x00, 0x00, 0x00},	/* 25, -12.5dB*/
	{0x0c, 0x0c, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},	/* 26, -13.0dB*/
	{0x0b, 0x0b, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},	/* 27, -13.5dB*/
	{0x0b, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},	/* 28, -14.0dB*/
	{0x0a, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},	/* 29, -14.5dB*/
	{0x0a, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},	/* 30, -15.0dB*/
	{0x09, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},	/* 31, -15.5dB*/
	{0x09, 0x08, 0x07, 0x04, 0x00, 0x00, 0x00, 0x00}	/* 32, -16.0dB*/
};


u4Byte OFDMSwingTable_New[OFDM_TABLE_SIZE] = {
	0x0b40002d, /* 0,  -15.0dB	*/
	0x0c000030, /* 1,  -14.5dB*/
	0x0cc00033, /* 2,  -14.0dB*/
	0x0d800036, /* 3,  -13.5dB*/
	0x0e400039, /* 4,  -13.0dB  */  
	0x0f00003c, /* 5,  -12.5dB*/
	0x10000040, /* 6,  -12.0dB*/
	0x11000044, /* 7,  -11.5dB*/
	0x12000048, /* 8,  -11.0dB*/
	0x1300004c, /* 9,  -10.5dB*/
	0x14400051, /* 10, -10.0dB*/
	0x15800056, /* 11, -9.5dB*/
	0x16c0005b, /* 12, -9.0dB*/
	0x18000060, /* 13, -8.5dB*/
	0x19800066, /* 14, -8.0dB*/
	0x1b00006c, /* 15, -7.5dB*/
	0x1c800072, /* 16, -7.0dB*/
	0x1e400079, /* 17, -6.5dB*/
	0x20000080, /* 18, -6.0dB*/
	0x22000088, /* 19, -5.5dB*/
	0x24000090, /* 20, -5.0dB*/
	0x26000098, /* 21, -4.5dB*/
	0x288000a2, /* 22, -4.0dB*/
	0x2ac000ab, /* 23, -3.5dB*/
	0x2d4000b5, /* 24, -3.0dB*/
	0x300000c0, /* 25, -2.5dB*/
	0x32c000cb, /* 26, -2.0dB*/
	0x35c000d7, /* 27, -1.5dB*/
	0x390000e4, /* 28, -1.0dB*/
	0x3c8000f2, /* 29, -0.5dB*/
	0x40000100, /* 30, +0dB*/
	0x43c0010f, /* 31, +0.5dB*/
	0x47c0011f, /* 32, +1.0dB*/
	0x4c000130, /* 33, +1.5dB*/
	0x50800142, /* 34, +2.0dB*/
	0x55400155, /* 35, +2.5dB*/
	0x5a400169, /* 36, +3.0dB*/
	0x5fc0017f, /* 37, +3.5dB*/
	0x65400195, /* 38, +4.0dB*/
	0x6b8001ae, /* 39, +4.5dB*/
	0x71c001c7, /* 40, +5.0dB*/
	0x788001e2, /* 41, +5.5dB*/
	0x7f8001fe  /* 42, +6.0dB*/
};               


u1Byte CCKSwingTable_Ch1_Ch14_88F[CCK_TABLE_SIZE_88F][16] = {
{0x44, 0x42, 0x3C, 0x33, 0x28, 0x1C, 0x13, 0x0B, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-16dB*/
{0x48, 0x46, 0x3F, 0x36, 0x2A, 0x1E, 0x14, 0x0B, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-15.5dB*/
{0x4D, 0x4A, 0x43, 0x39, 0x2C, 0x20, 0x15, 0x0C, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-15dB*/
{0x51, 0x4F, 0x47, 0x3C, 0x2F, 0x22, 0x16, 0x0D, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-14.5dB*/
{0x56, 0x53, 0x4B, 0x40, 0x32, 0x24, 0x17, 0x0E, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-14dB*/
{0x5B, 0x58, 0x50, 0x43, 0x35, 0x26, 0x19, 0x0E, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-13.5dB*/
{0x60, 0x5D, 0x54, 0x47, 0x38, 0x28, 0x1A, 0x0F, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-13dB*/
{0x66, 0x63, 0x59, 0x4C, 0x3B, 0x2B, 0x1C, 0x10, 0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-12.5dB*/
{0x6C, 0x69, 0x5F, 0x50, 0x3F, 0x2D, 0x1E, 0x11, 0x08, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-12dB*/
{0x73, 0x6F, 0x64, 0x55, 0x42, 0x30, 0x1F, 0x12, 0x08, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-11.5dB*/
{0x79, 0x76, 0x6A, 0x5A, 0x46, 0x33, 0x21, 0x13, 0x09, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-11dB*/
{0x81, 0x7C, 0x71, 0x5F, 0x4A, 0x36, 0x23, 0x14, 0x0A, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-10.5dB*/
{0x88, 0x84, 0x77, 0x65, 0x4F, 0x39, 0x25, 0x15, 0x0A, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-10dB*/
{0x90, 0x8C, 0x7E, 0x6B, 0x54, 0x3C, 0x27, 0x17, 0x0B, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-9.5dB*/
{0x99, 0x94, 0x86, 0x71, 0x58, 0x40, 0x2A, 0x18, 0x0B, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-9dB*/
{0xA2, 0x9D, 0x8E, 0x78, 0x5E, 0x43, 0x2C, 0x19, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-8.5dB*/
{0xAC, 0xA6, 0x96, 0x7F, 0x63, 0x47, 0x2F, 0x1B, 0x0D, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-8dB*/
{0xB6, 0xB0, 0x9F, 0x87, 0x69, 0x4C, 0x32, 0x1D, 0x0D, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-7.5dB*/
{0xC1, 0xBA, 0xA8, 0x8F, 0x6F, 0x50, 0x35, 0x1E, 0x0E, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-7dB*/
{0xCC, 0xC5, 0xB2, 0x97, 0x76, 0x55, 0x38, 0x20, 0x0F, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-6.5dB*/
{0xD8, 0xD1, 0xBD, 0xA0, 0x7D, 0x5A, 0x3B, 0x22, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}     /*-6dB*/
};


u1Byte CCKSwingTable_Ch1_Ch13_88F[CCK_TABLE_SIZE_88F][16] = {
{0x44, 0x42, 0x3C, 0x33, 0x28, 0x1C, 0x13, 0x0B, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-16dB*/
{0x48, 0x46, 0x3F, 0x36, 0x2A, 0x1E, 0x14, 0x0B, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-15.5dB*/
{0x4D, 0x4A, 0x43, 0x39, 0x2C, 0x20, 0x15, 0x0C, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-15dB*/
{0x51, 0x4F, 0x47, 0x3C, 0x2F, 0x22, 0x16, 0x0D, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-14.5dB*/
{0x56, 0x53, 0x4B, 0x40, 0x32, 0x24, 0x17, 0x0E, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-14dB*/
{0x5B, 0x58, 0x50, 0x43, 0x35, 0x26, 0x19, 0x0E, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-13.5dB*/
{0x60, 0x5D, 0x54, 0x47, 0x38, 0x28, 0x1A, 0x0F, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-13dB*/
{0x66, 0x63, 0x59, 0x4C, 0x3B, 0x2B, 0x1C, 0x10, 0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-12.5dB*/
{0x6C, 0x69, 0x5F, 0x50, 0x3F, 0x2D, 0x1E, 0x11, 0x08, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-12dB*/
{0x73, 0x6F, 0x64, 0x55, 0x42, 0x30, 0x1F, 0x12, 0x08, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-11.5dB*/
{0x79, 0x76, 0x6A, 0x5A, 0x46, 0x33, 0x21, 0x13, 0x09, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-11dB*/
{0x81, 0x7C, 0x71, 0x5F, 0x4A, 0x36, 0x23, 0x14, 0x0A, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-10.5dB*/
{0x88, 0x84, 0x77, 0x65, 0x4F, 0x39, 0x25, 0x15, 0x0A, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-10dB*/
{0x90, 0x8C, 0x7E, 0x6B, 0x54, 0x3C, 0x27, 0x17, 0x0B, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-9.5dB*/
{0x99, 0x94, 0x86, 0x71, 0x58, 0x40, 0x2A, 0x18, 0x0B, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-9dB*/
{0xA2, 0x9D, 0x8E, 0x78, 0x5E, 0x43, 0x2C, 0x19, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-8.5dB*/
{0xAC, 0xA6, 0x96, 0x7F, 0x63, 0x47, 0x2F, 0x1B, 0x0D, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-8dB*/
{0xB6, 0xB0, 0x9F, 0x87, 0x69, 0x4C, 0x32, 0x1D, 0x0D, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-7.5dB*/
{0xC1, 0xBA, 0xA8, 0x8F, 0x6F, 0x50, 0x35, 0x1E, 0x0E, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-7dB*/
{0xCC, 0xC5, 0xB2, 0x97, 0x76, 0x55, 0x38, 0x20, 0x0F, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-6.5dB*/
{0xD8, 0xD1, 0xBD, 0xA0, 0x7D, 0x5A, 0x3B, 0x22, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}     /*-6dB*/
};


u1Byte CCKSwingTable_Ch14_88F[CCK_TABLE_SIZE_88F][16] = {
{0x44,	 0x42, 0x3C, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-16dB*/
{0x48, 0x46, 0x3F, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-15.5dB*/
{0x4D, 0x4A, 0x43, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-15dB*/
{0x51, 0x4F, 0x47, 0x2F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},	    /*-14.5dB*/
{0x56, 0x53, 0x4B, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-14dB*/
{0x5B, 0x58, 0x50, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-13.5dB*/
{0x60, 0x5D, 0x54, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-13dB*/
{0x66, 0x63, 0x59, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-12.5dB*/
{0x6C, 0x69, 0x5F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-12dB*/
{0x73, 0x6F, 0x64, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-11.5dB*/
{0x79, 0x76, 0x6A, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-11dB*/
{0x81, 0x7C, 0x71, 0x4A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-10.5dB*/
{0x88, 0x84, 0x77, 0x4F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-10dB*/
{0x90, 0x8C, 0x7E, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-9.5dB*/
{0x99, 0x94, 0x86, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-9dB*/
{0xA2, 0x9D, 0x8E, 0x5E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-8.5dB*/
{0xAC, 0xA6, 0x96, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-8dB*/
{0xB6, 0xB0, 0x9F, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-7.5dB*/
{0xC1, 0xBA, 0xA8, 0x6F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-7dB*/
{0xCC, 0xC5, 0xB2, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},    /*-6.5dB*/
{0xD8, 0xD1, 0xBD, 0x7D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}     /*-6dB*/
};


u1Byte CCKSwingTable_Ch1_Ch13_New[CCK_TABLE_SIZE][8] = {
	{0x09, 0x08, 0x07, 0x06, 0x04, 0x03, 0x01, 0x01},	/*  0, -16.0dB*/
	{0x09, 0x09, 0x08, 0x06, 0x05, 0x03, 0x01, 0x01},	/*   1, -15.5dB*/
	{0x0a, 0x09, 0x08, 0x07, 0x05, 0x03, 0x02, 0x01},	/*  2, -15.0dB*/
	{0x0a, 0x0a, 0x09, 0x07, 0x05, 0x03, 0x02, 0x01},	/*   3, -14.5dB*/
	{0x0b, 0x0a, 0x09, 0x08, 0x06, 0x04, 0x02, 0x01},	/*   4, -14.0dB*/
	{0x0b, 0x0b, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x01},	/*   5, -13.5dB*/
	{0x0c, 0x0c, 0x0a, 0x09, 0x06, 0x04, 0x02, 0x01},	/*   6, -13.0dB*/
	{0x0d, 0x0c, 0x0b, 0x09, 0x07, 0x04, 0x02, 0x01},	/*   7, -12.5dB*/
	{0x0d, 0x0d, 0x0c, 0x0a, 0x07, 0x05, 0x02, 0x01},	/*  8, -12.0dB*/
	{0x0e, 0x0e, 0x0c, 0x0a, 0x08, 0x05, 0x02, 0x01},	/*   9, -11.5dB*/
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01},	/*  10, -11.0dB*/
	{0x10, 0x10, 0x0e, 0x0b, 0x08, 0x05, 0x03, 0x01},	/*  11, -10.5dB*/
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	/*  12, -10.0dB*/
	{0x12, 0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	/*  13, -9.5dB*/
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},	/*  14, -9.0dB */
	{0x14, 0x14, 0x11, 0x0e, 0x0b, 0x07, 0x03, 0x02},	/*  15, -8.5dB*/
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},	/*  16, -8.0dB */
	{0x17, 0x16, 0x13, 0x10, 0x0c, 0x08, 0x04, 0x02},	/*  17, -7.5dB*/
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},	/*  18, -7.0dB */
	{0x1a, 0x19, 0x16, 0x12, 0x0d, 0x09, 0x04, 0x02},	/*  19, -6.5dB*/
    {0x1b, 0x1a, 0x17, 0x13, 0x0e, 0x09, 0x04, 0x02},	/*20, -6.0dB */
	{0x1d, 0x1c, 0x18, 0x14, 0x0f, 0x0a, 0x05, 0x02},	/*  21, -5.5dB*/
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},	/* 22, -5.0dB */
	{0x20, 0x20, 0x1b, 0x16, 0x11, 0x08, 0x05, 0x02},	/*  23, -4.5dB*/
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},	/*  24, -4.0dB */
	{0x24, 0x23, 0x1f, 0x19, 0x13, 0x0c, 0x06, 0x03},	/*  25, -3.5dB*/
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},	/*  26, -3.0dB*/
	{0x28, 0x28, 0x22, 0x1c, 0x15, 0x0d, 0x07, 0x03},	/*  27, -2.5dB*/
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},	/*  28, -2.0dB */
	{0x2d, 0x2d, 0x27, 0x1f, 0x18, 0x0f, 0x08, 0x03},	/*  29, -1.5dB*/
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},	/*  30, -1.0dB*/
	{0x33, 0x32, 0x2b, 0x23, 0x1a, 0x11, 0x08, 0x04},	/*  31, -0.5dB*/
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04} 	/*  32, +0dB*/
};                                                                  


u1Byte CCKSwingTable_Ch14_New[CCK_TABLE_SIZE][8]= {
	{0x09, 0x08, 0x07, 0x04, 0x00, 0x00, 0x00, 0x00},	/*  0, -16.0dB*/
	{0x09, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},	/* 1, -15.5dB*/
	{0x0a, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},	/*  2, -15.0dB*/
	{0x0a, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},	/* 3, -14.5dB*/
	{0x0b, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},	/*  4, -14.0dB*/
	{0x0b, 0x0b, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},	/*5, -13.5dB*/
	{0x0c, 0x0c, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},	/* 6, -13.0dB*/
	{0x0d, 0x0c, 0x0b, 0x06, 0x00, 0x00, 0x00, 0x00},	/*  7, -12.5dB*/
	{0x0d, 0x0d, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},	/* 8, -12.0dB*/
	{0x0e, 0x0e, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},	/* 9, -11.5dB*/
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00},	/* 10, -11.0dB*/
	{0x10, 0x10, 0x0e, 0x08, 0x00, 0x00, 0x00, 0x00},	/*11, -10.5dB*/
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	/* 12, -10.0dB*/
	{0x12, 0x12, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	/* 13, -9.5dB*/
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},	/*14, -9.0dB  */
	{0x14, 0x14, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x00},	/* 15, -8.5dB*/
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},	/* 16, -8.0dB  */
	{0x17, 0x16, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00},	/* 17, -7.5dB*/
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},	/* 18, -7.0dB  */
	{0x1a, 0x19, 0x16, 0x0d, 0x00, 0x00, 0x00, 0x00},	/* 19, -6.5dB */
	{0x1b, 0x1a, 0x17, 0x0e, 0x00, 0x00, 0x00, 0x00},	/* 20, -6.0dB  */
	{0x1d, 0x1c, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},	/* 21, -5.5dB*/
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},	/* 22, -5.0dB  */
	{0x20, 0x20, 0x1b, 0x10, 0x00, 0x00, 0x00, 0x00},	/*23, -4.5dB*/
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},	/* 24, -4.0dB  */
	{0x24, 0x23, 0x1f, 0x12, 0x00, 0x00, 0x00, 0x00},	/* 25, -3.5dB  */
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},	/* 26, -3.0dB  */
	{0x28, 0x28, 0x24, 0x14, 0x00, 0x00, 0x00, 0x00},	/*27, -2.5dB*/
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},	/* 28, -2.0dB  */
	{0x2d, 0x2d, 0x17, 0x17, 0x00, 0x00, 0x00, 0x00},	/*29, -1.5dB*/
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},	/* 30, -1.0dB  */
	{0x33, 0x32, 0x2b, 0x19, 0x00, 0x00, 0x00, 0x00},	/* 31, -0.5dB */
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00} 	/* 32, +0dB	*/
};

VOID
odm_TXPowerTrackingInit(
	IN	PVOID	pDM_VOID 
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	odm_TXPowerTrackingThermalMeterInit(pDM_Odm);
}	

u1Byte 
getSwingIndex(
	IN	PVOID	pDM_VOID 
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u1Byte 			i = 0;
	u4Byte 			bbSwing;
	u4Byte 			swingTableSize;
	pu4Byte 			pSwingTable;

	if (pDM_Odm->SupportICType == ODM_RTL8188E || pDM_Odm->SupportICType == ODM_RTL8723B
		|| pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8188F || pDM_Odm->SupportICType == ODM_RTL8703B
	) {
		bbSwing = PHY_QueryBBReg(Adapter, rOFDM0_XATxIQImbalance, 0xFFC00000);

		pSwingTable = OFDMSwingTable_New;
		swingTableSize = OFDM_TABLE_SIZE;
	} else {
		bbSwing = 0;
		pSwingTable = OFDMSwingTable;
		swingTableSize = OFDM_TABLE_SIZE;
	}

	for (i = 0; i < swingTableSize; ++i) {
		u4Byte tableValue = pSwingTable[i];
		
		if (tableValue >= 0x100000 )
			tableValue >>= 22;
		if (bbSwing == tableValue)
			break;
	}
	return i;
}

VOID
odm_TXPowerTrackingThermalMeterInit(
	IN	PVOID	pDM_VOID
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	u1Byte defaultSwingIndex = getSwingIndex(pDM_Odm);
	u1Byte 			p = 0;
	PODM_RF_CAL_T	pRFCalibrateInfo = &(pDM_Odm->RFCalibrateInfo);

	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	pRFCalibrateInfo->bTXPowerTracking = _TRUE;
	pRFCalibrateInfo->TXPowercount = 0;
	pRFCalibrateInfo->bTXPowerTrackingInit = _FALSE;

	if(pDM_Odm->mp_mode == FALSE)
		pRFCalibrateInfo->TxPowerTrackControl = _TRUE;
	else
		pRFCalibrateInfo->TxPowerTrackControl = _FALSE;	

	if(pDM_Odm->mp_mode == FALSE)
		pRFCalibrateInfo->TxPowerTrackControl = _TRUE;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_CALIBRATION, ODM_DBG_LOUD, ("pDM_Odm TxPowerTrackControl = %d\n", pRFCalibrateInfo->TxPowerTrackControl));

	//pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = TRUE;
	pRFCalibrateInfo->ThermalValue = pHalData->EEPROMThermalMeter;
	pRFCalibrateInfo->ThermalValue_IQK = pHalData->EEPROMThermalMeter;
	pRFCalibrateInfo->ThermalValue_LCK = pHalData->EEPROMThermalMeter;	

		/*The index of "0 dB" in SwingTable.*/
		if (pDM_Odm->SupportICType == ODM_RTL8188E || pDM_Odm->SupportICType == ODM_RTL8723B ||
			pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8703B) {
			pRFCalibrateInfo->DefaultOfdmIndex = (defaultSwingIndex >= OFDM_TABLE_SIZE) ? 30 : defaultSwingIndex;
			pRFCalibrateInfo->DefaultCckIndex = 20;	
		} else if (pDM_Odm->SupportICType == ODM_RTL8188F) {          /*add by Mingzhi.Guo  2015-03-23*/
			pRFCalibrateInfo->DefaultOfdmIndex = 28;							/*OFDM: -1dB*/
			pRFCalibrateInfo->DefaultCckIndex = 20;							/*CCK:-6dB*/
		} else {
			pRFCalibrateInfo->DefaultOfdmIndex = (defaultSwingIndex >= TXSCALE_TABLE_SIZE) ? 24 : defaultSwingIndex;
			pRFCalibrateInfo->DefaultCckIndex = 24;	
		}

	pRFCalibrateInfo->BbSwingIdxCckBase = pRFCalibrateInfo->DefaultCckIndex;
	pRFCalibrateInfo->CCK_index = pRFCalibrateInfo->DefaultCckIndex;
	
	for (p = ODM_RF_PATH_A; p < MAX_RF_PATH; ++p)
	{
		pRFCalibrateInfo->BbSwingIdxOfdmBase[p] = pRFCalibrateInfo->DefaultOfdmIndex;		
		pRFCalibrateInfo->OFDM_index[p] = pRFCalibrateInfo->DefaultOfdmIndex;		
		pRFCalibrateInfo->DeltaPowerIndex[p] = 0;
		pRFCalibrateInfo->DeltaPowerIndexLast[p] = 0;
		pRFCalibrateInfo->PowerIndexOffset[p] = 0;
	}
	pRFCalibrateInfo->Modify_TxAGC_Value_OFDM = 0;			
	pRFCalibrateInfo->Modify_TxAGC_Value_CCK = 0;			

}


VOID
ODM_TXPowerTrackingCheck(
	IN	PVOID	pDM_VOID
	)
{
	/* 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
	at the same time. In the stage2/3, we need to prive universal interface and merge all
	HW dynamic mechanism. */
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	odm_TXPowerTrackingCheckCE(pDM_Odm);
}

VOID
odm_TXPowerTrackingCheckCE(
	IN	PVOID	pDM_VOID
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;

	PADAPTER	Adapter = pDM_Odm->Adapter;

	if (!(pDM_Odm->SupportAbility & ODM_RF_TX_PWR_TRACK))
		return;

	if (!pDM_Odm->RFCalibrateInfo.TM_Trigger) {
		
		if (IS_HARDWARE_TYPE_8188E(Adapter) || IS_HARDWARE_TYPE_8188F(Adapter) 
			|| IS_HARDWARE_TYPE_8703B(Adapter) || IS_HARDWARE_TYPE_8814A(Adapter)
		) {
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_T_METER_NEW, (BIT17 | BIT16), 0x03);
		} else {
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_T_METER_OLD, bRFRegOffsetMask, 0x60);
		}

		pDM_Odm->RFCalibrateInfo.TM_Trigger = 1;
		return;
	} else {
		ODM_TXPowerTrackingCallback_ThermalMeter(Adapter);
		pDM_Odm->RFCalibrateInfo.TM_Trigger = 0;
	}
}

VOID
odm_TXPowerTrackingCheckMP(
	IN	PVOID	pDM_VOID
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
}


VOID
odm_TXPowerTrackingCheckAP(
	IN	PVOID	pDM_VOID
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
}


