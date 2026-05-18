# C++ Codebase Structural Breakdown & Explanation

This document provides a line-by-line and section-by-section breakdown of every file inside your **ManageNoteSystem** project. Read this to understand exactly what each block of code does, why it was written that way, and how it fits into the overall architecture.

---

## 📌 Table of Contents
1. [User Module (User.h / User.cpp)](#1-user-module)
2. [Note Module (Note.h / Note.cpp)](#2-note-module)
3. [Notebook Module (Notebook.h / Notebook.cpp)](#3-notebook-module)
4. [NoteService Module (NoteService.h / NoteService.cpp)](#4-noteservice-module)
5. [Main Entry & UI Loop (main.cpp)](#5-main-entry--ui-loop)

---

## 1. User Module

The `User` class represents the author of the notes. It encapsulates basic identity credentials.

### 📄 [User.h](file:///c:/Users/Pinto/Downloads/CompSciYr3%20(1)/ManageNoteSystem/User.h) (Header Declaration)
* **`#ifndef USER_H` / `#define USER_H` / `#endif`**: These are called **header guards**. They prevent the compiler from including this file more than once, avoiding duplicate symbol errors during compilation.
* **`#include <string>`**: Includes C++ standard library string objects.
* **`using namespace std;`**: Enables us to write `string` instead of the fully qualified name `std::string`.
* **`class User { ... };`**: Defines the user blueprint.
* **`public:` / `private:`**: Access specifiers.
  - `private` members (`username`, `userId`) can *only* be accessed directly inside this class. This enforces **Encapsulation**.
  - `public` members (constructors and getter methods) are accessible by external code.
* **`User();`**: The default constructor. It initializes an empty user object when no arguments are provided.
* **`User(string name, string id);`**: The parameterized constructor. It initializes a new user session with a username and ID.
* **`string getUsername() const;`**: Declares a getter method. The `const` keyword at the end guarantees that this method will *not* modify any variables inside the User class (read-only safety).

### 📄 [User.cpp](file:///c:/Users/Pinto/Downloads/CompSciYr3%20(1)/ManageNoteSystem/User.cpp) (Implementation Source)
* **`#include "User.h"`**: Pulls in the class definition.
* **`User::User() : username(""), userId("") {}`**: Constructor initializer list. It initializes `username` and `userId` to empty strings. The empty brackets `{}` represent an empty function body.
* **`User::User(string name, string id) : username(name), userId(id) {}`**: Directly assigns the constructor arguments `name` and `id` to the private class attributes `username` and `userId`.
* **`string User::getUsername() const { return username; }`**: Implementation of the getter method. It safely retrieves the private variable `username` for external modules (like `main.cpp`).

---

## 2. Note Module

The `Note` class represents individual note cards. It holds all metadata and handles the custom box layout rendering.

### 📄 [Note.h](file:///c:/Users/Pinto/Downloads/CompSciYr3%20(1)/ManageNoteSystem/Note.h)
* **`#include <vector>`**: Includes the standard library vector container to manage dynamic list arrays (used for storing tags).
* **`class Note { ... };`**: Declaration of the Note blueprint.
* **`private:` variables**:
  - `id`: Unique note identifier (int).
  - `title`, `content`, `createdAt`: String values containing note data and timestamps.
  - `tags`: A list of tag strings (`vector<string>`).
  - `isPinned`: Flag indicating priority sorting (`bool`).
* **`public:` methods**:
  - `Note()`: Empty default constructor.
  - `Note(int id, string title, string content, string createdAt, vector<string> tags, bool isPinned = false)`: Initializes all attributes when creating a new note. Note that `isPinned` defaults to `false` if omitted!
  - `update(...)`: Overwrites note attributes when the user edits a note.
  - `display() const`: Custom function that prints a beautifully formatted note box.

### 📄 [Note.cpp](file:///c:/Users/Pinto/Downloads/CompSciYr3%20(1)/ManageNoteSystem/Note.cpp)
* **`#include <iostream>`**: Pulls in input/output streams to print note elements using `cout`.
* **`Note::display() const` (Layout Algorithm)**:
  - `string borderLine = "====..."`: Declares a visual character boundary.
  - `cout << "\n+" << borderLine << "+\n";`: Renders the top border of our card card.
  - `string pinStr = isPinned ? " [★ PINNED]" : "          ";`: Checks if the note is pinned and formats the string.
  - `cout << "| " << titleLine << string(60 - titleLine.length() - 2, ' ') << " |\n";`: Uses string padding subtraction (`60 - length - 2`) to ensure that no matter how long the title is, the right border (`|`) lines up exactly at column 60!
  - **The Word-Wrapping Engine**:
    - We loop character-by-character through `content`.
    - Characters are grouped into a `word` string.
    - If we hit a space (`' '`), we check if the active line length plus the word length exceeds `54` columns.
    - If it does, we push the current `line` to our `wrappedLines` vector and start a new line with `word`.
    - If we hit a newline (`'\n'`), we force push the line to create a clean line break.
    - Finally, we print each wrapped line with `|  ` content `  |` padding, ensuring perfectly aligned boundaries.

---

## 3. Notebook Module

The `Notebook` class groups notes together into folders (directories).

### 📄 [Notebook.h](file:///c:/Users/Pinto/Downloads/CompSciYr3%20(1)/ManageNoteSystem/Notebook.h)
* **`#include "Note.h"`**: Includes the Note class definition so that the Notebook can hold note vectors.
* **`string name;`**: Name of the notebook folder (e.g. "Personal", "ExamNotes").
* **`vector<Note> notes;`**: The dynamic array holding all note cards belonging to this folder.
* **`vector<Note>& getNotes();`**: Returns a reference (`&`) to the note array. Returning by reference is an efficiency optimization; it prevents C++ from duplicating the entire list of notes in memory when calling this function.

### 📄 [Notebook.cpp](file:///c:/Users/Pinto/Downloads/CompSciYr3%20(1)/ManageNoteSystem/Notebook.cpp)
* **`void Notebook::addNote(const Note& note)`**: Appends a copy of `note` to the end of the `notes` vector using `push_back()`.
* **`bool Notebook::deleteNote(int noteId)`**:
  - Starts a loop from `notes.begin()` to `notes.end()` using a vector **iterator** (`auto it`).
  - If it finds a note matching `noteId` (`it->getId() == noteId`), it deletes the note using `notes.erase(it)` and returns `true`.
  - If the loop finishes without finding the note, it returns `false` (error validation).
* **`Note* Notebook::findNote(int noteId)`**:
  - Uses a **range-based for loop** (`for (auto &note : notes)`) to scan notes.
  - If a match is found, it returns the memory address of the note (`&note`). If no match is found, it returns `nullptr` (a safe null pointer).

---

## 4. NoteService Module

The central coordinator of the system, managing all active users, active folders, search operations, and file persistence.

### 📄 [NoteService.h](file:///c:/Users/Pinto/Downloads/CompSciYr3%20(1)/ManageNoteSystem/NoteService.h)
* **`User* currentUser;` / `Notebook* currentNotebook;`**: Dynamic pointer variables pointing to the active session user and currently opened notebook folder. If no user is logged in, these are set to `nullptr`.
* **`int nextNoteId;`**: Increment counter that guarantees every note created gets a unique, incremental ID number automatically.
* **`bool saveToFile(...)` / `bool loadFromFile(...)`**: Declares file-system load and save methods.

### 📄 [NoteService.cpp](file:///c:/Users/Pinto/Downloads/CompSciYr3%20(1)/ManageNoteSystem/NoteService.cpp)
* **`#include <fstream>`**: Includes file streams (`ifstream` for reading from disk, `ofstream` for writing to disk).
* **`#include <sstream>`**: Includes string stream buffers used for tokenizing pipe-delimited data.
* **`toLower(string s)` (Static Search Helper)**:
  - Uses the Standard Template Library (STL) `transform` function along with `tolower` to convert all characters in a string to lowercase. This allows case-insensitive keyword searches.
* **`saveToFile(const string& filepath)` (Database Exporter)**:
  - Opens a file output stream `ofstream file(filepath)`.
  - Serializes the next available note ID (`NEXT_ID:<id>`).
  - Writes registered users list (`USERS:<size>`) followed by `username|userId`.
  - Writes notebooks folder list (`NOTEBOOKS:<size>`).
  - Writes notes metadata `NOTE_META:notebookName|id|isPinned|createdAt|tag1,tag2`.
  - Writes the title string on the next line.
  - Writes the multi-line content body, terminating the note block with a clear `[END_NOTE]` delimiter.
* **`loadFromFile(const string& filepath)` (Database Parser)**:
  - Opens a file input stream `ifstream file(filepath)`.
  - Reads line-by-line using `getline()`.
  - If a line starts with `USERS:`, it loops and tokenizes user profiles using pipe delimiters (`|`).
  - If a line starts with `NOTE_META:`, it tokenizes the metadata fields, reads the title on the subsequent line, and loops content lines into the body until it reads `[END_NOTE]`.
  - Appends the restored Note cards back to their original Notebook folders.
* **`searchNotes(const string& query)`**:
  - Performs case-insensitive matching (`lowTitle.find(lowQuery) != string::npos`).
  - Scans only the active folder if one is open; otherwise, it scans globally across all notebooks.
  - **STL Sorting**: Uses `std::sort` with a custom lambda expression to sort the resulting array so that notes with `isPinned == true` are listed at the very top of search results.

---

## 5. Main Entry & UI Loop

The terminal visual dashboard that coordinates all menu flows.

### 📄 [main.cpp](file:///c:/Users/Pinto/Downloads/CompSciYr3%20(1)/ManageNoteSystem/main.cpp)
* **`#include <limits>`**: Includes numeric limits, letting us clear out keyboard buffers safely.
* **`#include <ctime>`**: Includes traditional C time libraries used to read the system clock and create automatic formatting timestamps (`%Y-%m-%d %H:%M:%S`).
* **`clearInput()`**:
  - `cin.clear();`: Resets the input stream error flags (important if the user enters letters instead of a menu choice).
  - `cin.ignore(numeric_limits<streamsize>::max(), '\n');`: Ignores/removes any residual characters left in the keyboard buffer up to the next newline. This prevents inputs from being skipped!
* **`int main() { ... }`**: The core application runtime loop.
  - Creates a `NoteService ns;` instance.
  - Executes `ns.loadFromFile("notes_db.txt")` on startup to restore all notes.
  - Enters a continuous loop `while (true)`.
  - Renders visual frames using static print functions.
  - Uses state indicators (`ns.getCurrentUser()` and `ns.getCurrentNotebook()`) to check if the session is a guest, an author, or inside an active notebook folder, dynamically rendering the appropriate menu choices!
  - Simulates multi-line writing: When reading body text, it replaces any literal `\n` character strings typed by the user with real C++ carriage returns (`\n`), allowing multi-line paragraphs to write perfectly in the terminal box!
