#include "client.h"
#include "server.h"
#include "crypto.h"
#include <vector>
Client::Client(std::string id, const Server& server):id(id),server(&server){

    std::string public_key{};
    std::string private_key{};
    crypto::generate_key(public_key, private_key);
    this->public_key = public_key;
    this->private_key = private_key;
}

std::string Client::get_id(){
    return id;
}

std::string Client::get_publickey()const{
    return public_key;
}

double Client::get_wallet(){
    return server->get_wallet(id);
}

std::string Client::sign(std::string txt)const{
    return crypto::signMessage(private_key,txt);
}

bool Client::transfer_money(std::string receiver, double value){
    std::string trx = this->get_id() + '-' + receiver + '-' +std::to_string(value);
    auto signature = sign(trx);
    if(this->server->get_client(receiver) == NULL)
        return false;
    if(!this->server->add_pending_trx(trx, signature))
        return false;
    return true;
}
size_t Client::generate_nonce(){
    return random()%10;
}
