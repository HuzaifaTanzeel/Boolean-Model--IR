# Boolean-Model--IR

## How to Run?

1. Install the C/C++ extension for Visual Studio Code.
2. Obtain the latest version of Mingw-w64.
3. Open the Assignment Folder in Visual Studio Code.
4. The folder must contain the following files: a .cpp file, Stopword-List.txt, and all 30 .txt files.

## Compiling and Running in Shell Terminal

```
g++ -o test 20k-0305.cpp
./test
```

## Exceptions

- If the program displays "Wrong Format" after selecting an option, add `cin.ignore()` after `cin >> ask` in the main function.

## Process Flow

1. Read stop words into a vector, eliminating spaces, etc.
2. Data Structure: `vector<pair<string, vector<pair<int, vector<int>>>>> dic;`
   - The outer vector pair represents the term (string), and the inner vector pair represents the Docid (int) and positional indexes of that Docid for that particular term.
3. Read the files.
4. Tokenize the contents of the file, returning tokenized terms along with their positions.
5. Insert pairs of tokens with positions into the dictionary.
   - Insert only the Docid if the term is already present in the dictionary.
   - Insert only the position if the Docid is also present for that term.
6. Sort the dictionary to enable searching with a time complexity of nlogn.
7. Perform stemming along with tokenization.

### Boolean Query Processing

1. The query is converted to lowercase and its contents are stemmed.
2. Convert the query tokens to postfix form to handle complex queries like "not t1 and not t2 and not t3" by giving priority to operators (not > and > or).
3. The postfix tokens are placed in a vector and passed to the query processing function.
4. Iterate over the vector, and if the token is not an operator, find its Docids and store them in a vector, then push it onto the stack.
5. Take out two vectors of Docids from the stack in the case of "and", "or", and perform intersection or union operations, then push back the resultant vector.
6. Take out one vector from the stack in the case of "not" and push it back after applying the "not" operation.
7. The final vector left in the stack will be the result.

### Proximity Query Processing

1. Read and tokenize the query, then apply stemming.
2. Pass term1, term2, and k (the proximity limit) to the function.
3. Use two pointers to iterate over Docids of term1 and term2.
   - In the case of a match, find the difference between positions.
   - If the difference is less than or equal to k, append that Docid to the result vector.
