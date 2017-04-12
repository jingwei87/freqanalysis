#!/bin/bash


echo 3 > /proc/sys/vm/drop_caches
rm -rf ./uniq-db/*
./db-stat-local 5 30 200000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> top
rm -rf ./uniq-db/*
./db-stat-local 10 30 200000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> top
rm -rf ./uniq-db/*
./db-stat-local 15 30 200000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> top
rm -rf ./uniq-db/*
./db-stat-local 20 30 200000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> top


echo 3 > /proc/sys/vm/drop_caches
rm -rf ./uniq-db/*
./db-stat-local 5 10 200000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> span
rm -rf ./uniq-db/*
./db-stat-local 5 20 200000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> span
rm -rf ./uniq-db/*
./db-stat-local 5 30 200000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> span
rm -rf ./uniq-db/*
./db-stat-local 5 40 200000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> span

echo 3 > /proc/sys/vm/drop_caches
rm -rf ./uniq-db/*
./db-stat-local 5 30 100000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> queue
rm -rf ./uniq-db/*
./db-stat-local 5 30 200000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> queue
rm -rf ./uniq-db/*
./db-stat-local 5 30 300000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> queue
rm -rf ./uniq-db/*
./db-stat-local 5 30 400000 0 ./3/ ./3-left/ ./3-right/ ./5/ ./5-left/ ./5-right/ >> queue
