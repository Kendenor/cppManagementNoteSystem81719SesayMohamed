#include "Note.h"
#include <iostream>
#include <sstream>

using namespace std;

// Default constructor
Note::Note() : id(0), title(""), content(""), createdAt(""), tags({}), isPinned(false), isArchived(false) {}

// Parameterized constructor
Note::Note(int id, string title, string content, string createdAt, vector<string> tags, bool isPinned, bool isArchived)
    : id(id), title(title), content(content), createdAt(createdAt), tags(tags), isPinned(isPinned), isArchived(isArchived) {}

// Getters
int Note::getId() const { return id; }
string Note::getTitle() const { return title; }
string Note::getContent() const { return content; }
string Note::getCreatedAt() const { return createdAt; }
vector<string> Note::getTags() const { return tags; }
bool Note::getIsPinned() const { return isPinned; }
bool Note::getIsArchived() const { return isArchived; }

// Setters
void Note::setTitle(const string& newTitle) { title = newTitle; }
void Note::setContent(const string& newContent) { content = newContent; }
void Note::setTags(const vector<string>& newTags) { tags = newTags; }
void Note::setIsPinned(bool pinned) { isPinned = pinned; }
void Note::setIsArchived(bool archived) { isArchived = archived; }

// Update helper
void Note::update(const string& newTitle, const string& newContent, const vector<string>& newTags) {
    title = newTitle;
    content = newContent;
    tags = newTags;
}

// Display method to render the note in a beautiful CLI card
void Note::display() const {
    string borderLine = "============================================================";
    
    // ANSI code constants
    string colorReset = "\033[0m";
    string colorCode = "\033[36m"; // Default Cyan for normal notes
    
    if (isArchived) {
        colorCode = "\033[90m"; // Dim/Gray for archived notes
    } else if (isPinned) {
        colorCode = "\033[1;33m"; // Gold/Yellow for pinned notes
    }
    
    // Top border
    cout << "\n" << colorCode << "+" << borderLine << "+" << colorReset << "\n";
    
    // Render Pinned and ID line
    string pinStr = isArchived ? " [ARCHIVED]" : (isPinned ? " [★ PINNED]" : "          ");
    string idLine = " Note ID: " + to_string(id) + pinStr;
    cout << colorCode << "| " << colorReset << idLine << string(60 - idLine.length() - 2, ' ') << colorCode << " |\n" << colorReset;
    
    // Render Title Line
    string titleLine = " Title:   " + title;
    if (titleLine.length() > 56) {
        titleLine = titleLine.substr(0, 53) + "...";
    }
    cout << colorCode << "| " << colorReset << titleLine << string(60 - titleLine.length() - 2, ' ') << colorCode << " |\n" << colorReset;
    
    // Render Timestamp
    string timeLine = " Created: " + createdAt;
    cout << colorCode << "| " << colorReset << timeLine << string(60 - timeLine.length() - 2, ' ') << colorCode << " |\n" << colorReset;
    
    // Render Tags
    string tagsLine = " Tags:    ";
    if (tags.empty()) {
        tagsLine += "(None)";
    } else {
        for (size_t i = 0; i < tags.size(); ++i) {
            tagsLine += tags[i];
            if (i < tags.size() - 1) tagsLine += ", ";
        }
    }
    if (tagsLine.length() > 56) {
        tagsLine = tagsLine.substr(0, 53) + "...";
    }
    cout << colorCode << "| " << colorReset << tagsLine << string(60 - tagsLine.length() - 2, ' ') << colorCode << " |\n" << colorReset;
    
    // Divider
    cout << colorCode << "|" << string(60 - 2, '-') << "|" << colorReset << "\n";
    
    // Content body wrapping
    string word = "";
    string line = "";
    vector<string> wrappedLines;
    
    for (char c : content) {
        if (c == '\n') {
            if (!line.empty() || !word.empty()) {
                if (!word.empty()) {
                    if (line.length() + word.length() + 1 > 54) {
                        wrappedLines.push_back(line);
                        line = word;
                    } else {
                        if (!line.empty()) line += " ";
                        line += word;
                    }
                    word = "";
                }
                wrappedLines.push_back(line);
                line = "";
            } else {
                wrappedLines.push_back(""); // Empty line break
            }
        } else if (c == ' ') {
            if (!word.empty()) {
                if (line.length() + word.length() + 1 > 54) {
                    wrappedLines.push_back(line);
                    line = word;
                } else {
                    if (!line.empty()) line += " ";
                    line += word;
                }
                word = "";
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        if (line.length() + word.length() + 1 > 54) {
            wrappedLines.push_back(line);
            line = word;
        } else {
            if (!line.empty()) line += " ";
            line += word;
        }
    }
    if (!line.empty() || wrappedLines.empty()) {
        wrappedLines.push_back(line);
    }
    
    // Print body lines
    for (const auto& l : wrappedLines) {
        cout << colorCode << "|  " << colorReset << l << string(60 - l.length() - 6, ' ') << colorCode << "  |\n" << colorReset;
    }
    
    // Calculate statistics
    int charCount = content.length();
    stringstream ss(content);
    string tempWord;
    int wordCount = 0;
    while (ss >> tempWord) {
        wordCount++;
    }
    
    // Divider
    cout << colorCode << "|" << string(60 - 2, '-') << "|" << colorReset << "\n";
    
    // Stats display
    string statsLine = " Words: " + to_string(wordCount) + " | Characters: " + to_string(charCount);
    cout << colorCode << "| " << colorReset << statsLine << string(60 - statsLine.length() - 2, ' ') << colorCode << " |\n" << colorReset;
    
    // Bottom border
    cout << colorCode << "+" << borderLine << "+" << colorReset << "\n\n";
}
