export PATH=$PATH:$HOME/os161/tools/bin

cd src/kern/conf
./config ASST1 
cd ../compile/ASST1
bmake depend 
bmake 
bmake install

cd ~/os161/root
ls
sys161 kernel-ASST1 "1b"
