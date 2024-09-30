#!/usr/bin/env bash

export current=$(pwd)
export nuttx=$current/..
export top=$current/../../..

cd $top
git submodule
git submodule init
git submodule update Ubiquitous/Nuttx_Fusion_XiUOS/apps
git submodule update Ubiquitous/Nuttx_Fusion_XiUOS/nuttx
cd $current

find $top -name Kconfig -exec dos2unix -q {} \;

cp -rf $current/nuttx $nuttx
cp -rf $current/apps $nuttx
cp -rf $nuttx/aiit_board/aiit-arm32-board  $nuttx/nuttx/boards/arm/stm32
cp -rf $nuttx/aiit_board/aiit-riscv64-board $nuttx/nuttx/boards/risc-v/k210
cp -rf $nuttx/aiit_board/xidatong-riscv64 $nuttx/nuttx/boards/risc-v/k210
cp -rf $nuttx/aiit_board/edu-riscv64 $nuttx/nuttx/boards/risc-v/k210
cp -rf $nuttx/aiit_board/xidatong-arm32 $nuttx/nuttx/boards/arm/imxrt
cp -rf $nuttx/aiit_board/hc32f4a0 $nuttx/nuttx/boards/arm/hc32
cp -rf $nuttx/aiit_board/sabre-lite $nuttx/nuttx/boards/arm/imx6

cd ../nuttx
