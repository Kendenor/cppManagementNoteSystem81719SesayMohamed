# ManageNoteSystem: Presentation Deck & Panel Q&A Bank

Use this document to prepare your presentation slides, learn about the build technologies, and study potential questions that your computer science panel or professor might ask!

---

## 🛠️ Technologies & Tools Used
This project is built using modern C++ standards and relies entirely on standard tools to maximize portability, speed, and clean compiling.

* **Language Standard**: **C++17** (provides clean variable initializations, nested namespaces, and high compiler optimization levels).
* **Compiler**: **g++ (MinGW-w64)** version 14.2.0.
* **IDE Integration**: **Code::Blocks CBP Project File** (GCC Compiler Profile).
* **Standard Template Library (STL) Containers**:
  - `std::vector`: Used for dynamic array allocations, letting us grow notebooks and notes lists in memory without manual memory leaks or static size constraints.
  - `std::string`: Safe character array wrappers avoiding old, crash-prone C-style string buffers (`char*`).
* **Input/Output Utilities**:
  - `std::cin` / `std::cout`: Standard input/output console streams.
  - `std::ifstream` / `std::ofstream`: High-performance disk file reading and writing streams.
  - `std::stringstream`: String buffering used to split text strings by custom tokens.
* **Algorithms Library**:
  - `std::sort`: Uses IntroSort (hybrid of QuickSort, HeapSort, and InsertionSort, running in $O(N \log N)$ complexity) to sort notes.
  - `std::transform`: Efficiently maps elements to low-case representation.
* **Time Utilities (`<ctime>`)**:
  - `std::time`, `std::localtime`, `std::strftime`: Polls the hardware clock to append real creation dates to note cards automatically.

---

## 📊 Presentation Slides Outline
Copy and paste this outline directly onto your slide deck:

### Slide 1: Title & Introduction
* **Title**: ManageNoteSystem - A Premium C++ Note Manager
* **Subtitle**: Applying Object-Oriented Software Design to Personal Productivity
* **Presenter Name**: Pinto (ID: CIF001)

### Slide 2: The Core Problem
* **Traditional Notes Tools**: Heavy resource hogs, require constant internet connections, or lack dynamic structuring.
* **Legacy Code Baseline**: The project began as a basic, incomplete C++ banking system with broken linkages and single-use classes.
* **The Goal**: Build a lightweight, high-performance, modular console note system featuring absolute local persistence, dynamic indexing, and an intuitive state-driven terminal user interface.

### Slide 3: System Architecture
* **Entity Layer**: `User` (encapsulating author profiles), `Note` (encapsulating note metadata).
* **Collection Layer**: `Notebook` (folder groups of notes).
* **Coordination Layer**: `NoteService` (mediates user logs, database operations, and searches).
* **Boundary Layer**: `main.cpp` (state-based UI routing).

### Slide 4: Key Feature: Automatic Persistence Database
* **The Challenge**: Preserving user records, notebooks, and multi-line notes without heavy SQL databases or JSON libraries.
* **The Solution**: A lightweight custom parser in `NoteService`. It exports data using pipe-delimiters (`|`) and handles paragraphs using unique note block indicators (`[END_NOTE]`).
* **Benefit**: Extreme speed, 100% portable text structure, and Zero dependencies.

### Slide 5: Key Feature: Card Box Wrapping & Priority Pinning
* **Visual Excellence**: Note cards are wrapped dynamically to fit a 60-column terminal grid, aligning margins perfectly.
* **Priority Pinning**: Important notes are sorted to the top of list views and keyword search indices.
* **Search Engine**: Case-insensitive substring search across titles and body contents.

### Slide 6: Live System Demo
* **Registering User**: Create account Pintos (ID: CIF001).
* **Notebook Creation**: Make folder `CompSci`.
* **Note Creation**: Write new C++ note with tags.
* **Persistence Test**: Exit program, show `notes_db.txt`, restart, and reload note automatically.

### Slide 7: Conclusion & Summary
* **Lightweight**: Compiles to a tiny binary.
* **Fast**: Searches, sorts, and loads disk data in milliseconds.
* **Future Work**: Add AES-256 local database encryption and multi-user note-sharing files.

---

## 🧠 Panel Q&A Bank: Hard Questions & Pro Answers

Study these answers to sound like an expert during the post-presentation Q&A session!

### Q1: What OOP principles did you apply to this project?
> **Pro Answer**: *"We heavily applied **Encapsulation** and **Separation of Concerns**. Every class holds its private data attributes (like usernames, note content, or vectors of notes), protecting them from direct corruption. External classes can only interact through well-defined public const getter/setter interfaces. Furthermore, we avoided building a 'monolithic' project; main.cpp does not know how notes are saved or searched—it only directs visual options, delegating logic entirely to the coordinator, NoteService."*

### Q2: Why did you build a custom file persistence format instead of using JSON or SQLite?
> **Pro Answer**: *"As a standard console utility, one of our key design goals was **Zero-Dependency Portability**. Integrating libraries like SQLite or nlohmann/JSON would require linking external binaries, which can complicate deployment and compile targets across different OS setups. By building our own line-based, pipe-delimited database engine using `ifstream` and `ofstream`, the program compiles down to a single, portable executable that runs instantly on any machine while still offering highly optimized, reliable persistence."*

### Q3: How do you prevent infinite input loops if a user enters letters when a choice number is requested?
> **Pro Answer**: *"In C++, if you expect an integer via `cin >> integer` and the user enters text, the input stream goes into a 'fail' state, locks up, and can trigger an infinite loop. We resolved this by building a robust `clearInput()` utility. If `cin >> choice` fails, we detect it immediately, reset the stream flags with `cin.clear()`, and discard the invalid keyboard buffer up to the next newline using `cin.ignore(numeric_limits<streamsize>::max(), '\n')`. This makes our UI extremely crash-proof."*

### Q4: Why do some of your functions return references (`vector<Note>&`) and others return pointers (`Note*`)?
> **Pro Answer**: *"This was done to optimize memory efficiency and protect pointer safety:
> 1. We return the notebook notes list by **Reference** (`vector<Note>&`) to avoid C++ copying the entire vector of notes in memory, which is expensive ($O(N)$ overhead). The reference allows us to modify the vector directly.
> 2. We return **Pointers** (`Note*`) for methods like `findNote` because if a note is not found, we can safely return a `nullptr` representing a null state, which main.cpp can check and handle without raising runtime exceptions."*

### Q5: How does your search engine handle note sorting? What is the complexity?
> **Pro Answer**: *"When a search query is submitted, `NoteService::searchNotes` executes a case-insensitive substring search. The matching `Note*` pointers are stored in a temporary vector. We then call `std::sort` with a custom lambda comparitor that evaluates note priority. This pushes notes with `isPinned == true` to the top of the vector. The search takes $O(N \cdot M)$ where $N$ is notes count and $M$ is string search time, and the sorting takes $O(K \log K)$ where $K$ is results count, running practically instantaneously."*

### Q6: How does the word wrapping algorithm function?
> **Pro Answer**: *"In C++, console text overflows if a paragraph string is longer than the console width. In `Note::display()`, we break content into individual words. We monitor an active line length buffer. If adding the next word makes the line exceed 54 columns, the line is written to a vector, and the active line is reset starting with that word. This guarantees that paragraphs are formatted beautifully inside a standard double-sided box layout."*
