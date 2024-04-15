
# GPU Kernel Module Demonstration

This document outlines the steps for compiling, loading, and unloading the `hello_gpu` kernel module. It includes  descriptions of the commands used, their outputs, and explanations of intermediate files created during the compilation.



## compiling for your GPU My gpu is 3090 


```
nvidia-smi
```


```


+---------------------------------------------------------------------------------------+
| NVIDIA-SMI 535.171.04             Driver Version: 535.171.04   CUDA Version: 12.2     |
|-----------------------------------------+----------------------+----------------------+
| GPU  Name                 Persistence-M | Bus-Id        Disp.A | Volatile Uncorr. ECC |
| Fan  Temp   Perf          Pwr:Usage/Cap |         Memory-Usage | GPU-Util  Compute M. |
|                                         |                      |               MIG M. |
|=========================================+======================+======================|
|   0  NVIDIA GeForce RTX 3090 Ti     Off | 00000000:01:00.0 Off |                  Off |
| 30%   24C    P8              30W / 480W |     10MiB / 24564MiB |      0%      Default |
|                                         |                      |                  N/A |
+-----------------------------------------+----------------------+----------------------+
|   1  NVIDIA GeForce RTX 3090 Ti     Off | 00000000:03:00.0 Off |                  Off |
|  0%   32C    P8              25W / 480W |     10MiB / 24564MiB |      0%      Default |
|                                         |                      |                  N/A |
+-----------------------------------------+----------------------+----------------------+

+---------------------------------------------------------------------------------------+
| Processes:                                                                            |
|  GPU   GI   CI        PID   Type   Process name                            GPU Memory |
|        ID   ID                                                             Usage      |
|=======================================================================================|
|    0   N/A  N/A      1652      G   /usr/lib/xorg/Xorg                            4MiB |
|    1   N/A  N/A      1652      G   /usr/lib/xorg/Xorg                            4MiB |
+---------------------------------------------------------------------------------------+
```


## find out arch of your CPU

```
❯ lscpu
```

```
❯ lscpu
Architecture:            x86_64
  CPU op-mode(s):        32-bit, 64-bit
  Address sizes:         48 bits physical, 48 bits virtual
  Byte Order:            Little Endian
CPU(s):                  32
  On-line CPU(s) list:   0-31
Vendor ID:               AuthenticAMD
  Model name:            AMD Ryzen 9 7950X 16-Core Processor
```
## Hence to compile open-gpu-kernel-modules

```
make modules -j$(nproc) \                                                                  
    CC=gcc-12 \
    LD=ld \
    AR=ar \
    CXX=g++-12 \
    OBJCOPY=objcopy

```


## Setup

The project is located in the `/home/data2/ronak/open-gpu-kernel-modules/small-test` directory and consists of the following files:
- `hello_gpu.c` - Source code for the kernel module that logs messages when loaded and unloaded.
- `Makefile` - Used to compile the kernel module, specifying how to build and clean up the module.

## Compilation

To compile the kernel module, navigate to the module's directory and use the `make` command.

### Command:
```bash
cd small-test
make
```

### Expected Output:
This output indicates that the module compilation process is invoking the compiler with settings suitable for building a kernel module.
```
make -C /lib/modules/6.5.0-27-generic/build M=/home/data2/ronak/small-test modules
make[1]: Entering directory '/usr/src/linux-headers-6.5.0-27-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-12 (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0
  You are using:           gcc-12 (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0
  CC [M]  /home/data2/ronak/small-test/hello_gpu.o
  MODPOST /home/data2/ronak/small-test/Module.symvers
  CC [M]  /home/data2/ronak/small-test/hello_gpu.mod.o
  LD [M]  /home/data2/ronak/small-test/hello_gpu.ko
  BTF [M] /home/data2/ronak/small-test/hello_gpu.ko
Skipping BTF generation for /home/data2/ronak/small-test/hello_gpu.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-6.5.0-27-generic'
```

### Intermediate Files Explained:
- `hello_gpu.o`: The object file generated from the source code.
- `Module.symvers`: Contains symbol information needed for dynamic linking.
- `hello_gpu.mod.o`: Object file for the module loader.
- `hello_gpu.ko`: The final kernel module file.

## Loading the Module

Once compiled, the module can be loaded into the Linux kernel using the `insmod` command.

### Command:
```bash
sudo insmod hello_gpu.ko
```

## Verifying Module Output

To verify that the module is functioning as expected, inspect the kernel logs using `dmesg`.

### Command:
```bash
sudo dmesg | tail
```

### Expected Output:
This output shows the messages logged by the kernel module when loaded and unloaded.
```
[ 3557.513125] Goodbye, world - from the GPU
[ 3670.314453] Hello, world - this is the GPU talking
[ 3674.742025] Goodbye, world - from the GPU
[ 3732.045270] Hello, world - this is the GPU talking
[ 3903.543716] Goodbye, world - from the GPU
[ 3906.506908] Hello, world - this is the GPU talking
[ 3915.940945] Goodbye, world - from the GPU
[ 4108.575311] Hello, world - this is the GPU talking
[ 4126.337889] Goodbye, world - from the GPU
[ 4962.565146] Hello, world - this is the GPU talking
```

## Unloading the Module

The module can be removed from the Linux kernel using the `rmmod` command.

### Command:
```bash
sudo rmmod hello_gpu
```

## Confirming Module Unload

After unloading, it's good practice to check the kernel logs to confirm the module was unloaded correctly.

### Command:
```bash
sudo dmesg | tail
```

### Expected Output:
```
[ 3670.314453] Hello, world - this is the GPU talking
[ 3674.742025] Goodbye, world - from the GPU
[ 3732.045270] Hello, world - this is the GPU talking
[ 3903.543716] Goodbye, world - from the GPU
[ 3906.506908] Hello, world - this is the GPU talking
[ 3915.940945] Goodbye, world - from the GPU
[ 4108.575311] Hello, world - this is the GPU talking
[ 4126.337889] Goodbye, world - from the GPU
[ 4962.565146] Hello, world - this is the GPU talking
[ 4990.360779] Goodbye, world - from the GPU
```



