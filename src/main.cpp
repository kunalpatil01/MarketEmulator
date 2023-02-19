#include <iostream>
#include <future>
#include <thread>
#include "Exchange.hpp"

int main(){
    
    Exchange NYSE{{{"APPL", 200}, {"AMAZ",300}}};
    std::future<void> fut1 = std::async(std::launch::async, Exchange::start_rate_limiter, &NYSE);
    std::future<void> fut3 =std::async(std::launch::async, Exchange::fluctuate_prices, &NYSE);
    Trader trader0(10000, &NYSE, {{"AMAZ", 200}}); 
    Trader trader1(10000, &NYSE, {{"APPL", 200}}); 

    
    std::future<void> fut2 = std::async(std::launch::async, Trader::buy, &trader0, "APPL", 2);  
    
    for(int i = 0; i<10; ++i){
        trader1.sell("APPL", 1); 
    }
    std::future<void> fut4 = std::async(std::launch::async, Trader::buy, &trader1, "AMAZ", 5);
    trader0.sell("AMAZ", 4); 
   
    std::this_thread::sleep_for(std::chrono::seconds(5)); 
   
     trader0.getInfo();
     trader1.getInfo(); 

    return 0; 
}