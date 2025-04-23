make clean && cmake -DCMAKE_BUILD_TYPE=Release .. && make

# # # 16 thread
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1" 16
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6,7" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6,7,8" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6,7,8,9" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6,7,8,9,10" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6,7,8,9,10,11" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6,7,8,9,10,11,12" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6,7,8,9,10,11,12,13" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6,7,8,9,10,11,12,13,14" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic128m16v 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16" 16 



#./om3-compress static ../data/om3data/nycdata.csv_4733280 "soccerdata 5,3,1,2,4,6" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "sensordata 5,4,3,2,1,7,6" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "stockdata 9,5,1,2,3,4,6,7,8,10" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "traffic 1,2,3,4,5,6,7,8,9,10" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "nycdata 7,9,1,2,3,4,5,6,8,10,11" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_1m 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_2m 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_4m 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_8m 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_16m 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_32m 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_64m 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_128m 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_256m 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic512m5v 1,2,3,4,5" 16 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic1b5v 1,2,3,4,5" 16 


# interactions

./om3-compress interactions ../src/plans.csv "interactions" 16
#./om3-compress interactions /data/zly/didi01/zhanglingyu/interaction/data/plans.txt "interactions" 16

# # 1 thread
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "soccerdata 5,3,1,2,4,6" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "sensordata 5,4,3,2,1,7,6" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "stockdata 9,5,1,2,3,4,6,7,8,10" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "traffic 1,2,3,4,5,6,7,8,9,10" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "nycdata 7,9,1,2,3,4,5,6,8,10,11" 1

# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_1m 1,2,3,4,5" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_2m 1,2,3,4,5" 1 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_4m 1,2,3,4,5" 1 
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_8m 1,2,3,4,5" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_16m 1,2,3,4,5" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_32m 1,2,3,4,5" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_64m 1,2,3,4,5" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_128m 1,2,3,4,5" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic_256m 1,2,3,4,5" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic512m5v 1,2,3,4,5" 1
# ./om3-compress static ../data/om3data/nycdata.csv_4733280 "synthetic1b5v 1,2,3,4,5" 1
