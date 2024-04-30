# Basic Blockchain Implementation in C++

This is a simple blockchain implementation written in C++. It follows some principles from Bitcoin's design, including the use of SHA-256 for hashing, merkle trees, and a basic "mining" process.

## Requirements

- C++14
- OpenSSL library
- Simple-Web-Server
- [JSON library](https://github.com/nlohmann/json)

## Command Line Interface (CLI)

The project includes a CLI that allows you to interact with the blockchain. You can view the longest chain of blockchain and add new blocks. By default, the CLI allows adding blocks up to 20 times before automatically quitting.

## Peer-to-Peer Network

Instead of a full peer-to-peer network with WebSockets, this implementation uses HTTP requests to connect nodes to the network. Key aspects of the network include:

- Tracking nodes in the network
- Retrieving the latest chains from nodes to validate and stay up-to-date
- Broadcasting your chain to the network when adding a new block

## Conflict Resolution

In case of conflicting chains (e.g., different nodes have different versions of the blockchain), the longest chain is chosen. This implementation replaces the entire chain except for the genesis block when conflicts occur.

## BlockChain Object

The `BlockChain` class manages the blockchain. Private variables include a vector of smart pointers to `Block` objects. It ensures the integrity of blocks by validating hashes and indices during block addition.

## Mining (Proof of Work)

A simplified "mining" process is implemented using the `findHash` function to find a valid hash and nonce pair. The block's hash must start with "00" to be considered valid.

## Block Object

The `Block` class represents individual blocks in the blockchain. Its properties are private for immutability, and methods exist to retrieve block information but not update it.

## Steps to Run the Code

1. Navigate to the directory where the `main.cpp` file resides.
2. In the terminal, enter the following commands to compile the code:
   ```bash
   chmod +x compile.sh
   ./compile.sh
3. Once the compilation is successful, run an instance of the node by entering:
   ```bash
   ./a.out

## Contributors

* [Aditya Mandal](https://github.com/addy-1406)
* [Tanish](https://github.com/v-tanish012)
* [Mihit](https://github.com/mihits)
* [Soumyadeep](https://github.com/soumyadeep-p)
