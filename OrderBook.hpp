#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include "Order.hpp"
#include <map>
#include <deque>
#include <iostream> 
#include <algorithm>
#include <mutex>

class OrderBook {
public:
    void addOrder(const Order& order);
    void cancelOrder(int orderId);
    void printOrderBook() const;

private:
    std::map<double, std::deque<Order>> buyOrders;  // price descending
    std::map<double, std::deque<Order>> sellOrders; // price ascending
    std::map<int, std::pair<Side, double>> idMap;   // For cancelling orders

    mutable std::mutex mtx;

    void matchOrder(Order order, std::map<double, std::deque<Order>>& oppositeOrders, bool isBuy);
    void addLimitOrder(const Order& order);
    void addMarketOrder(Order order);
    void addFOKOrder(Order order);
    void addIOCOrder(Order order);
};

#endif
