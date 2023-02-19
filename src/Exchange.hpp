#ifndef __EXCHANGE__
#define __EXCHANGE__

#include <map>
#include <set>
#include <string>
#include <queue> 
#include <list> 
#include <iostream>
enum Side{Buy, Sell};
#include "Request.hpp"
#include "Trader.hpp"

class Trade;

class Request;
   /*
    @class Exchange manages logic for an exchange
    */
    class Exchange{

    private:

        /*
        Bucket-token data structure to implement rate limit algorithm
        */
        std::map<long, short> rate_limiter; 
        std::map<std::string, double> tickers; 
        std::map<std::string, std::deque<Request>> sell_reqs;
    
    public:
        
        Exchange(); 
        Exchange(std::map<std::string,double> tickers); 

        void addTicker(std::string ticker, double val);

        bool makeRequest(Request* request); 

        bool run_matching_engine(Request* request);  

        double tickerVal(std::string ticker) const; 
      
        bool updateBuckets(bool updateAll, long client); 

        // asynchronous functions that always run in the background 
        void start_rate_limiter(); 
        void fluctuate_prices();



    };






#endif