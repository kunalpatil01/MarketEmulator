#ifndef _REQUEST_
#define _REQUEST_

#include <chrono>
#include <ctime>
#include <string>
class Exchange;
class Trader;
#include "Exchange.hpp"
#include "Trader.hpp"
 

class Request{

    private:
        static unsigned long long next_id;
        unsigned long long id; 
        Trader* trader;
        Side side; 
        std::chrono::time_point<std::chrono::system_clock> timestamp; 
        std::string ticker; 
        unsigned amount; 
    
    public:

        Request(Side side, std::string ticker, unsigned amount, Trader* trader); 

        const std::string getTicker() const; 

        const unsigned getAmount() const; 

        const std::chrono::time_point<std::chrono::system_clock> getTime() const; 

        const Side getSide() const;

        const unsigned long long getId() const;

        Trader* getTrader() const;

        void updateAmount(unsigned long long amt); 

}; 



#endif