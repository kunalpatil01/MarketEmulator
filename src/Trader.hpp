#ifndef _TRADER_
#define _TRADER_


#include "Exchange.hpp"
#include "Request.hpp"

#include <map> 
#include <list>
#include <string>

#include <chrono>
#include <ctime>
class Request;
class Trade{

    private:

        static unsigned long long next_id; 
        const unsigned long long id; 
        std::string ticker;
        unsigned long long amount; 
        std::chrono::time_point<std::chrono::system_clock> timestamp; 

    public:
        
        Trade(unsigned long long amount, std::string ticker);

        void getTradeInfo() const; 



};

class Trader{
    private:

        static unsigned long long next_id; 
        unsigned long long id; 
        std::map<std::string, int> portfolio; 
        double cash; 
        double init; 
        std::list <std::pair<Trade*, Side>> trades;
        Exchange * exchange; 
        std::map<unsigned long long, double> waiting_sells; 
    
    public:

        Trader(double cash, Exchange* exchange, std::map<std::string, int> stock); 

        unsigned long long getId() const; 

        void buy(std::string ticker, int amount); 

        void sell(std::string ticker, int amount); 

        void addTrade(Trade* trade, Side side); 

        void confirmSell(Request request, int amount); 

        void getInfo() const; 
        
        double getEquity() const; 


        


}; 


#endif