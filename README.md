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
- [Google Leveldb](https://github.com/google/leveldb): a version of 1.20 is provided in `util/` 
- [fs-hasher](http://tracer.filesystems.org/fs-hasher-0.9.4.tar.gz): a version
	of 0.9.4 is provided in `util/` 

## Attack Usages

### Basic Attack

The basic attack builds on classical frequency analysis. Follow the steps to
simulate the basic attack.

**Step 1, configure pre-requsite components:** copy `util/fs-hasher/` and
`util/leveldb/` into working directory and compile them respectively.  

**Step 2, configure basic attack:** modify variables in `Basic_script.sh` to
adapt desired setting:

- `fsl` specifies the path of the fsl trace.
- `users` specifies which users are collectively considered in backups.
- `date_of_aux` specifies the backups of which dates are separately considered as auxiliary information.
- `date_of_latest` specifies the backup of which date is the target for
  inference.

To compile the program of the basic attack, run the command
```
make
```
**Step 3, run basic attack:** type the command
```
./Basic_script.sh
```
**Result:** The result of basic attack is shown as follows

```
Auxilliary information: YYYY-MM-DD; 	Target backup: YYYY-MM-DD
Total number of unique chunks: AAAA
Correct inference: BBBB
```

Here, `AAAA` is the number of unique chunks in the target latest backup, while
`BBBB` is the number of (unique) chunks that can be successfully inferred by the
basic attack. The inference ratio is evaluated by `BBBB/AAAA`.  Note that the
inference rate is slightly affected by the sorting algorithm in frequency analysis. The
reason is different sorting algorithms may break tied chunks (that
have the same frequency rank) in different ways and lead to (slightly) different
results. 

 


### Locality-based Attack

## Defense Usages

### MinHash Encryption
