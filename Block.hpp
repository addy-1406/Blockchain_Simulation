//author: tko
#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "json.hh"
using json = nlohmann::json;

class Block {
    public:
        Block(int index, string prevHas, string hash, string nonce, vector<string> data, string timestamp,int minerID);
        string getPreviousHash(void);
        string getHash(void);
        int getIndex(void);
        vector<string> getData(void);
        void toString(void);
        json toJSON(void);
    private:
        int index;
        int minerID;
        string previousHash;
        string blockHash;
        string nonce;
        vector<string> data;
        string timestamp;
        // string getMerkleRoot(const vector<string> &merkle);
};
// Constructor 
Block::Block(int index, string prevHash, string hash, string nonce, vector<string> data, string timestamp,int minerID) {
    printf("\nInitializing Block: %d ---- Hash: %s \n", index,hash.c_str());
    this -> previousHash = prevHash;
    this -> data = data;
    this -> index = index;
    this -> nonce = nonce;
    this -> blockHash = hash;
    this ->timestamp = timestamp;
    this ->minerID = minerID;
}

int Block::getIndex(void) {
    return this -> index;
}

string Block::getPreviousHash(void) {
    return this -> previousHash;
}

string Block::getHash(void) {
    return this -> blockHash;
}

vector<string> Block::getData(void){
    return this -> data;
}

// Prints Block data 
void Block::toString(void) {
    std::string dataString;
    for (int i=0; i < data.size(); i++)
        dataString += data[i] + ", ";
    
    // ANSI escape codes for cyan color
    const std::string cyanColor = "\033[1;36m";
    const std::string resetColor = "\033[0m";

    printf("\n-------------------------------\n");
    printf("%sBlock %d\nPrevious Hash: %s\nTransactions: %s\nTimestamp: %s\nMinerID: %d\n%s",
        cyanColor.c_str(), index, previousHash.c_str(), dataString.c_str(), timestamp.c_str(),minerID, resetColor.c_str());
    printf("\n-------------------------------\n");
}

json Block::toJSON(void) {
    json j;
    j["index"] = this->index;
    j["hash"] = this->blockHash;
    j["previousHash"] = this->previousHash;
    j["nonce"] = this->nonce;
    j["data"] = this->data;
    j["timestamp"] = this->timestamp;
    j["minerID"] = this->minerID;
    return j;
}

#endif