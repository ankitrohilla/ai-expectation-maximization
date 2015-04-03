#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>

// format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

string networkFile, recordFile;

void printIntVector(vector<int> v) {
    for_each( v.begin(), v.end(), [](int i){cout << i << " ";});
    cout << endl;
}
void printFloatVector(vector<float> v) {
    for_each( v.begin(), v.end(), [](float i){cout << i << " ";});
    cout << endl;
}


// our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

public:

    string Node_Name;  // Variable name
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning

	// Constructor- a node is initialised with its name and its categories
    Graph_Node(string name,int n,vector<string> vals)
	{
		Node_Name=name;
		nvalues=n;
        values=vals;
	}
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<string> get_Parents()
	{
		return Parents;
	}
	vector<float> get_CPT()
	{
		return CPT;
	}
	int get_nvalues()
	{
		return nvalues;
	}
	vector<string> get_values()
	{
		return values;
	}
	void set_CPT(vector<float> new_CPT)
	{
		CPT.clear();
		CPT=new_CPT;
    }
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        for(int i=0;i<Children.size();i++)
        {
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }

//    vector of indices as a result of the permutation
//    set of all such vectors as a set of all possible permutations in 000,001 ... 110,111 fashion
    vector<vector<int>> permute(vector<int> maxIndices) {

//        to be returned
        vector<vector<int>> final;
        vector<int> v;

        for( int i = 0; i <= maxIndices[0]; i++ ) {
            v.erase( v.begin(), v.end());
            v.push_back(i);
            if( maxIndices.size() == 1 ) {
                final.push_back(v);
                v.clear();
                continue;
            }
            for( int j = 0; j <= maxIndices[1]; j++ ) {
                v.erase( v.begin()+1, v.end());
                v.push_back(j);
                if( maxIndices.size() == 2 ) {
                    final.push_back(v);
                    v.clear();
                    continue;
                }
                for( int k = 0; k <= maxIndices[2]; k++ ) {
                    v.erase( v.begin()+2, v.end());
                    v.push_back(k);
                    if( maxIndices.size() == 3 ) {
                        final.push_back(v);
                        v.clear();
                        continue;
                    }
                    for( int l = 0; l <= maxIndices[3]; l++ ) {
                        v.erase( v.begin()+3, v.end());
                        v.push_back(l);
                        if( maxIndices.size() == 4 ) {
                            final.push_back(v);
                            v.clear();
                            continue;
                        }
                        for( int m = 0; m <= maxIndices[4]; m++ ) {
                            v.erase( v.begin()+4, v.end());
                            v.push_back(m);
                            if( maxIndices.size() == 5 ) {
                                final.push_back(v);
                                v.clear();
                                continue;
                            }
                            if( maxIndices.size() != 5 ) cout << "IMPOSSIBLE";
                        }
                    }
                }
            }
        }

        return final;

    }

    void display();

};

// The whole network represted as a list of nodes
class network{

public:

	list <Graph_Node> Pres_Graph;

	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
		return 0;
	}
    
    
	int netSize()
	{
		return Pres_Graph.size();
	}
    // get the index of node with a given name
    int get_index(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return count;
            count++;
        }
        return -1;
    }
    // get the node at nth index
    list<Graph_Node>::iterator get_nth_node(int n)
    {
       list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(count==n)
                return listIt;
            count++;
        }
        return listIt; 
    }
    //get the iterator of a node with a given name
    list<Graph_Node>::iterator search_node(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return listIt;
        }
    
            cout<<"node not found\n";
        return listIt;
    }
	

}Alarm;

class patient {
//    size of this list is the number of nodes in the network
public:
    vector<string> data;
};

vector<patient> data;

typedef list<Graph_Node>::iterator Graph_NodeIt;
typedef vector<float>::iterator floatIt;
typedef vector<int>::iterator intIt;
typedef vector<patient>::iterator patientIt;
typedef vector<string>::iterator stringIt;


void Graph_Node::display()
{
    cout << "\nNode name - " << Node_Name;
    cout << "\nValues that can be taken - ";
    for_each( values.begin(), values.end(), [](string s){cout << s << " ";});
    cout << "\nMy parents -> ";
    for_each( Parents.begin(), Parents.end(), [](string p){cout << p << " ";});
    cout << "\nMy CPT contents -> ";
    for_each( CPT.begin(), CPT.end(), [](float c){cout << c << " ";});
    cout << "\nMy CPT meanings -> \n";

    vector<int> maxIndices;
    maxIndices.push_back( nvalues-1 );
    for( int i = 0; i < Parents.size(); i++ ) {
        Graph_NodeIt gIt = Alarm.search_node( Parents[i] );
        maxIndices.push_back( (*gIt).nvalues-1 );
    }

    vector<vector<int>> permutationResult = permute(maxIndices);

    int CPTindex = 0;
    for_each( permutationResult.begin(), permutationResult.end(), [&](vector<int> v){
        cout << "P(" << Node_Name << "=" << values[v[0]] << "|";
        for( intIt it = v.begin()+1; it != v.end(); it++){
            Graph_NodeIt gIt = Alarm.search_node(Parents[it-v.begin()-1]);
            cout << Parents[it-v.begin()-1] << "=" << (*gIt).values[v[it-v.begin()]];
        }
        cout << ") = " << CPT[CPTindex++] << "\n";
    });

}


