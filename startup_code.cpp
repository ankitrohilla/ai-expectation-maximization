#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <iomanip>

// format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

string networkFile, recordFile;

void output();

void printIntVector(vector<int> v) {
    for_each( v.begin(), v.end(), [](int i){cout << i << " ";});
    cout << endl;
}
void printFloatVector(vector<float> v) {
    for_each( v.begin(), v.end(), [](float i){
        if( i > 0.1 ) cout << setprecision(2) << i << " ";
        else if( i > 0.01 ) cout << setprecision(1) << i << " ";
        else cout << "0.01 ";
    });
}
void printStringVector(vector<string> v) {
    for_each( v.begin(), v.end(), [](string i){cout << i << " ";});
    cout << endl;
}
int sumIntVector( vector<int> v) {
    int x = 0;
    for_each( v.begin(), v.end(), [&](int v){x+=v;});
    return x;
}

//to be written to the output file
int position[37][2] = {
    54,35,
    184,113,
    145,36,
    68,114,
    111,177,
    32,179,
    238,61,
    564,38,
    640,86,
    738,86,
    682,168,
    564,172,
    722,253,
    226,237,
    229,305,
    366,278,
    289,305,
    220,396,
    154,396,
    195,176,
    308,171,
    120,301,
    31,239,
    329,37,
    1045,292,
    969,258,
    1014,162,
    329,107,
    926,387,
    894,293,
    949,197,
    754,387,
    530,393,
    474,277,
    881,165,
    706,344,
    843,86
};

// our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

public:

    string Node_Name;  // Variable name
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
    vector<vector<int>> permutationResult;
    vector<int> maxIndices;

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

//    sets maxIndices and permutationResult
    void myInitialization();

    void findCPT( vector<int> countFrequencies ) {
        int sizeCPT = CPT.size();
        int total;
        vector<float> newCPT(sizeCPT,-1);

//        for each values of B in P(A|B)
        for( int i = 0; i < sizeCPT/nvalues; i++ ) {
            total = 0;
//            for each values of A given B in P(A|B), find the total number of records
            for( int j = 0; j < nvalues; j++ ) {
                total += countFrequencies[i + j*sizeCPT/nvalues];
            }
//            for each values of A given B in P(A|B), find P(A,B) / P(B)
//            i.e. number of records having both A and B / number of records having B irrespective of {A-"?"}
            for( int j = 0; j < nvalues; j++ ) {
                newCPT[ i + j*sizeCPT/nvalues ] = (float)countFrequencies[i + j*sizeCPT/nvalues] / total;
            }
        }

        set_CPT( newCPT );
    }

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
    int weight = 1;
    patient() { weight = 1; }
    vector<string> data;
};

vector<patient> data;

typedef list<Graph_Node>::iterator Graph_NodeIt;
typedef vector<float>::iterator floatIt;
typedef vector<int>::iterator intIt;
typedef vector<patient>::iterator patientIt;
typedef vector<string>::iterator stringIt;

void Graph_Node::myInitialization() {

    maxIndices.push_back( nvalues-1 );
    for( int i = 0; i < Parents.size(); i++ ) {
        Graph_NodeIt gIt = Alarm.search_node( Parents[i] );
        maxIndices.push_back( (*gIt).nvalues-1 );
    }

    permutationResult = permute(maxIndices);

}

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

        cout << endl << (*it).Node_Name << " has index " << index << " and has been initialized\n";
        (*it).myInitialization();

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
            int total = sumIntVector( countFrequencies );

//            assign the CPT values
            for( floatIt fIt = newCPT.begin(); fIt != newCPT.end(); fIt++ ) {
                (*fIt) = (float)countFrequencies[ fIt - newCPT.begin() ] / (float)total;
            }

            (*it).set_CPT(newCPT);
        } else {

            vector<float> newCPT;
            vector<string> properties, values;
            vector<int> countFrequencies;
            properties.push_back( (*it).Node_Name );
            for_each( (*it).Parents.begin(), (*it).Parents.end(), [&](string s){properties.push_back( s );});

            for_each( (*it).permutationResult.begin(), (*it).permutationResult.end(), [&](vector<int> v){
                values.clear();
                values.push_back( (*it).values[ v[0] ] );

                for( intIt iIt = v.begin()+1; iIt != v.end(); iIt++ ) {
                    Graph_NodeIt gIt = Alarm.search_node( (*it).Parents[iIt-v.begin()-1] );
                    values.push_back( (*gIt).values[ v[iIt-v.begin()] ] );
                }
                printStringVector(values);

//                the first element here is the A in P(A|B) and the rest of the list is B
//                we have to find P(A,B)/P(B)
//                P(A,B) = number of records with both A and B present / total number of records with both A and B non "?"
//                P(B) = number of records with B present / total number of records with both A and B non "?"
                countFrequencies.push_back( countRecords(properties, values) );
            });

            // now, I have to set the newCPT based on all the countFrequencies I got
//            cout << "countFrequencies content -> ";
//            printIntVector( countFrequencies );
            (*it).findCPT( countFrequencies );

        }
        (*it).display();
//        cin.ignore();
    }

//    modify input patient data - E step


    output();

}

void output() {
    cout << "// Bayesian Network in the Interchange Format\n";
    cout << "// Produced by BayesianNetworks package in JavaBayes\n";
    cout << "// Output created Sun Nov 02 17:58:15 GMT+00:00 1997\n";
    cout << "// Bayesian network \n";
    cout << "network \"Alarm\" { //37 variables and 37 probability distributions\n";
    cout << "}\n";

    int index = 0;
    for( Graph_NodeIt gIt = Alarm.Pres_Graph.begin(); gIt != Alarm.Pres_Graph.end(); gIt++, index++ ) {
        string nodeName = (*gIt).Node_Name;
        int nvalues = (*gIt).nvalues;
        cout << "variable  " << nodeName << " { //" << nvalues << " values\n";
        cout << "\ttype discrete[" << nvalues << "] {";
        for_each( (*gIt).values.begin(), (*gIt).values.end(), [](string i){cout << "  " << i;});
        cout << " };\n";

        cout << "\tproperty \"position = ("<< position[index][0] <<", "<< position[index][1] <<")\" ;\n";
        cout << "}\n";
    }

    for( Graph_NodeIt gIt = Alarm.Pres_Graph.begin(); gIt != Alarm.Pres_Graph.end(); gIt++, index++ ) {
        string nodeName = (*gIt).Node_Name;
        vector<string> parents = (*gIt).Parents;
        vector<float> CPT = (*gIt).CPT;

        cout << "probability (  " << nodeName;
        for_each( parents.begin(), parents.end(), [](string i){cout << "  " << i;});
        cout << " ) { //" << 1+parents.size() << " variable(s) and " << CPT.size() << " values\n";
        cout << "\ttable ";
        printFloatVector( CPT );
        cout << ";\n}\n";
    }
}
