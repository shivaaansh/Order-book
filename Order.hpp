// Yeh h bhai Header Guards for preventing ki do C++ files agr call krri h to
// simply sirf ek defintion pass hogi
#ifndef ORDER_HPP
#define ORDER_HPP

#include <string>

// LIMIT: Executes at a specific price or better -> Buy at <= x(x is limit) Sell at >= x
// MARKET: Executes Immediately at the best available price -> buy ya sell krdo based on best available price in the orderbook
// Ex: TCS ke 200 share uthalo Market Price pr
// FOK: ya to pura order karo ya fir karo hi na 
// Buy 500 shares of Wipro at ₹450 using FOK.
// If only 300 shares are available at or below ₹450, the order is killed, and no shares are bought
// IOC: Execute what you can and cancel the rest
// ple:

// Sell 800 shares of HDFC Bank at ₹1600 using IOC.
// If buyers are available for only 600 shares at that price, it will sell 600 and cancel the remaining 200.
// Enum isliye bnaya kyuki ab mn OrderType bula skta hu OrderType::LIMIT type se krke, Name clashes se bchega
enum class OrderType { LIMIT, MARKET, FOK, IOC };
enum class Side { BUY, SELL };

// Bta rha h ki ek order kaisa dikhega
struct Order {
    int id;
    Side side;
    OrderType type;
    double price;
    int quantity;
    long long timestamp;

    Order(int id_, Side side_, OrderType type_, double price_, int quantity_, long long ts)
        : id(id_), side(side_), type(type_), price(price_), quantity(quantity_), timestamp(ts) {}
};

#endif
