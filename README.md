# Information Leakage in Encrypted Deduplication via Frequency Analysis

## Introduction

Encrypted deduplication seamlessly combines encryption and deduplication to simultaneously achieve both data security and storage efficiency. State-of-the-art encrypted deduplication systems mostly adopt a deterministic encryption approach that encrypts each plaintext chunk with a key derived from the content of the chunk itself, so that identical plaintext chunks are always encrypted into identical ciphertext chunks for deduplication. However, such deterministic encryption inherently reveals the underlying frequency distribution of the original plaintext chunks. This allows an adversary to launch frequency analysis against the resulting ciphertext chunks, and ultimately infer the content of the original plaintext chunks.

We study how frequency analysis practically affects information leakage in encrypted deduplication storage, from both attack and defense perspectives. We first propose a new inference attack that exploits chunk locality to increase the coverage of inferred chunks. We conduct trace-driven evaluation on a real-world dataset, and show that the new inference attack can infer a significant fraction of plaintext chunks under backup workloads. To protect against frequency analysis, we borrow the idea of existing performance-driven deduplication approaches and consider an encryption scheme called MinHash encryption, which disturbs the frequency rank of ciphertext chunks by encrypting some identical plaintext chunks into multiple distinct ciphertext chunks. Our trace-driven evaluation shows that MinHash encryption effectively mitigates the inference attack, while maintaining high storage efficiency.

### Publication

- Jingwei Li, Chuan Qin, Patrick P. C. Lee, Xiaosong Zhang. Information Leakage in Encrypted Deduplication via Frequency Analysis. In Proc. of IEEE/IFIP DSN, 2017.

## Preparation 

The attack is running under Linux (e.g., Ubuntu 14.04) with a C++ compiler (e.g., g++). To run the attack program, you need to install/compile the following dependencies. 

- Libssl API: run the command `sudo apt-get install libssl-dev`.
- Snappy compression library: run the command `sudo apt-get install libsnappy`.
- [Google Leveldb](https://github.com/google/leveldb): download and compile Leveldb, and put compiled Leveldb folder into working directory. 
- [fs-hasher](http://tracer.filesystems.org/fs-hasher-0.9.4.tar.gz): download and compile fs-hasher, and put compiled fs-hasher folder into working directory. 

## Attack Usages

### Basic Attack

The basic attack builds on classical frequency analysis.

### Locality-based Attack

## Defense Usages

### MinHash Encryption
