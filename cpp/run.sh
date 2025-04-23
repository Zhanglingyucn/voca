 g++ -std=c++14 -O2 main.cpp -o main
clear

./main static ../data/om3data/nycdata.csv_4733280 "nycdata 7,9,1,2,3,4,5,6,8,10,11"
./main static ../data/om3data/synthetic_1m.csv_1051921 "synthetic_1m 1,2,3,4,5"
./main static ../data/om3data/synthetic_2m.csv_2103841 "synthetic_2m 1,2,3,4,5"
./main static ../data/om3data/synthetic_4m.csv_3944701 "synthetic_4m 1,2,3,4,5"
./main static ../data/om3data/synthetic_8m.csv_7889401 "synthetic_8m 1,2,3,4,5"
./main static ../data/om3data/synthetic_16m.csv_15778801 "synthetic_16m 1,2,3,4,5"

