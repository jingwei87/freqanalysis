#!/bin/bash

echo 3 > /proc/sys/vm/drop_caches
./mh-stat 0.001 ./3/ ./3-left/ ./3-right/ ./2048/k1-main/ ./2048/k1-left/ ./2048/k1-right/ ./2048/k1-rel/ >> def-leak-2048
rm -rf ./uniq-db/*
./mh-stat 0.002 ./3/ ./3-left/ ./3-right/ ./2048/k1-main/ ./2048/k1-left/ ./2048/k1-right/ ./2048/k1-rel/ >> def-leak-2048

echo 3 > /proc/sys/vm/drop_caches
rm -rf ./uniq-db/*
./mh-stat 0.001 ./3/ ./3-left/ ./3-right/ ./2048/k2-main/ ./2048/k2-left/ ./2048/k2-right/ ./2048/k2-rel/ >> def-leak-2048
rm -rf ./uniq-db/*
./mh-stat 0.002 ./3/ ./3-left/ ./3-right/ ./2048/k2-main/ ./2048/k2-left/ ./2048/k2-right/ ./2048/k2-rel/ >> def-leak-2048

echo 3 > /proc/sys/vm/drop_caches
rm -rf ./uniq-db/*
./mh-stat 0.001 ./3/ ./3-left/ ./3-right/ ./2048/k3-main/ ./2048/k3-left/ ./2048/k3-right/ ./2048/k3-rel/ >> def-leak-2048
rm -rf ./uniq-db/*
./mh-stat 0.002 ./3/ ./3-left/ ./3-right/ ./2048/k3-main/ ./2048/k3-left/ ./2048/k3-right/ ./2048/k3-rel/ >> def-leak-2048