network read_network()
{
	network Alarm;
	string line;
	int find=0;
    ifstream myfile(networkFile);
  	string temp;
  	string name;
  	vector<string> values;
  	
    if (myfile.is_open())
    {
    	while (! myfile.eof() )
    	{
    		stringstream ss;
      		getline (myfile,line);
      		
      		
      		ss.str(line);
     		ss>>temp;
     		
     		
     		if(temp.compare("variable")==0)
     		{
                    
     				ss>>name;
     				getline (myfile,line);
                   
     				stringstream ss2;
     				ss2.str(line);
     				for(int i=0;i<4;i++)
     				{
     					
     					ss2>>temp;
     					
     					
     				}
     				values.clear();
     				while(temp.compare("};")!=0)
     				{
     					values.push_back(temp);
     					
     					ss2>>temp;
    				}
     				Graph_Node new_node(name,values.size(),values);
     				int pos=Alarm.addNode(new_node);

     				
     		}
     		else if(temp.compare("probability")==0)
     		{
                    
     				ss>>temp;
     				ss>>temp;
     				
                    list<Graph_Node>::iterator listIt;
                    list<Graph_Node>::iterator listIt1;
     				listIt=Alarm.search_node(temp);
                    int index=Alarm.get_index(temp);
                    ss>>temp;
                    values.clear();
     				while(temp.compare(")")!=0)
     				{
                        listIt1=Alarm.search_node(temp);
                        listIt1->add_child(index);
     					values.push_back(temp);
     					
     					ss>>temp;

    				}
                    listIt->set_Parents(values);
    				getline (myfile,line);
     				stringstream ss2;
                    
     				ss2.str(line);
     				ss2>> temp;
                    
     				ss2>> temp;
                    
     				vector<float> curr_CPT;
                    string::size_type sz;
     				while(temp.compare(";")!=0)
     				{
                        curr_CPT.push_back(atof(temp.c_str()));
     					
     					ss2>>temp;

    				}
                    
                    listIt->set_CPT(curr_CPT);


     		}
            else
            {
                
            }
        }
    	
    	if(find==1)
    	myfile.close();
  	}
  	
  	return Alarm;
}

// reads the input data file and fills up the list named data
void read_data() {

    ifstream myfile(recordFile);
    string line;
    int i = 0;
    while( !myfile.eof() ) {
        getline(myfile,line);

        patient p;
        string lin = line;
        int y;

        for(int i = 0; i < Alarm.netSize(); i++ ) {
            y = lin.find(" ");
            p.data.push_back( lin.substr(0,y) );
            lin = lin.substr(y+1,lin.length()-y);
        }
        data.push_back(p);
    }
    cout << "Data filled with data size - " << data.size() << endl;
}

//returns the number of patient records where given properties have given values
int countRecords( vector<string> properties, vector<string> values ) {
    int x = 0;
    vector<int> propertyIndices;
    for_each( properties.begin(), properties.end(), [&](string s){
        propertyIndices.push_back( Alarm.get_index( s ) );
    });

//    count here for each patient
    for_each( data.begin(), data.end(), [&](patient p){
        bool present = true;
        for( intIt iIt = propertyIndices.begin(); iIt != propertyIndices.end(); iIt++ ){
//            if the value of data present at this index is equal to the one given in the input
            if( p.data[*iIt] != values[iIt-propertyIndices.begin()] )
                present = false;
        }
        if( present )
            x++;
    });

//    laplace smoothing
    return x+1;
}

int main( int argc, char** argv )
{
    if( argc != 3 )
        exit(1);

    networkFile = argv[1];
    recordFile = argv[2];

	Alarm=read_network();

    read_data();

    for_each( Alarm.Pres_Graph.begin(), Alarm.Pres_Graph.end(), [](Graph_Node gn) {
        cout << gn.Node_Name << " has values ";
        for_each( gn.values.begin(), gn.values.end(), [](string s){cout << s << " ";});
        cout << endl;
    });

    cout << "Net size is " << Alarm.netSize() << endl;

//    APPLY COUNTING AND IGNORE THE ? VALUES

    vector<string> p,v;

    int index = 0;
    for( Graph_NodeIt it = Alarm.Pres_Graph.begin(); it != Alarm.Pres_Graph.end(); it++, index++ ) {

        cout << endl << (*it).Node_Name << " has index " << index;

//        if find the prior probability of this node who has no parent
//        else find the CPT of this node who has some parents
        if( (*it).Parents.empty() ) {

//            index i of this list is incremented when corresponding value is found in the data
            vector<int> countFrequencies((*it).nvalues,0);
            vector<float> newCPT((*it).nvalues,0);

//            check the value stored in each patient record
            for_each( data.begin(), data.end(), [&](patient p){
                if( p.data[index] != "\"?\"" ) {
                    stringIt sIt = find( (*it).values.begin(), (*it).values.end(), p.data[index] );
                    countFrequencies[ sIt- (*it).values.begin() ]++;
                }
            });

//            total will be number of patient records - ? corresponding to this entry
            int total = 0;
            for_each( countFrequencies.begin(), countFrequencies.end(), [&](int i){total+=i;});

//            assign the CPT values
            for( floatIt fIt = newCPT.begin(); fIt != newCPT.end(); fIt++ ) {
                (*fIt) = (float)countFrequencies[ fIt - newCPT.begin() ] / (float)total;
            }

            (*it).set_CPT(newCPT);
        } else {
//            FIND THE CPT HERE;
        }
        cin.ignore();
        (*it).display();
    }

    cout << endl;
}
