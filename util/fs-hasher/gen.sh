#!/bin/bash

./fs-hasher -p $1 -c variable -C :algo=rabin:win_size=48:match_bits=13:pattern=ffff:min_csize=2048:max_csize=16384 -h md5-48bit -z none -o $2



