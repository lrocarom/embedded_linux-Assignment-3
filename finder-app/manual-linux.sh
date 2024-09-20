#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

export PATH=$PATH:/home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/bin
sudo apt install qemu-system-arm

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-


if [ $# -lt 1 ]
then
    echo "Using default directory ${OUTDIR} for output"
else
    OUTDIR=$1
    echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
    echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
    git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here

#    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper

#    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig

 #   make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all
     
 #   make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- modules

 #   make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs
    
  #  cp ${OUTDIR}/linux-stable/arch/arm64/boot/Image ${OUTDIR}


    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
    make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs

    cp ${OUTDIR}/linux-stable/arch/arm64/boot/Image ${OUTDIR}


fi


echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
    echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi



# TODO: Create necessary base directories

mkdir -p "${OUTDIR}/rootfs/bin"
mkdir -p "${OUTDIR}/rootfs/etc"
mkdir -p "${OUTDIR}/rootfs/lib"
mkdir -p "${OUTDIR}/rootfs/lib64"
mkdir -p "${OUTDIR}/rootfs/dev"
mkdir -p "${OUTDIR}/rootfs/proc"
mkdir -p "${OUTDIR}/rootfs/proc"
mkdir -p "${OUTDIR}/rootfs/tmp"
mkdir -p "${OUTDIR}/rootfs/home/root"


#cd "$OUTDIR"

#mkdir -p "rootfs/bin" "rootfs/etc" "rootfs/home/root" "rootfs/lib" "rootfs/dev" "rootfs/proc" "rootfs/tmp" "rootfs/usr/bin" "rootfs/usr/lib"

if [ ! -d "${OUTDIR}/busybox" ] 
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
    make defconfig
    make ARCH=${ARCH}
    CROSS_COMPILE=${CROSS_COMPILE}
    make CONFIG_PREFIX="${OUTDIR}/rootfs"  ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install
else
    cd busybox
fi



# TODO: Make and install busybox


cd "${OUTDIR}/rootfs/"

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
sudo cp /home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib/ld-linux-aarch64.so.1  lib

sudo cp /home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libm.so.6   lib64
sudo cp /home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libresolv.so.2  lib64
sudo cp /home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib64/libc.so.6  lib64


# TODO: Make device nodes

sudo mknod -m 666 dev/null c 1 3

sudo mknod -m 600 dev/console c 5 1


# TODO: Clean and build the writer utility




# TODO: Copy the finder related scripts and executables to the /home directory

mkdir -p home/root/conf

cp /home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/T2/embedded_linux-Assignment-3/unit-test.sh home/

cp /home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/T2/embedded_linux-Assignment-3/conf/username.txt home/conf

cp /home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/T2/embedded_linux-Assignment-3/conf/assignment.txt home/conf

cp /home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/T2/embedded_linux-Assignment-3/finder-app/finder-test.sh home/


cp /home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/T2/embedded_linux-Assignment-3/finder-app/autorun-qemu.sh home/



cp /home/lroca/Documentos/FORMACIONS/linux_introduction_to_buildroot/T1/linux_training/finder-app/writer home/root/
# TODO: Chown the root directory

chmod 744 home/root

# TODO: Create initramfs.cpio.gz

find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio

cd ${OUTDIR}

gzip -f initramfs.cpio