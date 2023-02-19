#include "Exchange.hpp"

#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <mutex>
 
std::mutex mtx; // mutex for critical section of the mathing algorithm
std::mutex mtx2; // mutex for updating rate_limiter


    Exchange::Exchange() : tickers(), sell_reqs(){}; 
    Exchange::Exchange(std::map<std::string,double> tickers) : tickers(tickers), sell_reqs(){}; 

    void Exchange::start_rate_limiter(){
        while(true){
            updateBuckets(true, 0); // update all the buckets
            std::this_thread::sleep_for(std::chrono::seconds(1)); // repeat every second  
        }
    }

    void Exchange::fluctuate_prices(){
        while(true){
            for(auto &it : tickers){
                int num = rand()%2;  // 50% chance of incrementing or decrementing
                int delta = rand()%20+1;   // change in price will be between 1 and 20
                if(num%2==0){
                    it.second += delta;
                }
                else{
                    it.second-=delta; 
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(5)); //repeat every 5 seconds 
        }
    }

    bool Exchange::updateBuckets(bool updateAll, long client){
        mtx2.lock(); //lock so we don't have a data race 
        if(updateAll){ 
            for(auto it: rate_limiter){
                if(it.second<60){it.second +=1; } // only increment if we are not at capacity 
            }
            
        }
        else{
            if(rate_limiter.find(client)==rate_limiter.end()){ // this is the first time seeing this client, set bucket to 60 tokens 
                rate_limiter[client] = 60; 
            }
            else if(rate_limiter[client]==0){  // if the client is at 0 tokens left deny them the ability to make a request 
                std::cout << "DENIED!"; 
                mtx.unlock(); 
                return false; 
            }
            rate_limiter[client]-=1; 
        }
        mtx2.unlock(); 
        return true;
    }

    void Exchange::addTicker(std::string ticker, double val){
        tickers.insert(std::pair<std::string, double>(ticker,val)); 
    }

    bool Exchange::makeRequest(Request* request){
      
        if(tickers.find(request->getTicker())==tickers.end()){ //requesting a ticker that is not in the exchange
            return false;
        }
       
        bool done = run_matching_engine(request); //check if we handled the request first try
   
        while(!done){ //retry the request every second until it is satisfies (someone sells the stock this trader wants to buy)
        
            std::this_thread::sleep_for(std::chrono::seconds(1));
            done = run_matching_engine(request); 

        }
    
        return done; 

    }

    bool Exchange::run_matching_engine(Request* request){

      mtx.lock(); //lock so we do not try satisfying multiple requests at the same time 
        if(request->getSide() == Buy){ // this is a buy request 
          
            auto & sell_q = sell_reqs[request->getTicker()]; //get sell requests for this ticker 
            
            if(sell_q.size() == 0){ // no sell requests available to satisfy the request
                 mtx.unlock(); 
                return false;
            }
        
            unsigned long long amt = request->getAmount(); 

            while(!sell_q.empty() && amt!=0){
                  
                    auto it = sell_q.begin(); 
                    if(it->getAmount()<=amt){

                    amt-=it->getAmount();
                    it->getTrader()->confirmSell(*it, it->getAmount()); 
                     
                    it->updateAmount(0); 
                    sell_q.pop_front(); 
                    
                    request->updateAmount(amt); 
                    }
                    else{
                        it->updateAmount(it->getAmount()-amt); 
                        it->getTrader()->confirmSell(*it, amt);
                        mtx.unlock(); 
                        return true; 
                    }
                }
              mtx.unlock(); 
             return amt == 0; 
            
        }
        else{  
                auto q = sell_reqs.find(request->getTicker()); 
                if(q==sell_reqs.end()){
                    sell_reqs.insert(std::pair<std::string, std::deque<Request>>(request->getTicker(), std::deque<Request>{*request})); 
                }
                else{
                    q->second.push_back(*request); 
                }
            }
        mtx.unlock(); 
        return true; 
    }

double Exchange::tickerVal(std::string ticker) const{
    return tickers.at(ticker); 
}

