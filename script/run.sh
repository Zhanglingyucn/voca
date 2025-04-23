#1、准备工作：
#1）从git上下载代码
#下载数据，将下载下来的csv数据，后面生成的模拟数据，也以csv格式命名，放到 data/tvdata/下


#创建相关目录
mkdir data images m4_result compareresult  data/tvdata/ data/duckdata/ data/om3data/ output/ cpp/voca-server-c/om3_data/ -p

#2）生成 om3系数数据
#在cpp/om3-compress 下编译代码，压缩

#！！！！！将生成的om3数据放到 om3-compres-online下

#2、运行static实验程序

cd script/

#清空当前结果缓存
rm ../m4_result/* -rf
rm ../images/* -rf
rm ../compareresult/* -rf

#运行duckdb程序，作为baseline。
node ./applications.js static | grep experiment | awk 'BEGIN{print "experiment,table,function,width,time,memory(kb)"}{gsub(/,/, "I"); print $2,$5,$20,$23,$38,$41}' OFS="," > ../output/duck_static.csv



#cd到cpp/voca-server-c/om3-compress-online 目录下
cd ../cpp/voca-server-c/om3-compress-online
rm build -rf
 mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release ..
make


#开始运行 static 实验
rm ../m4_result/* -rf
./om3-compress static "" "nycdata 7,9,1,2,3,4,5,6,8,10,11" 16
./om3-compress static "" "soccerdata 5,3,1,2,4,6" 16
./om3-compress static "" "sensordata 5,4,3,2,1,7,6" 16
./om3-compress static "" "stockdata 9,5,1,2,3,4,6,7,8,10" 16

#将cpp程序生成的m4结果，mv到duck下的m4目录
#cd到根目录下，
cd ../../../../
mv cpp/voca-server-c/om3-compress-online/m4_result/* ./m4_result/

#根据m4结果，生成图片,结果在images
python script/createPhoto.py m4_result/ images/

#对比图片，生成ssim对比结果，保存到compareresult目录
python script/comparePhoto.py images/ compareresult/

ls compareresult/ | grep ours-cpp | awk -F'_' 'BEGIN{print "experiment,table,function,errorbound,width,time,ssim,memory(kb)"}{gsub(/,/, "I"); print $1,$2,$5,$10,$8,$14,$12,$16}' OFS=","| awk -F'.png' '{print $1}' > ./output/ours-cpp_static.csv


#3、运行interaction实验，interaction的执行计划保存在 script/plans.csv

cd script/

#清空当前结果缓存
rm ../m4_result/* -rf
rm ../images/* -rf
rm ../compareresult/* -rf

#运行duckdb程序，作为baseline。

node ./applications.js interactions | grep experiment | awk 'BEGIN{print "experiment,table,function,width,time,memory(kb)"}{gsub(/,/, "I"); print $2,$5,$20,$23,$38,$41}' OFS="," > ../output/duck_interaction.csv


#运行cpp程序
#cd到cpp/voca-server-c/om3-compress-online/build 目录下
cd ../cpp/voca-server-c/om3-compress-online/build
rm ../m4_result/* -rf
./om3-compress interactions ../../../../script/plans.csv "interactions" 16



cd ../../../../
mv cpp/voca-server-c/om3-compress-online/m4_result/* ./m4_result/

#根据m4结果，生成图片,结果在images
python script/createPhoto.py m4_result/ images/

#对比图片，生成ssim对比结果，保存到compareresult目录
python script/comparePhoto.py images/ compareresult/

ls compareresult/ | grep ours-cpp | awk -F'_' 'BEGIN{print "experiment,table,function,errorbound,width,time,ssim,memory(kb)"}{gsub(/,/, "I"); print $1,$2,$5,$10,$8,$14,$12,$16}' OFS=","| awk -F'.png' '{print $1}' > ./output/ours-cpp_interaction.csv



# 4、运行网页版交互程序(确保3000端口未被占用)


# 启动 cpp后端
cd到cpp/voca-server-c/ 目录下
cd cpp/voca-server-c/
rm build -rf
mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release ..
make

#！！！！！将nycdata的om3数据放到 voca-server-c/om3_data/ 下
cp om3-compress-online/om3_data/nycdata_om3* ./om3_data/

./voca-server-c



# 启动 client---- js版本
