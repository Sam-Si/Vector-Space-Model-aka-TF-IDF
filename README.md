# Vector-Space-Model-aka-TF-IDF
Vector space model or term vector model is an algebraic model for representing text documents (and any objects, in general) as vectors of identifiers, such as, for example, index terms. It is used in information filtering, information retrieval, indexing and relevancy rankings.

## Requirements: 
The system needs to have the BOOST Library installed (for Linux). 
The library can be obtained from the following link. 

[BOOST](http://www.boost.org/doc/libs/1_61_0/more/getting_started/unix-variants.html)

To install boost follow the below steps 
```
  1. Extract file.tar.bz2
  2. Run ./boostrap.sh
  3. Run ./b2 install
```
After installing the BOOST Library, the program can now be compiled and
 executed. The instructions are given below:

### Compile -

```
c++ basicSearch.cpp porter2_stemmer.cpp -I /usr/local/boost_1_61_0 -lboost_system -lboost_filesystem
```

### Run -

```
./a.out ./Dataset/ OR ./a.out /path/to/your/dataset
```

## The Basic Search - Vector Space Model

* The files of the corpus are accessed through the use of an object in the C++ Boost Library,
  called the recursive_directory_iterator. This object provides standard library compliant iter-
  ation over the contents of a directory, and recursively into sub directories as well.
  
* One important data structure used is called a map, available in the C++ STL library. The
  dictionary is a collection of these maps (terPosLisMap in the program), where each map
  contains a string holding the terms, and an extending tuple that contains the docID and TF
  score that is computed when necessary.
  
* Another one is the ’docIdTitleMap’ that stores the docId, it’s corresponding file name, and
  file location. Another is the ’termFMap’ that holds a term and its frequency w.r.t to the docu-
  ment, used in calculating the TF score that is to be stored in the dictionary.
  
* The directory_iterator recursively accesses each directory and takes the 1st file that is to be
  tokenized. The first word is taken, case-folded, stemmed (using the Porter2Stemmer) and
  then stored before moving onto the next word. If the word repeats, the term frequency is
  incremented (w.r.t the document) and the process is repeated for all words, and in all doc-
  uments of the corpus. The pair of docId and tf is pushed into the terPosLisMap in order to
  create the dictionary.
  
* The dictionary has now been generated. The query is now taken, tokenized and stemmed.
  With the query being tokenized, we calculate the TF-IDF score that is required for ranking
  our documents using the TF score stored in the terPosLisMap.
  The formula to calculate the score is given by: 

```  
   Score[d] += w(t,q) * wg(t,d)
```  

* A structure is defined to hold the docId and the TF-IDF score that is calculated. An array
  of structures holds the docIds and their corresponding TF-IDF scores, which is sorted in de-
  scending order of their scores. The first 10 among them are displayed to the user as the re-
  sults of the query.
  

# The Advanced Search Boolean Retrieval Model

* The code contains two major structures, the nodes for the postings list (it is implemented as
  **  linked list) and the hash table (the dictionary).
      Each node stores a document ID (int doc_id), a term frequency corresponding to the
      particular document (int frequency), and the frequency score (double score) along with a
      pointer to the next node.
  **  The dictionary is implemented as a hash table, where each unit has a structure called
      terms. Each term has a string to store the term (char s[50]), the total term frequency
      in the corpus (t_frequency), the document frequency (doc_frequency). The maximum
      length of the term is taken to be 50 bytes, since the longest English word is around 45
      characters
  
* The advanced search program takes one line at a time, tokenizes it and each token is pro-
  cessed through a Case-Folding algorithm and is then added to the inverted index. The doc-
  ument Frequency and term frequency is calculated and updated in the index if the term al-
  ready exists. If not, a new entry for the term is created. This process is repeated for all the
  words in the document, and for every document in the corpus, and the index is constructed.
  
* The dictionary is implemented as a hash table, with a hash functions that uses two prime
  numbers, 101 and 17, to get a unique hash value for each term. The size of the dictionary
  has a limit of 1,100,000 words, being the maximum number of English words as of Jan 21,
  2014.
  
* After the inverted index has been completely constructed, it now takes the query and tok-
  enizes it. Each token generated has a posting list in the inverted index, and these posting
  lists are merged to give a final postings lists that contains the list of documents in which the
  query terms occur. Spaces between terms is taken to be an ’and’ of the terms and the solu-
  tion contains only those documents that have all of the terms tokenized from the query.
