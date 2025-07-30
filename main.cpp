#include "OrderBook.hpp"

int main() {
    OrderBook book;

    book.addOrder(Order(1, Side::BUY, OrderType::LIMIT, 100.0, 10, 1));
    book.addOrder(Order(2, Side::SELL, OrderType::LIMIT, 99.0, 5, 2));
    book.addOrder(Order(3, Side::SELL, OrderType::LIMIT, 100.0, 5, 3));
    book.addOrder(Order(4, Side::BUY, OrderType::MARKET, 0.0, 10, 4));
    book.addOrder(Order(5, Side::SELL, OrderType::FOK, 98.0, 15, 5));
    book.addOrder(Order(6, Side::SELL, OrderType::IOC, 101.0, 5, 6));

    book.printOrderBook();

    book.cancelOrder(1);
    book.printOrderBook();
}
