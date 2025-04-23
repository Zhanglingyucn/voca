1、准备工作：
1）从git上下载代码
下载数据，将下载下来的csv数据，后面生成的模拟数据，也以csv格式命名，放到 data/tvdata/下

2）生成 om3系数数据

在cpp/om3-compress 下编译代码，压缩

将生成的om3数据放到 om3-compres-online下

2、运行static实验程序

cd script/

清空当前结果缓存
rm ../m4_result/*
rm ../images/*
rm ../compareresult/*

#运行duckdb程序，作为baseline。
node ./applications.js static | grep experiment | awk 'BEGIN{print "experiment,table,function,width,time,memory(kb)"}{gsub(/,/, "I"); print $2,$5,$20,$23,$38,$41}' OFS="," > ../output/duck_static.csv



cd到cpp/voca-server-c/om3-compress-online 目录下
rm build -rf
 mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release ..
make


#开始运行 static 实验
rm ../m4_result/*
./om3-compress static "" "nycdata 7,9,1,2,3,4,5,6,8,10,11" 16
./om3-compress static "" "soccerdata 5,3,1,2,4,6" 16
./om3-compress static "" "sensordata 5,4,3,2,1,7,6" 16
./om3-compress static "" "stockdata 9,5,1,2,3,4,6,7,8,10" 16

#将cpp程序生成的m4结果，mv到duck下的m4目录
cd到根目录下，
cd ../../../../
mv cpp/voca-server-c/om3-compress-online/m4_result/* ./m4_result/

#根据m4结果，生成图片,结果在images
python script/createPhoto.py m4_result/ images/

#对比图片，生成ssim对比结果，保存到compareresult目录
python script/comparePhoto.py images/ compareresult/

ls compareresult/ | grep ours-cpp | awk -F'_' 'BEGIN{print "experiment,table,function,errorbound,width,time,ssim,memory(kb)"}{gsub(/,/, "I"); print $1,$2,$5,$10,$8,$14,$12,$16}' OFS=","| awk -F'.png' '{print $1}' > ./output/ours-cpp_static.csv


3、运行interaction实验，interaction的执行计划保存在 script/plans.csv

cd script/

清空当前结果缓存
rm ../m4_result/*
rm ../images/*
rm ../compareresult/*

#运行duckdb程序，作为baseline。

node ./applications.js interactions | grep experiment | awk 'BEGIN{print "experiment,table,function,width,time,memory(kb)"}{gsub(/,/, "I"); print $2,$5,$20,$23,$38,$41}' OFS="," > ../output/duck_interaction.csv


#运行cpp程序
cd到cpp/voca-server-c/om3-compress-online/build 目录下
cd ../cpp/voca-server-c/om3-compress-online/build
rm ../m4_result/*
./om3-compress interactions ../../../../script/plans.csv "interactions" 16



cd ../../../../
mv cpp/voca-server-c/om3-compress-online/m4_result/* ./m4_result/

#根据m4结果，生成图片,结果在images
python script/createPhoto.py m4_result/ images/

#对比图片，生成ssim对比结果，保存到compareresult目录
python script/comparePhoto.py images/ compareresult/

ls compareresult/ | grep ours-cpp | awk -F'_' 'BEGIN{print "experiment,table,function,errorbound,width,time,ssim,memory(kb)"}{gsub(/,/, "I"); print $1,$2,$5,$10,$8,$14,$12,$16}' OFS=","| awk -F'.png' '{print $1}' > ./output/ours-cpp_interaction.csv



4、运行网页版交互程序(确保3000端口未被占用)
启动 client---- js版本

启动 cpp后端
cd到cpp/voca-server-c/ 目录下
cd cpp/voca-server-c/
rm build -rf
mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release ..
make

将nycdata的om3数据放到 voca-server-c/om3_data/ 下

./voca-server-c











#end ==================================







clear && cat ../output/static.txt | grep experiment |awk 'BEGIN {print "method,data,type,colums,func,start,end,width,height,errorbound,error,ssim,iterations,totaltime,sqltime"}{gsub(/,/, ";");print $2 "," $5 "," $8 "," $11 "," $20 "," $26 "," $29 "," $23 "," 600 "," $17 "," 0 "," 0 "," $32 "," $35 "," $38}'




1、数据库配置，在
按照postgres、influxdb数据库
修改 initdb/dbconfig.json文件中相应的数据库连接信息。

2、将数据写入数据库：
postgres：

（1）t-v数据
（3）om3数据
    sh init_postgress.sh 

（2）vldb数据
黄硕、刘柯岐补充；

influxDB：
	sh init_influx.sh


3、运行实验程序
（1）static实验
运行程序
node applications.js static | grep experiment > ../output/static.txt

处理实验结果
    处理耗时结果
    mkdir ../result
    cat ../output/static.txt | grep experiment > ../result/experiment.txt

    处理ssim结果
        生成图片
        python createPhoto.py ../m4_result/ ../images/
        
        生成ssm图片
        python comparePhoto.py ../images/ ../compareresult/

        提取ssim数值
         ll ../compareresult/ > ../result/ssim.txt


 耗时、ssim
ls ours_*static* | awk -F'.png' 'BEGIN {print "method,data,type,colums,func,start,end,width,height,errorbound,error,ssim,iterations,totaltime,sqltime"} {gsub(/,/, ";");gsub(/_/, ","); print $1}' > ../output/static.ssim.time.csv

nodecount：
cat ../output/static.txt | grep ours|grep static |awk 'BEGIN {print "method,data,type,colums,func,start,end,width,height,errorbound,iterations,nodecount"}{gsub(/,/, ";");print $2 "," $5 "," $8 "," $11 "," $20 "," $26 "," $29 "," $23 "," 600 "," $17 "," $32 "," $14}' > ../output/static.nodecount.csv



（2）interact实验

    运行程序


    处理实验结果


（3）vldb实验
    运行程序
    mvn clean package
    java -jar target/experiments.jar -path new_nycdata.csv -c initialize -type postgres -out output -schema more -table vldbnycdata -timeFormat "yyyy-MM-dd[ HH:mm:ss.SSS]"
    mvn exec:java -Dexec.mainClass="gr.imsi.athenarc.visual.middleware.service.impl.QueryServiceImpl" -Dexec.args="-queryPath plans_nyc.csv -type postgres -schema more -agg 4 -reduction 0 -p 1 -q 0.1 -c run"

    处理实验结果



（4） 新实验
    运行程序
    cd voca/script
    1. 直接比较序列数量对查询时间、准确性和迭代次数的影响
    node applications.js tsnum

    2. 直接比较数据量对于查询时间与准确性的影响
    node applications.js linenum

    处理实验结果
    python createPhoto.py ../m4_result/ ../images/ 
    python comparePhoto.py ../images/ ../compareresult/

    ls ../compareresult/ |grep tsnum| awk -F'.png' 'BEGIN {print "method,data,type,colums,func,start,end,width,height,errorbound,error,ssim,iterations,totaltime,sqltime"} {gsub(/,/, ";");gsub(/_/, ","); print $1}' > ../output/tsnum.csv
    ls ../compareresult/ |grep linenum| awk -F'.png' 'BEGIN {print "method,data,type,colums,func,start,end,width,height,errorbound,error,ssim,iterations,totaltime,sqltime"} {gsub(/,/, ";");gsub(/_/, ","); print $1}' > ../output/linenum.csv


（5）计算像素错误率
python computeerrorbound_multi.py ../m4_result/ ../m4_result/ | grep static > ../output/static.error.txt
cat ../output/static.error.txt | awk -F'.png' 'BEGIN {print "method,type,data,colums,func,start,end,width,height,errorbound,iterations,errorrate"} {gsub(/,/, ";");gsub(/_/, ","); print $1}' > ../output/static.error.csv
python mergefile_2.py 



(6)
处理其他baseline数据

cd compareresult 

ours:
ls ours_*static* | grep -v "func1" | grep -v "boxcox0" | grep -v "plus" | grep -v "minus" | grep -v "multi" | grep -v "devide" | grep -v "func4" | grep -v "mean" | grep -v "variance" | grep _600_600_0.05_  | awk -F'.png' '{gsub(/,/, ";");gsub(/_/, ","); print $1}' | awk -F',' 'BEGIN {print "errorbound,data,func,width,method,ssim,totaltime,sqltime"} {print $10 "," $2 "," $5 "," $8 "," $1 "," $12 "," $14 "," $15}' > ../output/ours.0.05.600.csv

PG-F:
ls PG-F_*static* | grep -v "func1" | grep -v "boxcox0" | grep -v "plus" | grep -v "minus" | grep -v "multi" | grep -v "devide" | grep -v "func4" | grep -v "mean" | grep -v "variance" | grep _600_600_0.05_  | awk -F'.png' '{gsub(/,/, ";");gsub(/_/, ","); print $1}' | awk -F',' 'BEGIN {print "errorbound,data,func,width,method,ssim,totaltime,sqltime"} {print $10 "," $2 "," $5 "," $8 "," $1 "," $12 "," $14 "," $15}' > ../output/PG-F.0.05.600.csv

PG-M4:
ls PG-M4_*static* | grep -v "func1" | grep -v "boxcox0" | grep -v "plus" | grep -v "minus" | grep -v "multi" | grep -v "devide" | grep -v "func4" | grep -v "mean" | grep -v "variance" | grep _600_600_0.05_  | awk -F'.png' '{gsub(/,/, ";");gsub(/_/, ","); print $1}' | awk -F',' 'BEGIN {print "errorbound,data,func,width,method,ssim,totaltime,sqltime"} {print $10 "," $2 "," $5 "," $8 "," $1 "," $12 "," $14 "," $15}' > ../output/PG-M4.0.05.600.csv

OM3:
ls OM3_*static* | grep -v "func1" | grep -v "boxcox0" | grep -v "plus" | grep -v "minus" | grep -v "multi" | grep -v "devide" | grep -v "func4" | grep -v "mean" | grep -v "variance" | grep _600_600_0.05_  | awk -F'.png' '{gsub(/,/, ";");gsub(/_/, ","); print $1}' | awk -F',' 'BEGIN {print "errorbound,data,func,width,method,ssim,totaltime,sqltime"} {print $10 "," $2 "," $5 "," $8 "," $1 "," $12 "," $14 "," $15}' > ../output/OM3.0.05.600.csv

IFX-F:
ls IFX-F_*static* | grep -v "func1" | grep -v "boxcox0" | grep -v "plus" | grep -v "minus" | grep -v "multi" | grep -v "devide" | grep -v "func4" | grep -v "mean" | grep -v "variance" | grep _600_600_0.05_  | awk -F'.png' '{gsub(/,/, ";");gsub(/_/, ","); print $1}' | awk -F',' 'BEGIN {print "errorbound,data,func,width,method,ssim,totaltime,sqltime"} {print $10 "," $2 "," $5 "," $8 "," $1 "," $12 "," $14 "," $15}' > ../output/IFX-F.0.05.600.csv

IFX-M4:
ls IFX-M4_*static* | grep -v "func1" | grep -v "boxcox0" | grep -v "plus" | grep -v "minus" | grep -v "multi" | grep -v "devide" | grep -v "func4" | grep -v "mean" | grep -v "variance" | grep _600_600_0.05_  | awk -F'.png' '{gsub(/,/, ";");gsub(/_/, ","); print $1}' | awk -F',' 'BEGIN {print "errorbound,data,func,width,method,ssim,totaltime,sqltime"} {print $10 "," $2 "," $5 "," $8 "," $1 "," $12 "," $14 "," $15}' > ../output/IFX-M4.0.05.600.csv


