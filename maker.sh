echo 构建项目并将最终程序拷贝到buildtest文件夹下
cd /home/ubuntu/MySever/
cd build
ls
rm -rf *
cmake ..
make
cd ..
cp ./build/SavenServer ./buildtest/
