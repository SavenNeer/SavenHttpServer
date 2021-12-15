workspace=`pwd`
cd $workspace
echo 清空runer文件夹
cd runer/
rm -rf *

# echo 编译提取spider程序和脚本
# cd $workspace
# cd spiderTest
# g++ Search_classroom.cpp -o Search_classroom -lcurl
# cd ..
# cp spiderTest/Search_classroom runer/
# cp spiderTest/jsonroomparse.py runer/

echo 添加index.html文件
cd $workspace
cp ./test/index.html runer/

echo 构建服务器程序
cd $workspace
cd build
rm -rf *
cmake ..
make
cd ..
cp build/SavenServer runer/
cp Screen启动方法.txt runer/
cd runer/
touch logfile.txt

echo 完成构建
echo 新构建的程序位于runer文件夹下

