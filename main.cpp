#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "Blockchain/hash.hpp"
#include "Blockchain/Block.hpp"
#include "Blockchain/common.hpp"
#include "Blockchain/BlockChain.hpp"
#include "Network/requests.hpp"

#include "JSON/json.hh"
using json = nlohmann::json;

using namespace std;

#include "Network/client_http.hpp"
#include "Network/server_http.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

/*
Hash header: index + prevHash + merkleRoot(data) + nonce
*/

/*
 * Main function - sets up server, command line interface
 */
int main() {
    printf("\033[1;32mWelcome! To quit-> Control c\033[0m\n");
    HttpServer server;

    // Set up ports

    int port;
    printf("\033[1;34mEnter port:\033[0m ");
    scanf("%d", &port);
    server.config.port = port; // server port

    vector<int> listOfNodes; // vector of the ports of nodes in the network

    // BLOCK CHAIN INITIALIZATION AND ADDING SELF TO NETWORK

    char ch;
    printf("\033[1;34mAre you the initial Node? (y or n) \033[0m");
    scanf(" %c", &ch);
    BlockChain bc;
    if (ch == 'y') {
        // Initial Node: setup Blockchain with genesis block
        bc = BlockChain(0);
    } else if (ch == 'n') {
        // New Node - need to add self to network by providing ports
        bc = BlockChain(0);
        char otherPorts[50];
        // Example input: 8000,3000,3030
        printf("\033[1;34mEnter ports of nodes in network(with commas in between):\033[0m ");
        scanf("%s", otherPorts);
        stringstream ss(otherPorts);
        int i;
        // parse string of nodes and add them to listOfNoes
        while (ss >> i) {
            listOfNodes.push_back(i);
            if (ss.peek() == ',' || ss.peek() == ' ')
                ss.ignore();
        }
        addSelfToNetwork(&listOfNodes, server.config.port);
        json chain = getChainFromNodes(&listOfNodes);
        // skips first block - same genesis block across all nodes
        for (int a = 1; a <chain["length"].get<int>(); a++ ) {
            auto block = chain["data"][a];
            vector<string> data = block["data"].get<vector<string>>();
            bc.addBlock(block["index"], block["previousHash"], block["hash"], block["nonce"], data, block["timestamp"],block["minerID"]);
        }
    } else {
        return 0;
    }

    // SERVER INITIALIZATION

    /* POST /addnode - used to add node to network, called by new node to all the nodes in the network
     * adds node(port) to listOfNodes
    */
    server.resource["^/addnode$"]["POST"] = [&listOfNodes](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        printf("\033[1;33m--- New Node adding to network....\033[0m\n");
        try {
            json content = json::parse(request->content);
            int port = content["port"].get<int>();
            listOfNodes.push_back(port); // Adds port to listOfNodes
            printf("\033[1;33m----Adding node %d to listOfNodes\033[0m\n", port);
            response->write("Added You to our List");
        } catch(const exception &e) {
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
        }
    };

    /* GET /latestchain gets latest blockchain and sends it */
    server.resource["^/latestchain$"]["GET"] = [&bc](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        printf("\033[1;33m--- Sending BlockChain....\033[0m\n");
        response->write(bc.toJSON());
        printf("\033[1;33m---Sent current BlockChain\033[0m\n");
    };

    /* POST /newchain called by a node when a new block is added to it -
     * checks whether the length of the blockchain is bigger than our own blockchain
     * if it is bigger -> replace chain, else don't do anything
    */
    server.resource["^/newchain$"]["POST"] = [&bc](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        cout << "\n\033[1;33m--- Node in Network sent new chain\033[0m\n";
        try {
            json content = json::parse(request->content);
            if (content["length"].get<int>() > bc.getNumOfBlocks()) {
                bc.replaceChain(content);
                cout << "\033[1;33m----Replaced current chain with new one\033[0m" << endl;
                response->write("Replaced Chain\n");
            } else {
                cout << "\033[1;33m----Chain was not replaced: sent chain had same size\033[0m" <<endl;
                response->write("Same Chain Size -- invalid");
            }
        } catch(const exception &e) {
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
        }
    };

    // On error lambda function
    server.on_error = [](shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & ec) {
        if (ec.message() != "End of file") {
            cout << "\033[1;31mSERVER ERROR: \033[0m" << ec.message() << endl;
        }
    };
    printf("\033[1;34mStarting server at %d\033[0m", server.config.port);

    // start server
    thread server_thread([&server]() {
        server.start();
    });

    // COMMAND LINE INTERFACE

    // loop for 20 inputs - can change
    for ( int i = 0; i < 20; i++ ) {
        vector<string> transactions;
        int temp;
        // ask for what to do
        printf("\n\033[1;34m(1) Look at longest Blockchain\n(2) Add block\n\033[0m");
        printf("\033[1;34mEnter your choice : \033[0m");
        int valid = scanf("%d",&temp);

        if ( (valid == 1) && (temp == 1)) { // queue up block if 1
            // printf("\033[1;34mWhat Block do you want to look at? \033[0m");
            // scanf("%d", &temp);
            try {
                // bc.getBlock(temp).toString();
                bc.printBlockchain();
            } catch (const exception& e) {
                cout << e.what() << endl;
            }
        } else if (temp == 2) { // add a new block if 2

            int numTransactions;

            // Input the number of transactions
            std::cout << "\033[1;34mEnter the number of transactions: \033[0m";
            std::cin >> numTransactions;
            std::cin.ignore(); // Ignore the newline character in the input buffer

            // Input each transaction string and add it to the vector
            for (int i = 0; i < numTransactions; ++i) {
                std::string transaction;
                std::cout << "\033[1;34mEnter transaction " << (i + 1) << ": \033[0m";
                std::getline(std::cin, transaction);
                transactions.push_back(transaction);
            }

            try {
                if (bc.getNumOfBlocks() == 0) {
                    printf("\033[1;31m----------------------------------\nPlease join the network... Your blockchain doesn't have any blocks \033[0m");
                    continue;
                }
                // mine for the hash

                // validate list of transactions

                // mining
                string timestamp = getCurrentTimestampAsString();
                auto pair = findHash( bc.getLatestBlockHash(), transactions,timestamp);
                // add the block to the blockchain
                int minerID = port;
                bc.addBlock(bc.getNumOfBlocks(), bc.getLatestBlockHash(), pair.first, pair.second, transactions, timestamp,port);
                // send the blockchain to the network
                for (auto x : listOfNodes) {
                    cout << x << endl;
                }
                sendNewChain(&listOfNodes, bc.toJSON());
            } catch (const exception& e) {
                cout << e.what() << "\n" << endl;
            }
        }
    }

    printf("\n");
    return 0;
}
