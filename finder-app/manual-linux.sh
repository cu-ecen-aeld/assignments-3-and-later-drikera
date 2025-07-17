#!/bin/bash
# Script outline to install and build kernel.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
# kernel_version is a branch
KERNEL_VERSION=v5.15.188
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
	git clone --depth 1 --branch ${KERNEL_VERSION} ${KERNEL_REPO} 
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout tags/${KERNEL_VERSION}

   # TODO: Add your kernel build steps here
   echo "# TODO: Add your kernel build steps here"
   make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- -j12 mrproper
   make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- -j12 defconfig
   make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- -j12 all
   # make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- -j12 modules
   make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- -j12 dtbs

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
echo "# TODO: Create necessary base directories"
mkdir -p ${OUTDIR}/rootfs/bin  ${OUTDIR}/rootfs/dev  ${OUTDIR}/rootfs/etc   \
         ${OUTDIR}/rootfs/home ${OUTDIR}/rootfs/lib  ${OUTDIR}/rootfs/lib64 \
         ${OUTDIR}/rootfs/proc ${OUTDIR}/rootfs/sbin ${OUTDIR}/rootfs/sys   \
         ${OUTDIR}/rootfs/tmp  ${OUTDIR}/rootfs/usr  ${OUTDIR}/rootfs/var

mkdir -p ${OUTDIR}/rootfs/usr/bin ${OUTDIR}/rootfs/usr/lib ${OUTDIR}/rootfs/usr/sbin
mkdir -p ${OUTDIR}/rootfs/var/log

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
    echo "Cloning busybox!"
    git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    echo "# TODO:  Configure busybox"
    make distclean
    make defconfig
else
    cd busybox
fi

# TODO: Make and install busybox
echo "# TODO: Make and install busybox"
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

echo "Library dependencies"
sudo chown root:root /usr/bin/busybox
sudo chmod u+s /usr/bin/busybox
${CROSS_COMPILE}readelf -a ${OUTDIR}/rootfs/bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a ${OUTDIR}/rootfs/bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
echo "# TODO: Add library dependencies to rootfs"
sysroot=$(${CROSS_COMPILE}gcc -print-sysroot)
prog_inter=$(${CROSS_COMPILE}readelf -a ${OUTDIR}/rootfs/bin/busybox | grep "program interpreter" | \
   tr " " "\n" | \
   grep ".so"  | \
   tr -d "]")
shar_lib=$(${CROSS_COMPILE}readelf -a ${OUTDIR}/rootfs/bin/busybox | grep "Shared library" | \
   tr " " "\n" | \
   grep ".so"  | \
   tr -d "[]")
# echo "prog_inter ${prog_inter}"
# echo "shar_lib ${shar_lib}"
cp ${sysroot}/${prog_inter} ${OUTDIR}/rootfs/lib
echo "${shar_lib}" | xargs -I % cp ${sysroot}/lib64/% ${OUTDIR}/rootfs/lib64

# TODO: Make device nodes
echo "# TODO: Make device nodes"
sudo mknod -m 666 ${OUTDIR}/rootfs/dev/null c 1 3
sudo mknod -m 666 ${OUTDIR}/rootfs/dev/console c 5 1

# TODO: Clean and build the writer utility
pushd /home/adriker/Documents/assignments-3-and-later-drikera/finder-app
echo "# TODO: Clean and build the writer utility"
make clean
make writer CROSS_COMPILE=aarch64-none-linux-gnu- 

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
echo "# TODO: Copy the finder related scripts and executables to the /home directory on the target rootfs"

cp *.sh     ${OUTDIR}/rootfs/home
cp writer   ${OUTDIR}/rootfs/home
mkdir -p ${OUTDIR}/rootfs/home/conf
cp ./conf/* ${OUTDIR}/rootfs/home/conf

#imgdir=$(find ${OUTDIR} -name "Image" | grep "arm64")
imgdir=$(find ${OUTDIR} -name "Image" )

#echo "imgdir ${imgdir}"
#echo "outdir ${OUTDIR}"

cp ${imgdir} ${OUTDIR}

# cat ${OUTDIR}/rootfs/home/finder-test.sh | awk '{gsub(/..\/conf\/assignment.txt/, "/conf/assignment.txt")}1' 
cat ${OUTDIR}/rootfs/home/finder-test.sh | \
   awk '{gsub(/..\/conf\/assignment.txt/, "./conf/assignment.txt")}1' \
   > ${OUTDIR}/rootfs/home/temp
mv ${OUTDIR}/rootfs/home/temp ${OUTDIR}/rootfs/home/finder-test.sh
sudo chmod +111 ${OUTDIR}/rootfs/home/finder-test.sh
popd
# TODO: Chown the root directory
echo "# TODO: Chown the root directory"
sudo chown -R root:root ${OUTDIR}/rootfs
# TODO: Create initramfs.cpio.gz
echo "# TODO: Create initramfs.cpio.gz"
cd "$OUTDIR/rootfs"
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
gzip -f ${OUTDIR}/initramfs.cpio
