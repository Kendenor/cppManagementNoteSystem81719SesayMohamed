#ifndef NOTE_H
#define NOTE_H

#include <string>
#include <vector>

using namespace std;

class Note {
public:
    // Default constructor
    Note();
    
    // Parameterized constructor
    Note(int id, string title, string content, string createdAt, vector<string> tags, bool isPinned = false, bool isArchived = false);
    
    // Getters
    int getId() const;
    string getTitle() const;
    string getContent() const;
    string getCreatedAt() const;
    vector<string> getTags() const;
    bool getIsPinned() const;
    bool getIsArchived() const;
    
    // Setters & Modifiers
    void setTitle(const string& newTitle);
    void setContent(const string& newContent);
    void setTags(const vector<string>& newTags);
    void setIsPinned(bool pinned);
    void setIsArchived(bool archived);
    
    // Helper to edit note contents directly
    void update(const string& newTitle, const string& newContent, const vector<string>& newTags);
    
    // Renders the note in a beautiful box in the console
    void display() const;

private:
    int id;
    string title;
    string content;
    string createdAt;
    vector<string> tags;
    bool isPinned;
    bool isArchived;
};

#endif // NOTE_H
