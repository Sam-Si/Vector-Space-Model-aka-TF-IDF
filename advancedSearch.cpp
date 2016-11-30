#include <bits/stdc++.h>
using namespace std;
#define BUFFER_SIZE 1024

int N =2225; //Total no. of documents in the corpus

/* Structures Defined */
typedef struct node{
	int doc_id;
	int frequency;
	double score;
	struct node *right;
}node; //Details of Document w.r.t Term

typedef struct terms{
	char s[50];	//The longest English word is no more than 45 characters long.
	int t_frequency;
	int doc_frequency;
	int flag;
	struct node *head;
	struct node *tail;
}terms; //Details of terms in the corpus

/* Functions */

//Function that calculates the idf value, given df.
double idf(int df){
	double ret;
	ret = log10(N/df);
	return df;
}

// Calculates term frequency in the query.
int calre(char x[],char* y[]){
	int i;
	int count=0;
	for(i=0;y[i]!=NULL;i++){
		if(strcmp(x,y[i])==0){
			count++;
		}
	}
	return count;
}

// Hasher function, gives unique Hasher value for a term
int Hasher(char *str){
        long Hasher = 101;
        int c;

        while (c = *str++){
        	if(isalpha(c))
            Hasher = ((Hasher << 4) + Hasher) + c; /* Hasher * 17 + c */
		}
        return abs(Hasher%1100000);
    }

// Normalises term to remove noise.
void rem(char token[]){
	int i;
	for(i=0;i<=strlen(token);i++){
		if(!isalpha(token[i])){
			token[i]='\0';
			break;
		}
	}
}

// Sorts and Merges two postings lists.
node* SortedMerge(node* a, node* b)
{
    node* result = NULL;

    // Base Cases
    if (a == NULL)
        return NULL;
    else if(b == NULL)
        return NULL;

    // Pick either a or b, and recurse
    if(a->doc_id < b->doc_id)
    {
        SortedMerge(a->right, b);
    }
    else
    {
         SortedMerge(a, b->right);
    }
    if(a->doc_id==b->doc_id){
    	result = a;
    	result->right=SortedMerge(a, b->right);
    }

    return result;
}

// The main function that takes an array of lists
// arr[0..last] and generates the sorted output
node* mergeKLists(node* arr[], int last)
{
    // repeat until only one list is left
    while (last != 0)
    {
        int i = 0, j = last;

        // (i, j) forms a pair
        while (i < j)
        {
            // merge List i with List j and store
            // merged list in List i
            arr[i] = SortedMerge(arr[i], arr[j]);

            // consider right pair
            i++, j--;

            // If all pairs are merged, update last
            if (i >= j)
                last = j;
        }
    }

    return arr[0];
}

// Main Function
int main()
{
	int i,j,k;
	terms *a = (terms*)malloc(1100000*sizeof(terms));
	// total number of words in the english language is 1025110(avg), based on Jan 1st, 2014, rounded to 1100000.

	//initalises the terms in the Heap Memory
	for(i=0;i<1100000;i++){
		a[i].doc_frequency=0;
		a[i].head=NULL;
		a[i].flag=0;
		strcpy(a[i].s,"");
		a[i].tail=NULL;
	}

	int d_len = 0;
	char ch[10];
	node *temp;
	char in[10];
	char buffer[ BUFFER_SIZE ];
	const char *delimiter_characters = " ";
    char *last_token;
    printf("Please wait until it is sets up\n");
	for(i=1;i<=N;i++){
		if(i<10){
			strcpy(in,"bus/00");
		}
		else if(i>=10&&i<100){
			strcpy(in,"bus/0");
		}
		else{
			strcpy(in,"bus/");
		}
		sprintf(ch,"%d",i);
		strcat(in,ch);
		strcat(in,".txt");
    FILE *input_file = fopen( in, "r" );


    if( input_file == NULL ){

        fprintf( stderr, "Unable to open file %s\n", in );

    }else{

        // Read each line into the buffer
        while( fgets(buffer, BUFFER_SIZE, input_file) != NULL ){
            // Write the line to stdout
            //fputs( buffer, stdout );

            // Gets each token as a string.
            last_token = strtok( buffer, delimiter_characters );
            while( last_token != NULL ){
			if(strcmp(last_token,"")==0){ //removes all empty strings i.e. it doesn't Hasher any empty strings
				continue;
			}
            rem(last_token); //removes all noise form the token, like commas, apostrophes etc.
// stem(last_token,0,strlen(last_token));
            // Main function of the Porter's Algorithm that calls the other 9 steps
            k=Hasher(last_token);
			/*printf("%d\n",k);			*/

            // adds the token into the inverted index.
			if(strcmp(a[k].s,"")!=0){
				node *tem;
				int flag =0;
				tem = a[k].head;
				a[k].t_frequency++;
				while(tem!=NULL){
					if(tem->doc_id==i){
						flag =1;
						tem->frequency++;
						break;
					}
					tem=tem->right;
				}
				if(flag==0){
					temp = (node*)malloc(sizeof(node));
					temp->doc_id=i;
					temp->frequency=1;
					temp->right=NULL;
					(a[k].tail)->right=temp;
					a[k].tail=temp;
					a[k].doc_frequency++;

				}
			}
			else{
				strcpy(a[k].s,last_token);
				temp = (node*)malloc(sizeof(node));
				temp->doc_id=i;
				temp->frequency=1;
				temp->right=NULL;
				a[k].head=temp;
				a[k].tail=temp;
				a[k].doc_frequency=1;
				a[k].t_frequency=1;
			}
                last_token = strtok( NULL, delimiter_characters );
            }

        }

		}
        if( ferror(input_file)) {
            perror( "The following error occurred" );
        }

        fclose( input_file );
    }
    i=0;
    char query[101],str[10][50];
	char *d[10];
		/* Assuming the maximum length to be of the query to be 100 character as average google query is 3 words (rounded to maximum
    	* and average length of english word is 5 letters there for no longer than 100 characters 1 for '/0' */
    	printf("Enter your query\n");
		fgets(query,101,stdin);
		query[strlen(query)-1]='\0';
		last_token = strtok(query,delimiter_characters); //Tokenizes the query

		while(last_token!=NULL){
			k = Hasher(last_token);
			temp = a[k].head;
			while(temp!=NULL){
				temp->score = 1+log10(temp->frequency);
				temp = temp->right;
			}
			strcpy(str[i],last_token);
			i++;
			last_token = strtok( NULL, delimiter_characters );
		}
		node *ten[i];
		strcpy(str[i],"");
		int m = i;
		i=0;
		while(strcmp(str[i],"")!=0){
			k = Hasher(str[i]);
			ten[i] = a[k].head;
			i++;
		}
		/* this function merges the sorted posting lists
		*  to one list which is pointed by header*/
		node *head = mergeKLists(ten,i-1);
		if(head==NULL){
			printf("No document matches the given query\n");
			return 0;
		}

		//Prints all the relevant document IDs
		while(head!=NULL){
			printf("%d\n",head->doc_id);
			head = head->right;

		}



	return 0;
}
