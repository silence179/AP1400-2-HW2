#include "server.h"
#include "client.h"
#include <vector>
#include <memory>
#include <random>
#include "crypto.h"

std::vector<std::string> pending_trxs;


void show_wallets(const Server& server) {
    std::cout << std::string(20, '*') << std::endl;
    for (const auto& client : server.get_clients())
        std::cout << client.first->get_id() << " : " << client.second << std::endl;
    std::cout << std::string(20, '*') << std::endl;
}

std::shared_ptr<Client> Server::add_client(std::string id){
    bool exist = false;
    for (const auto& [client_ptr, balance] : clients) {
        if (client_ptr->get_id() == id) {
            exist = true;
        }
    }
    auto new_client = std::make_shared<Client>(id,*this);
    if(exist){
        for(int i=0;i<4;i++){
            int num = random()%10;
            new_client = std::make_shared<Client>(id + std::to_string(num), *this);
        }
    }
    clients[new_client] = 5.0;
    return new_client;
}

std::shared_ptr<Client> Server::get_client(std::string id)const{
    for (const auto& [client_ptr, balance] : clients) {
        if (client_ptr->get_id() == id) {
            return client_ptr;
        }
    }
    return NULL;
}
double Server::get_wallet(std::string id) const{
    for (const auto& [client_ptr, balance] : clients) {
        if (client_ptr->get_id() == id) {
            return balance; // 找到了，返回余额
        }
    }
    return -1;
}
bool Server::parse_trx(std::string trx, std::string& sender, std::string &receiver, double &value){
    size_t start = 0;
    size_t end = trx.find('-');
    std::vector<std::string>parts{};
    
    while(end != std::string_view::npos){
        parts.push_back(trx.substr(start,end-start));
        start = end + 1;
        end = trx.find('-',start);
    }
    parts.push_back(trx.substr(start));

    if(parts.size()!=3){
        throw std::runtime_error("parse fault");
    }
    sender = parts[0];receiver = parts[1];value = std::stod(parts[2]);
    return true;
}
bool Server::add_pending_trx(std::string trx, std::string signature)const{
    std::string sender{},receiver{};
    double value;

    if(!parse_trx(trx,sender,receiver,value))
        return false;

    auto send_client = get_client(sender);
    auto receiver_client = get_client(receiver);
    
    bool is_authentic = crypto::verifySignature(send_client->get_publickey(),trx,signature);
    is_authentic = is_authentic & (send_client->get_wallet() >= value);

    if(is_authentic){
        pending_trxs.push_back(trx);
        return true;
    }

    return false;
}

size_t Server::mine(){
    std::string final_string{};
    for(auto & trx :pending_trxs){
        final_string += trx;
    }
    while(true)
    { 
        for (const auto& [client_ptr, balance] : clients) {
            std::string tmp_string = final_string;
            size_t nonce = client_ptr->generate_nonce();
            tmp_string += std::to_string(nonce);
            if(crypto::sha256(final_string).substr(0,10).find("000") != std::string::npos){
                for(auto & trx :pending_trxs){
                    std::string sender{},receiver{};
                    double value;

                    if(!parse_trx(trx,sender,receiver,value))
                        return false;

                    auto send_client = get_client(sender);
                    auto receiver_client = get_client(receiver);
                    
                    bool is_acceptable = send_client->get_wallet() >= value;

                    if(is_acceptable){
                        this->clients[send_client] -= value;
                        this->clients[receiver_client] += value;
                    }
                }
                pending_trxs.clear();
                this->clients[client_ptr] += 6.25;
                std::cout<<(client_ptr->get_id());
                return nonce;
            }
        } 
    }
}

std::map<std::shared_ptr<Client>, double> Server::get_clients()const{
    return this->clients;
}
