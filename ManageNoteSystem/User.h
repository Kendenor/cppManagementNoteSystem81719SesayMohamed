#ifndef USER_H
#define USER_H

#include <string>

using namespace std;

class User {
public:
    // Default constructor (needed for serialization and collection handling)
    User();
    
    // Parameterized constructor to initialize a user
    User(string name, string id);
    
    // Getters
    string getUsername() const;
    string getUserId() const;

private:
    string username;
    string userId;
};

#endif // USER_H
