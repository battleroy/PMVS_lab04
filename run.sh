sudo rm -f /dev/calcdev_*
sudo rmmod chrdev
make
sudo insmod ./chrdev.ko
modinfo ./chrdev.ko

sudo mknod /dev/calcdev_0 c 200 0
sudo mknod /dev/calcdev_1 c 200 1
sudo mknod /dev/calcdev_2 c 200 2
sudo 	mknod /dev/calcdev_3 c 200 3

sudo chmod a+r+w /dev/calcdev_*

echo "3 + 2"
echo "3" > /dev/calcdev_0
echo "2" > /dev/calcdev_1
echo "+" > /dev/calcdev_2
cat /dev/calcdev_3
echo "----------------------------"
echo "3 - 2"
echo "3" > /dev/calcdev_0
echo "2" > /dev/calcdev_1
echo "-" > /dev/calcdev_2
cat /dev/calcdev_3
echo "----------------------------"
echo "3 * 2"
echo "3" > /dev/calcdev_0
echo "2" > /dev/calcdev_1
echo "*" > /dev/calcdev_2
cat /dev/calcdev_3
echo "----------------------------"
echo "3 / 2"
echo "3" > /dev/calcdev_0
echo "2" > /dev/calcdev_1
echo "/" > /dev/calcdev_2
cat /dev/calcdev_3
echo "----------------------------"
echo "3 / 0"
echo "3" > /dev/calcdev_0
echo "0" > /dev/calcdev_1
echo "/" > /dev/calcdev_2
cat /dev/calcdev_3
echo "----------------------------"
echo "a - 2"
echo "a" > /dev/calcdev_0
echo "2" > /dev/calcdev_1
echo "-" > /dev/calcdev_2
cat /dev/calcdev_3
echo "----------------------------"
echo "1 - 5"
echo "1" > /dev/calcdev_0
echo "5" > /dev/calcdev_1
echo "-" > /dev/calcdev_2
cat /dev/calcdev_3
echo "----------------------------"

sudo rmmod chrdev
sudo rm -f /dev/calcdev_*
make clean