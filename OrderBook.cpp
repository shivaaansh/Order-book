#include "OrderBook.hpp"
#include<mutex>
#include<thread>

void OrderBook::addOrder(const Order& order) {
    // yeh wala lock apne aap call hua jaise hi function call hua
    // ab yeh addOrder koi critical section nhi raha and thread safe hogya
    std::lock_guard<std::mutex> lock(mtx);
    switch (order.type) {
        case OrderType::LIMIT: addLimitOrder(order); break;
        case OrderType::MARKET: addMarketOrder(order); break;
        case OrderType::FOK: addFOKOrder(order); break;
        case OrderType::IOC: addIOCOrder(order); break;
    }
}

void OrderBook::addLimitOrder(const Order& order) {
    bool isBuy = order.side == Side::BUY;
    matchOrder(order, isBuy ? sellOrders : buyOrders, isBuy);
    if (order.quantity > 0) {
        auto& book = isBuy ? buyOrders : sellOrders;
        book[order.price].push_back(order);
        idMap[order.id] = { order.side, order.price };
    }
}

void OrderBook::addMarketOrder(Order order) {
    bool isBuy = order.side == Side::BUY;
    matchOrder(order, isBuy ? sellOrders : buyOrders, isBuy);
}

void OrderBook::addFOKOrder(Order order) {
    bool isBuy = order.side == Side::BUY;
    auto& book = isBuy ? sellOrders : buyOrders;
    int remaining = order.quantity;

    for (auto it = book.begin(); it != book.end() && remaining > 0; ++it) {
        if ((isBuy && it->first > order.price) || (!isBuy && it->first < order.price))
            break;

        for (auto q : it->second)
            remaining -= q.quantity;
    }

    if (remaining <= 0)
        matchOrder(order, book, isBuy);
}

void OrderBook::addIOCOrder(Order order) {
    bool isBuy = order.side == Side::BUY;
    matchOrder(order, isBuy ? sellOrders : buyOrders, isBuy);
}

void OrderBook::matchOrder(Order order, std::map<double, std::deque<Order>>& book, bool isBuy) {
    auto cmp = [&](double a, double b) {
        return isBuy ? a <= b : a >= b;
    };

    for (auto it = book.begin(); it != book.end() && order.quantity > 0;) {
        if (!cmp(it->first, order.price) && order.type != OrderType::MARKET)
            break;

        auto& queue = it->second;
        for (auto qit = queue.begin(); qit != queue.end() && order.quantity > 0;) {
            int traded = std::min(order.quantity, qit->quantity);
            std::cout << "Matched Order ID " << order.id << " with Order ID " << qit->id
                      << " for quantity " << traded << " at price " << it->first << "\n";

            order.quantity -= traded;
            qit->quantity -= traded;

            if(qit->quantity == 0){
                // agr vo order hogya
                // to hatado book mn se
                idMap.erase(qit->id);
                qit = queue.erase(qit);
            } else {
                // vrna agle index of the deque mn chle jaao
                qit++;
            }
        }

        if(queue.empty()){
            // current wala price hogya
            it = book.erase(it);
        }
        else{
            // agle wale pr chlte h 
            it++;
        }
    }
}

void OrderBook::cancelOrder(int orderId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = idMap.find(orderId);
    if (it == idMap.end()) {
        std::cout << "Order ID " << orderId << " not found.\n";
        return;
    }

    // this is the reason for O(1) lookup and why we made the idMap ds
    // orderId -> {side, typeOfBook store kra h mne}
    Side side = it->second.first;
    double price = it->second.second;
    auto& book = side == Side::BUY ? buyOrders : sellOrders;

    auto qit = book.find(price);
    if (qit != book.end()) {
        auto& queue = qit->second;
        // hr deque ko check karo agr hogya to bdiya vrna next deque ko check karo
        // worst case mn O(N) lookup agr saare hi same price ke h
        for (auto it2 = queue.begin(); it2 != queue.end(); ++it2) {
            if (it2->id == orderId) {
                queue.erase(it2);
                idMap.erase(orderId);
                std::cout << "Cancelled Order ID " << orderId << "\n";
                return;
            }
        }
    }

    // this line will never be executed but it is good to writ this as a fallback
    std::cout << "Order ID " << orderId << " not found in book.\n";
}

void OrderBook::printOrderBook() const {
    // kyuki yeh sirf ek real operation h to lock ki zarurt nhi
    std::cout << "\n--- SELL ---\n";
    for (auto it = sellOrders.begin(); it != sellOrders.end(); ++it) {
        const auto& queue = it->second;
        std::cout << "Price: " << it->first << " -> ";
        for (const auto& order : queue)
            std::cout << "[" << order.id << ":" << order.quantity << "] ";
        std::cout << "\n";
    }

    std::cout << "--- BUY ---\n";
    for (auto it = buyOrders.rbegin(); it != buyOrders.rend(); ++it) {
        const auto& queue = it->second;
        std::cout << "Price: " << it->first << " -> ";
        for (const auto& order : queue)
            std::cout << "[" << order.id << ":" << order.quantity << "] ";
        std::cout << "\n";
    }
}
