#include "User.h"

// Default constructor
User::User() : username(""), userId("") {}

// Parameterized constructor
User::User(string name, string id) : username(name), userId(id) {}

// Getters
string User::getUsername() const {
    return username;
}

string User::getUserId() const {
    return userId;
}
