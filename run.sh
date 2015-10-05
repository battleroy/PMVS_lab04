sudo rmmod chrdev;
make;
sudo insmod ./chrdev.ko;
echo "5" > /dev/calcdev_0;
echo "5" > /dev/calcdev_1;
echo "+" > /dev/calcdev_2;