#include "Note.h"
#include <iostream>

using namespace std;

// Default constructor
Note::Note() : id(0), title(""), content(""), createdAt(""), tags({}), isPinned(false) {}

// Parameterized constructor
Note::Note(int id, string title, string content, string createdAt, vector<string> tags, bool isPinned)
    : id(id), title(title), content(content), createdAt(createdAt), tags(tags), isPinned(isPinned) {}

// Getters
int Note::getId() const { return id; }
string Note::getTitle() const { return title; }
string Note::getContent() const { return content; }
string Note::getCreatedAt() const { return createdAt; }
vector<string> Note::getTags() const { return tags; }
bool Note::getIsPinned() const { return isPinned; }

// Setters
void Note::setTitle(const string& newTitle) { title = newTitle; }
void Note::setContent(const string& newContent) { content = newContent; }
void Note::setTags(const vector<string>& newTags) { tags = newTags; }
void Note::setIsPinned(bool pinned) { isPinned = pinned; }

// Update helper
void Note::update(const string& newTitle, const string& newContent, const vector<string>& newTags) {
    title = newTitle;
    content = newContent;
    tags = newTags;
}

// Display method to render the note in a beautiful CLI card
void Note::display() const {
    string borderLine = "============================================================";
    
    // Top border
    cout << "\n+" << borderLine << "+\n";
    
    // Render Pinned and ID line
    string pinStr = isPinned ? " [★ PINNED]" : "          ";
    string idLine = " Note ID: " + to_string(id) + pinStr;
    cout << "| " << idLine << string(60 - idLine.length() - 2, ' ') << " |\n";
    
    // Render Title Line
    string titleLine = " Title:   " + title;
    if (titleLine.length() > 56) {
        titleLine = titleLine.substr(0, 53) + "...";
    }
    cout << "| " << titleLine << string(60 - titleLine.length() - 2, ' ') << " |\n";
    
    // Render Timestamp
    string timeLine = " Created: " + createdAt;
    cout << "| " << timeLine << string(60 - timeLine.length() - 2, ' ') << " |\n";
    
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
    cout << "| " << tagsLine << string(60 - tagsLine.length() - 2, ' ') << " |\n";
    
    // Divider
    cout << "|" << string(60 - 2, '-') << "|\n";
    
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
        cout << "|  " << l << string(60 - l.length() - 6, ' ') << "  |\n";
    }
    
    // Bottom border
    cout << "+" << borderLine << "+\n\n";
}
