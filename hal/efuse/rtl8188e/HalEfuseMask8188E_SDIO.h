// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2007 - 2011 Realtek Corporation. */



/******************************************************************************
*                           MSDIO.TXT
******************************************************************************/


u2Byte
EFUSE_GetArrayLen_MP_8188E_MSDIO(VOID);

VOID
EFUSE_GetMaskArray_MP_8188E_MSDIO(
	IN 	OUT pu1Byte Array
);

BOOLEAN
EFUSE_IsAddressMasked_MP_8188E_MSDIO( // TC: Test Chip, MP: MP Chip
	IN   u2Byte  Offset
);


