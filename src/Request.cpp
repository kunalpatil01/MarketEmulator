#include "Request.hpp"

    unsigned long long Request::next_id =0;

Request::Request(Side side, std::string ticker, unsigned amount, Trader* trader) : id(next_id), side(side), ticker(ticker), timestamp(std::chrono::system_clock::now()), amount(amount), trader(trader) {
    next_id++; 
} ;

const std::string Request::getTicker() const{
    return ticker;
}


const unsigned Request::getAmount() const{
    return amount; 
}

const std::chrono::time_point<std::chrono::system_clock> Request::getTime() const {
    return timestamp; 
}

const Side Request::getSide() const{
    return side; 
}

const unsigned long long Request::getId() const{
    return id; 
}

Trader* Request::getTrader() const{
    return trader;
}

void Request::updateAmount(unsigned long long amt){
    amount = amt; 
}