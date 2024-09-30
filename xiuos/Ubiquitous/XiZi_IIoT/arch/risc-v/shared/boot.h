/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiUOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/
#include "register_para.h"

.macro ZERO_X_REGISTERS
  xor zero, zero, zero
  xor ra, ra, ra
  xor sp, sp, sp
  xor gp, gp, gp
  xor tp, tp, tp
  xor t0, t0, t0
  xor t1, t1, t1
  xor t2, t2, t2
  xor s0, s0, s0
  xor s1, s1, s1
  xor a0, a0, a0
  xor a1, a1, a1
  xor a2, a2, a2
  xor a3, a3, a3
  xor a4, a4, a4
  xor a5, a5, a5
  xor a6, a6, a6
  xor a7, a7, a7
  xor s2, s2, s2
  xor s3, s3, s3
  xor s4, s4, s4
  xor s5, s5, s5
  xor s6, s6, s6
  xor s7, s7, s7
  xor s8, s8, s8
  xor s9, s9, s9
  xor s10, s10, s10
  xor s11, s11, s11
  xor t3, t3, t3
  xor t4, t4, t4
  xor t5, t5, t5
  xor t6, t6, t6
.endm

.macro ZERO_F_REGISTERS
  fssr x31

  fmv.d.x f0, x31
  FSubDS f0, f0, f0

  fmv.d.x f1, x31
  FSubDS f1, f1, f1

  fmv.d.x f2, x31
  FSubDS f2, f2, f2

  fmv.d.x f3, x31
  FSubDS f3, f3, f3

  fmv.d.x f4, x31
  FSubDS f4, f4, f4

  fmv.d.x f5, x31
  FSubDS f5, f5, f5

  fmv.d.x f6, x31
  FSubDS f6, f6, f6

  fmv.d.x f7, x31
  FSubDS f7, f7, f7

  fmv.d.x f8, x31
  FSubDS f8, f8, f8

  fmv.d.x f9, x31
  FSubDS f9, f9, f9

  fmv.d.x f10, x31
  FSubDS f10,f10,f10

  fmv.d.x f11, x31
  FSubDS f11,f11,f11

  fmv.d.x f12, x31
  FSubDS f12,f12,f12

  fmv.d.x f13, x31
  FSubDS f13,f13,f13

  fmv.d.x f14, x31
  FSubDS f14,f14,f14

  fmv.d.x f15, x31
  FSubDS f15,f15,f15

  fmv.d.x f16, x31
  FSubDS f16,f16,f16

  fmv.d.x f17, x31
  FSubDS f17,f17,f17

  fmv.d.x f18, x31
  FSubDS f18,f18,f18

  fmv.d.x f19, x31
  FSubDS f19,f19,f19

  fmv.d.x f20, x31
  FSubDS f20,f20,f20

  fmv.d.x f21, x31
  FSubDS f21,f21,f21

  fmv.d.x f22, x31
  FSubDS f22,f22,f22

  fmv.d.x f23, x31
  FSubDS f23,f23,f23

  fmv.d.x f24, x31
  FSubDS f24,f24,f24

  fmv.d.x f25, x31
  FSubDS f25,f25,f25

  fmv.d.x f26, x31
  FSubDS f26,f26,f26

  fmv.d.x f27, x31
  FSubDS f27,f27,f27

  fmv.d.x f28, x31
  FSubDS f28,f28,f28

  fmv.d.x f29, x31
  FSubDS f29,f29,f29

  fmv.d.x f30, x31
  FSubDS f30,f30,f30

  fmv.d.x f31, x31
  FSubDS f31,f31,f31
.endm

 .macro SAVE_X_REGISTERS

    addi   sp, sp, -32 * RegLength

    StoreD ra,   1 * RegLength(sp)

    csrr   ra, mstatus
    StoreD ra,   2 * RegLength(sp)

    csrr   ra, mepc
    StoreD ra, 0 * RegLength(sp)

    StoreD tp,   4 * RegLength(sp)
    StoreD t0,   5 * RegLength(sp)
    StoreD t1,   6 * RegLength(sp)
    StoreD t2,   7 * RegLength(sp)
    StoreD s0,   8 * RegLength(sp)
    StoreD s1,   9 * RegLength(sp)
    StoreD a0,  10 * RegLength(sp)
    StoreD a1,  11 * RegLength(sp)
    StoreD a2,  12 * RegLength(sp)
    StoreD a3,  13 * RegLength(sp)
    StoreD a4,  14 * RegLength(sp)
    StoreD a5,  15 * RegLength(sp)
    StoreD a6,  16 * RegLength(sp)
    StoreD a7,  17 * RegLength(sp)
    StoreD s2,  18 * RegLength(sp)
    StoreD s3,  19 * RegLength(sp)
    StoreD s4,  20 * RegLength(sp)
    StoreD s5,  21 * RegLength(sp)
    StoreD s6,  22 * RegLength(sp)
    StoreD s7,  23 * RegLength(sp)
    StoreD s8,  24 * RegLength(sp)
    StoreD s9,  25 * RegLength(sp)
    StoreD s10, 26 * RegLength(sp)
    StoreD s11, 27 * RegLength(sp)
    StoreD t3,  28 * RegLength(sp)
    StoreD t4,  29 * RegLength(sp)
    StoreD t5,  30 * RegLength(sp)
    StoreD t6,  31 * RegLength(sp)
.endm


