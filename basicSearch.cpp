// Compile - 
// cd '/home/SamSi/Desktop/IR Proj/Code'
// c++ searchText.cpp porter2_stemmer.cpp -I /usr.local/boost_1_61_0 -lboost_system -lboost_filesystem
// Run - 
// ./a.out ./Dataset/ OR ./a.out /path/to/your/dataset
// ./a.out '/home/SamSi/Desktop/IR Proj/bus'

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include "porter2_stemmer.h"
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <math.h>
#include <algorithm>

#define k 10

using namespace std;

struct docIdScoreMap{													//structure for docId and score
	int docId;
	double score;
};

bool myfunc(struct docIdScoreMap &lhs, struct docIdScoreMap &rhs){		// comparision function to sort array of structures
	return lhs.score > rhs.score;
}

int main(int argc, char *argv[]){
	int numDocuments;
	map<string, vector<pair<int,double> > > termPostingListsMapping;	// termPostingLists stores the posting lists for all the terms in the corpus.
	map<int, pair<string, string> > docIdTitleMapping; 					// docIdTitleMapping stores the title and location of the documents corresponding to a docID.
	map<string, int> termFrequencyMapping;								// stores the frequency of each term in a document
	unordered_map<int, int> docLength;									// to store the length of the documents
	
	boost::filesystem::path path(argv[1]);								//take dataset as first argument while compliling
    boost::filesystem::recursive_directory_iterator itr(path);			//iterate through the directory using "itr" through the corpus

    int assignDocId = 0;												//assigning a doc_id to each document in the corpus    
    string title, fileName, token;
	
    while (itr != boost::filesystem::recursive_directory_iterator()){	//loop till the itr becomes null

    	title.clear();						//clearing the title after each iteration
    	termFrequencyMapping.clear();		//clearing the termFrequencyMapping after each iteration

        fileName = itr->path().string();	// the path to the file is stored as a string in the variable fileName
        

        if (boost::ends_with(fileName, ".txt")){// checks if the path ends with .txt
        	
	        
	        ifstream ifs(fileName);
	        getline(ifs, title);		//get the title of the document
	        string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	        // stores the content of the file in the string variables after setting up a input stream

	        docLength[assignDocId] = title.length() + content.length();
	        // stores the length of the documents for CosineScore algorithm

	        boost::tokenizer<> tok(title);
	        for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
	   			token = *beg;
	   			boost::algorithm::to_lower(token);	   			// case folding to lower case	   			
	   			Porter2Stemmer::stem(token);					// performs stemming on the Token as per Porter's Algorithm	   			
	   			termFrequencyMapping[token]++;
	   		}
	   		//tokenizes and stores the freequency of the tokens in the title of the file

	   		boost::tokenizer<> tok_c(content);			
	   		for(boost::tokenizer<>::iterator beg=tok_c.begin(); beg!=tok_c.end();++beg){//tokenizes and stores the frequency of the tokens //in the content of the file
	       		token = *beg;
	       		boost::algorithm::to_lower(token);		//case folding to lower case	       			       		
	       		Porter2Stemmer::stem(token);			// performs stemming on the Token as per Porter's Algorithm	   			
	       		termFrequencyMapping[token]++;
	   		}
	   		
	        docIdTitleMapping[assignDocId].first = title;
	        docIdTitleMapping[assignDocId].second = fileName;
	        // assigns docId to the documents sequentially and storing their titles in the map

	        for(map<string, int>::iterator it = termFrequencyMapping.begin(); it!= termFrequencyMapping.end(); it++){
	        	termPostingListsMapping[it->first].push_back(make_pair(assignDocId, (1+log10(it->second))));
	        	// pushes the corresponding pair of (docId,tf) to posting list of all terms in the given document
	    	}	        
	        assignDocId++;	      
	    }
        ++itr;
    }
    //In the above while loop index construction and preprocessing is done and various results are stored in the HashMap data-structure

    numDocuments = assignDocId;
    struct docIdScoreMap scores[numDocuments];   
    map<string, int> queryTermFrequency;
	
	
    for(;;){					// loop infinetely for the query
    	
		queryTermFrequency.clear();		
		int flag = 0;
		cout << endl << "Enter the search query (or q to quit)" << endl << endl;
		string query;
		getline(cin, query); 	//take input and store it in query
		

		if(query == "q") 		//user quitting
			return 1;
		

		for(int i=0; i<numDocuments; i++){		// initialize the structure for each query
			scores[i].docId = i;
			scores[i].score = 0.0;
		}
		
		boost::tokenizer<> tok(query);
		// query is broken into tokens in exactly the same way as the documents were broken during indexing
   		for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
   			token = *beg;
   			boost::algorithm::to_lower(token);		//case folding to lower case   			   			
   			Porter2Stemmer::stem(token);			// performs stemming on the Token as per Porter's Algorithm	   		
   			queryTermFrequency[token]++;
   		}
   		// The above loop computes the frequency of all the tokens in the query and stores them in hash map

   		
      int temp_docid = -1;
      double temp_tf = -1;

   		for(map<string, int>::iterator it = queryTermFrequency.begin(); it!= queryTermFrequency.end(); it++){
   			int len = termPostingListsMapping[it->first].size();

   			for(int i=0; i < len; i++){
          double n_q = 0;
          double n_d = 0;
   				flag = 1;
   				// Sigma q.d
          
          if(termPostingListsMapping[it->first][i].second > temp_tf){
            // Updating Doc ID
            temp_docid = termPostingListsMapping[it->first][i].first;
            // Updating TF
            temp_tf = termPostingListsMapping[it->first][i].second;
            //cout << "TF: " << temp_tf << endl;


            //cout << "Path: " << docIdTitleMapping[temp_docid].second << endl;

          }

          for(map<string, int>::iterator itrr = queryTermFrequency.begin(); itrr!= queryTermFrequency.end(); itrr++)
          {double temp = termPostingListsMapping[itrr->first][i].second * (log10(numDocuments/len));

           double temp2 = ((itrr->second > 0)?(1+log10(itrr->second)):0);
           scores[termPostingListsMapping[itrr->first][i].first].score += temp * temp2;
           
           temp2 *= temp2;
           temp *= temp;
           n_d += temp;

           n_q += temp2;}

          scores[termPostingListsMapping[it->first][i].first].score /= (sqrt(n_q) * sqrt(n_d));
   			}
   		}

   		// Computes the final score of all the documents using Score[d] += w(t,q) * wg(t,d)

   		if(flag == 0){	//if the query you entered does not match any document
   			cout << endl << "Sorry the query you entered does not match any of the document" << endl ;
   			continue;
   		}
   		
   		for(int i=0; i<numDocuments; i++)		
   			scores[i].score /= docLength[i];		// divides the documents by their length which is stored in the hash map docLength
   		
   		sort(scores, scores+numDocuments, myfunc);	// sorts the scores in descending order
   		
   		for(int i=0; i<k; i++){
   			cout << scores[i].docId << " -> " << docIdTitleMapping[scores[i].docId].first << " -> " << docIdTitleMapping[scores[i].docId].second << endl;
   		}
      
      //cout << "Final Ans" << endl;
      //cout << docIdTitleMapping[temp_docid].second << endl;

    }
    return 0;
}
