#include "Trader.hpp"

#include <iostream>
#include <string> 
#include <future>

unsigned long long Trader::next_id = 0; 
unsigned long long Trade::next_id = 0; 

Trade::Trade(unsigned long long amount, std::string ticker) : id(next_id), timestamp(std::chrono::system_clock::now()), amount(amount), ticker(ticker) {
    next_id++; 
}

void Trade::getTradeInfo() const{

    std::time_t today_time = std::chrono::system_clock::to_time_t(timestamp);

    std::cout << "Trade with ID " << std::to_string(id)
    << " at " << std::ctime(&today_time) << ".\n"; 

}
Trader::Trader(double cash, Exchange* exchange, std::map<std::string, int> stocks) : id(next_id), cash(cash), portfolio(), trades(), exchange(exchange), waiting_sells() {
    init = cash; 
    for(auto it : stocks){
        init+=it.second * exchange->tickerVal(it.first); 
        portfolio.insert(std::pair<std::string, int>(it.first, it.second)); 
    }
    next_id++; 
} 

unsigned long long Trader::getId() const{
    return id; 
}

void Trader::getInfo() const{

    std::cout  << "Trader " << std::to_string(id) << " has $" << std::to_string(cash) << ", ";
    for(auto it = portfolio.begin(); it!=portfolio.end(); ++it){
        std::cout << std::to_string(it->second) << " of " << it->first;
        if(it == --portfolio.end()){
            std::cout << ".";
        }
        else{
            std::cout << ", "; 
        }
        
    }
    std::cout << " Their net profit is $" << this->getEquity() - init << ".\n"; 
}

void Trader::buy(std::string ticker, int amount){
    
    if(cash<exchange->tickerVal(ticker)*amount){ 
        throw std::logic_error( "Trader "+std::to_string(id)+"does not have enough money to buy " +ticker+"!");
    }

    if(!exchange->updateBuckets(false, id)){ return; }

    //asynchronosuly requests the buy 
    std::future<bool> fut = std::async(std::launch::async, Exchange::makeRequest, exchange, new Request(Buy,ticker,amount, this));

    if(!fut.get()){ //only returns false if request is invalid 
        throw std::invalid_argument("Make sure that the ticker exists."); 
    }

    cash-=exchange->tickerVal(ticker)*amount; //take away the cash to buy 

    if(portfolio.find(ticker)==portfolio.end()){ // first time buying this ticker
        portfolio.insert(std::pair<std::string, int>(ticker, amount));
    }
    else{ // this ticker is already in the portfolio, so just add to it 
        portfolio[ticker]+=amount; 
    }

}

void Trader::sell(std::string ticker, int amount){
    
    if(portfolio[ticker]<amount){ // user is trying to sell more than they have
        throw std::logic_error("Cannot sell more shares than you own!"); 
    }

    if(!exchange->updateBuckets(false, id)){ return; }

    auto req = new Request(Sell,ticker,amount, this);
    std::future<bool> fut = std::async(std::launch::async, Exchange::makeRequest, exchange, req);
    waiting_sells.insert(std::pair<unsigned long long, double>(req->getId(), req->getAmount()*exchange->tickerVal(req->getTicker()))); 
    if(!fut.get()){
        throw std::invalid_argument("Make sure that the ticker exists."); 
    }
    
}

void Trader::addTrade(Trade* trade, Side side){

    trades.push_back(std::pair<Trade*,Side>(trade,side)); 

}

void Trader::confirmSell(Request request, int amount){
    auto it = waiting_sells.find(request.getId()); 


    if(it == waiting_sells.end()){ throw;}
    portfolio[request.getTicker()] -= amount; 
    cash+=exchange->tickerVal(request.getTicker())*amount;
    if (portfolio[request.getTicker()] == 0){
        portfolio.erase(request.getTicker()); 
    } 

    if(amount==request.getAmount()){
        waiting_sells.erase(it); 
    }

    else{
        request.updateAmount(request.getAmount()-amount); 
    }
    
    return;

}

double Trader::getEquity() const {
    int eq = cash; 

    for(auto it : portfolio){
        eq+=it.second*exchange->tickerVal(it.first); 
    }
    return eq;
}