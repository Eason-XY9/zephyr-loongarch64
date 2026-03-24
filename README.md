## 项目介绍
Zephyr操作系统基于小内存内核，专为资源受限和嵌入式系统而设计：从简单的嵌入式环境传感器和LED可穿戴设备到复杂的嵌入式控制器、智能手表和物联网无线应用。LoongArch是由我国龙芯中科研发的自主指令系统（龙芯架构）。本项目将 zephyr 初步移植到LoongArch平台上，在Ubuntu 22.04中通过 `QEMU` 模拟器（在 PC 上模拟 LoongArch 硬件）编译 zephyr 并运行。  
## 实验环境配置  
所有实验须在 `linux` 系统上运行，因此需要搭建虚拟机平台，包含实验需要的开发环境，例如 `Linux` 环境、交叉编译器、`loongarch64` 仿真器等。  
### 虚拟机平台以及镜像
虚拟机平台使用 VMware Workstation Pro，镜像使用的是 ubuntu22.04，二者均可到官网下载，这里不再赘述安装过程。  
### 交叉编译器
运行 `zephyr_la64` 所需要的交叉编译器为 `loongarch64-unknown-linux-gnu-`，可使用以下命令进行安装
```bash  
wget https://github.com/loongson/build-tools/releases/download/2023.08.08/x86_64-cross-tools-loongarch64-gcc-libc.tar.xz
tar xvf ./x86_64-cross-tools-loongarch64-gcc-libc.tar.xz
```  
解压完成后使用 `sudo vim /etc/profile` 指令进入配置文件，在文件末尾添加如下两行代码：  
```bash  
export CROSS_COMPILE=loongarch64-unknown-linux-gnu-
export PATH=/path/of/your/cross/compile/cross-tools/bin:$PATH
```  
保存退出后，重启虚拟机或使用如下指令重新应用配置文件：
```bash  
source /etc/profile
```  
重启系统后，使用以下命令检查软件安装情况：  
```sh  
$ loongarch64-unknown-linux-gnu-gcc --version
#出现以下版本信息则说明交叉编译器安装完成  
loongarch64-unknown-linux-gnu-gcc (GCC) 14.0.0 20231018 (experimental)  
Copyright (C) 2023 Free Software Foundation, Inc.  
This is free software; see the source for copying conditions.  There is NO  
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
```
### loongarch64仿真器 
实验中使用的硬件模拟器是 QEMU，下面介绍如何进行安装  
下载 QEMU 源码包  
```bash
$ wget https://download.qemu.org/qemu-9.0.2.tar.xz  
```  
解压  
```sh  
$ tar -xvf qemu-9.0.2.tar.xz  
```  
编译  
```bash  
$ cd qemu-9.0.2  
$ ../configure \  
  --target-list=loongarch64-softmmu \  
  --enable-slirp  
$ make -j24  
```  
完成后进行路径添加，使用`vim ~/.bashrc`指令进入配置文件，在文件末尾添加如下代码：  
```bash  
export PATH=/your/qemu/path/qemu-9.2.0/build:$PATH  
```  
保存退出后，重启虚拟机或使用如下指令重新应用配置文件：
```bash  
source ~/.bashrc
```  
重启系统后，使用以下命令检查QEMU安装情况： 
```bash  
$ qemu-system-loongarch64 -M ?  
#出现以下信息则说明qemu安装成功  
Supported machines are:  
none                 empty machine  
virt                 QEMU LoongArch Virtual Machine (default)  
```  
搭建好环境后，即可开始进行我们的实验。  
## 编译测试样例  
+ 进入 zephyr-loongarch/samples/philosophers 目录中
+ 使用 make clean 清理构建的输出内容  
+ 使用 make BOARD=qemu_loongarch64 进行编译
+ 使用 make run 运行 qemu

测试样例生成的 elf 文件位于 zephyr-loongarch/samples/philosophers/outdir/qemu_loongarch64 中