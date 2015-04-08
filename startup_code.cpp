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
#include <map>

#define getNodeFromIndex( i ) (*(Alarm.get_nth_node(i)))

// format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

string networkFile, recordFile;

int dataSize = 0;

map<string, float> CPT_map;
map<string, float> MCPT_map;

void output();

typedef vector<float>::iterator floatIt;
typedef vector<int>::iterator intIt;
typedef vector<string>::iterator stringIt;

typedef map<string, float>::iterator cptMapIt;

void printIntVector(vector<int> v) {
	for_each( v.begin(), v.end(), [](int i) {cout << i << " ";});
	cout << endl;
}
void printFloatVector(vector<float> v) {
for_each( v.begin(), v.end(), [](float i) {
			if( i > 0.99 ) cout << "0.99 ";
			else if( i > 0.1 ) cout << setprecision(2) << i << " ";
			else if( i > 0.01 ) cout << setprecision(1) << i << " ";
			else cout << "0.01 ";
		});
}
void printStringVector(vector<string> v) {
	for_each( v.begin(), v.end(), [](string i) {cout << i << " ";});
	cout << endl;
}
int sumIntVector(vector<int> v) {
	int x = 0;
	for_each( v.begin(), v.end(), [&](int v) {x+=v;});
	return x;
}
float sumFloatVector(vector<float> v) {
	float x = 0;
	for_each( v.begin(), v.end(), [&](float v) {x+=v;});
	return x;
}
int findIndexString(vector<string> vs, string s) {
	stringIt sIt = find(vs.begin(), vs.end(), s);
	return sIt - vs.begin();
}

//to be written to the output file
int position[37][2] = { 54, 35, 184, 113, 145, 36, 68, 114, 111, 177, 32, 179,
		238, 61, 564, 38, 640, 86, 738, 86, 682, 168, 564, 172, 722, 253, 226,
		237, 229, 305, 366, 278, 289, 305, 220, 396, 154, 396, 195, 176, 308,
		171, 120, 301, 31, 239, 329, 37, 1045, 292, 969, 258, 1014, 162, 329,
		107, 926, 387, 894, 293, 949, 197, 754, 387, 530, 393, 474, 277, 881,
		165, 706, 344, 843, 86 };

void set_MCPT_MAP();
void set_CPT_MAP();

string CPT_Keys[][12] = {
    { "\"Hypovolemia\"" , "EOF" },
    { "\"StrokeVolume\"", "\"LVFailure\"", "\"Hypovolemia\"" , "EOF" },
    { "\"LVFailure\"" , "EOF" },
    { "\"LVEDVolume\"", "\"Hypovolemia\"", "\"LVFailure\"" , "EOF" },
    { "\"PCWP\"", "\"LVEDVolume\"" , "EOF" },
    { "\"CVP\"", "\"LVEDVolume\"" , "EOF" },
    { "\"History\"", "\"LVFailure\"" , "EOF" },
    { "\"MinVolSet\"" , "EOF" },
    { "\"VentMach\"", "\"MinVolSet\"" , "EOF" },
    { "\"Disconnect\"" , "EOF" },
    { "\"VentTube\"", "\"VentMach\"", "\"Disconnect\"" , "EOF" },
    { "\"KinkedTube\"" , "EOF" },
    { "\"Press\"", "\"\"KinkedTube\"", "\"Intubation\"", "\"VentTube\"" , "EOF" },
    { "\"ErrLowOutput\"" , "EOF" },
    { "\"HRBP\"", "\"ErrLowOutput\"", "\"HR\"" , "EOF" },
    { "\"ErrCauter\"" , "EOF" },
    { "\"HREKG\"", "\"HR\"", "\"ErrCauter\"" , "EOF" },
    { "\"HRSat\"", "\"HR\"", "\"ErrCauter\"" , "EOF" },
    { "\"BP\"", "\"CO\"", "\"TPR\"" , "EOF" },
    { "\"CO\"", "\"HR\"", "\"StrokeVolume\"" , "EOF" },
    { "\"HR\"", "\"Catechol\"" , "EOF" },
    { "\"TPR\"", "\"Anaphylaxis\"" , "EOF" },
    { "\"Anaphylaxis\"" , "EOF" },
    { "\"InsuffAnesth\"" , "EOF" },
    { "\"PAP\"", "\"PulmEmbolus\"" , "EOF" },
    { "\"PulmEmbolus\"" , "EOF" },
    { "\"FiO2\"" , "EOF" },
    {"\"Catechol\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"" , "EOF" },
    { "\"SaO2\"", "\"Shunt\"", "\"PVSat\"" , "EOF" },
    { "\"Shunt\"", "\"PulmEmbolus\"", "\"Intubation\"" , "EOF" },
    { "\"PVSat\"", "\"VentAlv\"", "\"FiO2\"" , "EOF" },
    { "\"MinVol\"", "\"VentLung\"", "\"Intubation\"" , "EOF" },
    { "\"ExpCO2\"", "\"ArtCO2\"", "\"VentLung\"" , "EOF" },
    { "\"ArtCO2\"", "\"VentAlv\"" , "EOF" },
    { "\"VentAlv\"", "\"Intubation\"", "\"VentLung\"" , "EOF" },
    { "\"VentLung\"", "\"KinkedTube\"", "\"VentTube\"", "\"Intubation\"" , "EOF" },
    { "\"Intubation\"" , "EOF" },

    { "\"LVFailure\"", "\"Hypovolemia\"" , "EOF" },
    { "\"Hypovolemia\"", "\"LVFailure\"" , "EOF" },
    { "\"LVEDVolume\"" , "EOF" },
    { "\"LVFailure\"" , "EOF" },
    { "\"MinVolSet\"" , "EOF" },
    { "\"VentMach\"", "\"Disconnect\"" , "EOF" },
    { "\"KinkedTube\"", "\"Intubation\"", "\"VentTube\"" , "EOF" },
    { "\"ErrLowOutput\"", "\"HR\"" , "EOF" },
    { "\"HR\"", "\"ErrCauter\"" , "EOF" },
    { "\"CO\"", "\"TPR\"" , "EOF" },
    { "\"HR\"", "\"StrokeVolume\"" , "EOF" },
    { "\"Catechol\"" , "EOF" },
    { "\"Anaphylaxis\"" , "EOF" },
    { "\"PulmEmbolus\"" , "EOF" },
    { "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"" , "EOF" },
    { "\"Shunt\"", "\"PVSat\"" , "EOF" },
    { "\"PulmEmbolus\"", "\"Intubation\"" , "EOF" },
    { "\"VentAlv\"", "\"FiO2\"" , "EOF" },
    { "\"VentLung\"", "\"Intubation\"" , "EOF" },
    { "\"ArtCO2\"",	"\"VentLung\"" , "EOF" },
    { "\"VentAlv\"" , "EOF" },
    { "\"Intubation\"", "\"VentLung\"" , "EOF" },
    { "\"KinkedTube\"", "\"VentTube\"", "\"Intubation\"" , "EOF" },

};

string mCPT_Keys[][12] = {
    { "\"Hypovolemia\"", "\"StrokeVolume\"", "\"LVEDVolume\"", "\"LVFailure\"", "EOF" },
    { "\"StrokeVolume\"", "\"LVFailure\"", "\"Hypovolemia\"", "\"CO\"", "\"HR\"", "EOF" },
    { "\"LVFailure\"", "\"StrokeVolume\"", "\"LVEDVolume\"", "\"History\"", "\"Hypovolemia\"", "EOF" },
    { "\"LVEDVolume\"", "\"Hypovolemia\"", "\"LVFailure\"", "\"PCWP\"", "\"CVP\"", "EOF" },
    { "\"PCWP\"", "\"LVEDVolume\"", "EOF" },
    { "\"CVP\"", "\"LVEDVolume\"", "EOF" },
    { "\"History\"", "\"LVFailure\"", "EOF" },
    { "\"MinVolSet\"", "\"VentMach\"", "EOF" },
    { "\"VentMach\"", "\"MinVolSet\"", "\"VentTube\"", "\"Disconnect\"", "EOF" },
    { "\"Disconnect\"", "\"VentTube\"", "\"VentMach\"", "EOF" },
    { "\"VentTube\"", "\"VentMach\"", "\"Disconnect\"", "\"Press\"", "\"VentLung\"", "\"KinkedTube\"", "\"Intubation\"", "EOF" },
    { "\"KinkedTube\"", "\"Press\"", "\"VentLung\"", "\"Intubation\"", "\"VentTube\"", "EOF" },
    { "\"Press\"", "\"KinkedTube\"", "\"Intubation\"", "\"VentTube\"", "EOF" },
    { "\"ErrLowOutput\"", "\"HRBP\"", "\"HR\"", "EOF" },
    { "\"HRBP\"", "\"ErrLowOutput\"", "\"HR\"", "EOF" },
    { "\"ErrCauter\"", "\"HREKG\"", "\"HRSat\"", "\"HR\"", "EOF" },
    { "\"HREKG\"", "\"HR\"", "\"ErrCauter\"", "EOF" },
    { "\"HRSat\"", "\"HR\"", "\"ErrCauter\"", "EOF" },
    { "\"BP\"", "\"CO\"", "\"TPR\"", "EOF" },
    { "\"CO\"", "\"HR\"", "\"StrokeVolume,BP\"", "\"TPR\"", "EOF" },
    { "\"HR\"", "\"Catechol\"", "\"HRBP\"", "\"HREKG\"", "\"HRSat\"", "\"CO\"", "\"ErrLowOutput\"", "\"ErrCauter\"", "\"StrokeVolume\"", "EOF" },
    { "\"TPR\"", "\"Anaphylaxis\"", "\"BP\"", "\"Catechol\"", "\"CO\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"ArtCO2\"", "EOF" },
    { "\"Anaphylaxis\"", "\"TPR\"", "EOF" },
    { "\"InsuffAnesth\"", "\"Catechol\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"", "EOF" },
    { "\"PAP\"", "\"PulmEmbolus\"", "EOF" },
    { "\"PulmEmbolus\"", "\"PAP\"", "\"Shunt\"", "\"Intubation\"", "EOF" },
    { "\"FiO2\"", "\"PVSat\"", "\"VentAlv\"", "EOF" },
    { "\"Catechol\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"", "\"HR\"", "EOF" },
    { "\"SaO2\"", "\"Shunt\"", "\"PVSat\"", "\"Catechol\"", "\"InsuffAnesth\"", "\"TPR\"", "\"ArtCO2\"", "EOF" },
    { "\"Shunt\"", "\"PulmEmbolus\"", "\"Intubation\"", "\"SaO2\"", "\"PVSat\"", "EOF" },
    { "\"PVSat\"", "\"VentAlv\"", "\"FiO2\"", "\"SaO2\"", "\"Shunt\"", "EOF" },
    { "\"MinVol\"", "\"VentLung\"", "\"Intubation\"", "EOF" },
    { "\"ExpCO2\"", "\"ArtCO2\"", "\"VentLung\"", "EOF" },
    { "\"ArtCO2\"", "\"VentAlv\"", "\"Catechol\"", "\"ExpCO2\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"VentLung\"", "EOF" },
    { "\"VentAlv\"", "\"Intubation\"", "\"VentLung\"", "\"PVSat\"", "\"ArtCO2\"", "\"FiO2\"", "EOF" },
    { "\"VentLung\"", "\"KinkedTube\"", "\"VentTube\"", "\"Intubation\"", "\"MinVol\"", "\"ExpCO2\"", "\"VentAlv\"", "\"ArtCO2\"", "EOF" },
    { "\"Intubation\"", "\"Press\"", "\"Shunt\"", "\"MinVol\"", "\"VentAlv\"", "\"VentLung\"", "\"KinkedTube\"", "\"VentTube\"", "\"PulmEmbolus\"", "EOF" },

    { "\"StrokeVolume\"", "\"LVEDVolume\"", "\"LVFailure\"", "EOF" },
    { "\"LVFailure\"", "\"Hypovolemia\"", "\"CO\"", "\"HR\"", "EOF" },
    { "\"StrokeVolume\"", "\"LVEDVolume\"", "\"History\"", "\"Hypovolemia\"", "EOF" },
    { "\"Hypovolemia\"", "\"LVFailure\"", "\"PCWP\"", "\"CVP\"", "EOF" },
    { "\"LVEDVolume\"", "EOF" },
    { "\"LVFailure\"", "EOF" },
    { "\"VentMach\"", "EOF" },
    { "\"MinVolSet\"", "\"VentTube\"", "\"Disconnect\"", "EOF" },
    { "\"VentTube\"", "\"VentMach\"", "EOF" },
    { "\"VentMach\"", "\"Disconnect\"", "\"Press\"", "\"VentLung\"", "\"KinkedTube\"", "\"Intubation\"", "EOF" },
    { "\"Press\"", "\"VentLung\"", "\"Intubation\"", "\"VentTube\"", "EOF" },
    { "\"KinkedTube\"", "\"Intubation\"", "\"VentTube\"", "EOF" },
    { "\"HRBP\"", "\"HR\"", "EOF" },
    { "\"ErrLowOutput\"", "\"HR\"", "EOF" },
    { "\"HREKG\"", "\"HRSat\"", "\"HR\"", "EOF" },
    { "\"HR\"", "\"ErrCauter\"", "EOF" },
    { "\"CO\"", "\"TPR\"", "EOF" },
    { "\"HR\"", "\"StrokeVolume,BP\"", "\"TPR\"", "EOF" },
    { "\"Catechol\"", "\"HRBP\"", "\"HREKG\"", "\"HRSat\"", "\"CO\"", "\"ErrLowOutput\"", "\"ErrCauter\"", "\"StrokeVolume\"", "EOF" },
    { "\"Anaphylaxis\"", "\"BP\"", "\"Catechol\"", "\"CO\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"ArtCO2\"", "EOF" },
    { "\"TPR\"", "EOF" },
    { "\"Catechol\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"", "EOF" },
    { "\"PulmEmbolus\"", "EOF" },
    { "\"PAP\"", "\"Shunt\"", "\"Intubation\"", "EOF" },
    { "\"PVSat\"", "\"VentAlv\"", "EOF" },
    { "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"", "\"HR\"", "EOF" },
    { "\"Shunt\"", "\"PVSat\"", "\"Catechol\"", "\"InsuffAnesth\"", "\"TPR\"", "\"ArtCO2\"", "EOF" },
    { "\"PulmEmbolus\"", "\"Intubation\"", "\"SaO2\"", "\"PVSat\"", "EOF" },
    { "\"VentAlv\"", "\"FiO2\"", "\"SaO2\"", "\"Shunt\"", "EOF" },
    { "\"VentLung\"", "\"Intubation\"", "EOF" },
    { "\"ArtCO2\"", "\"VentLung\"", "EOF" },
    { "\"VentAlv\"", "\"Catechol\"", "\"ExpCO2\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"VentLung\"", "EOF" },
    { "\"Intubation\"", "\"VentLung\"", "\"PVSat\"", "\"ArtCO2\"", "\"FiO2\"", "EOF" },
    { "\"KinkedTube\"", "\"VentTube\"", "\"Intubation\"", "\"MinVol\"", "\"ExpCO2\"", "\"VentAlv\"", "\"ArtCO2\"", "EOF" },
    { "\"Press\"", "\"Shunt\"", "\"MinVol\"", "\"VentAlv\"", "\"VentLung\"", "\"KinkedTube\"", "\"VentTube\"", "\"PulmEmbolus\"", "EOF" }

};

// our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node {

public:

	string Node_Name; // Variable name
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues; // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
    //vector<float> MCPT; // conditional probability table
	vector<vector<int> > permutationResult, mPermutationResult;
	vector<int> maxIndices, mMaxIndices;

    int sizeCPT;
    //int sizeMCPT;

	vector<string> mbProperties;
	vector<int> mbPropertyIndices;
	vector<string> mbValues;

	// Constructor- a node is initialised with its name and its categories
	Graph_Node(string name, int n, vector<string> vals) {
		Node_Name = name;
		nvalues = n;
		values = vals;
	}
	string get_name() {
		return Node_Name;
	}
	vector<int> get_children() {
		return Children;
	}
	vector<string> get_Parents() {
		return Parents;
	}
	vector<float> get_CPT() {
		return CPT;
	}
	int get_nvalues() {
		return nvalues;
	}
	vector<string> get_values() {
		return values;
	}
	void set_CPT(vector<float> new_CPT) {
		CPT.clear();
		CPT = new_CPT;
	}
    /*void set_MCPT(vector<float> new_MCPT) {
		MCPT.clear();
		MCPT = new_MCPT;
    }*/

	void set_Parents(vector<string> Parent_Nodes) {
		Parents.clear();
		Parents = Parent_Nodes;
	}

	// add another node in a graph as a child of this node
	int add_child(int new_child_index) {
		for (unsigned int i = 0; i < Children.size(); i++) {
			if (Children[i] == new_child_index)
				return 0;
		}
		Children.push_back(new_child_index);
		return 1;
	}

	void permuteStep(vector<int> maxIndicesLeft, int elemAddedAlready,
			vector<int> v, vector<vector<int> >* final) {
		if (maxIndicesLeft.size() == 0) {
			final->push_back(v);
			return;
		}
		for (int i = 0; i <= maxIndicesLeft[0]; i++) {
			v.erase(v.begin() + elemAddedAlready, v.end());
			v.push_back(i);
			vector<int> leftNow(maxIndicesLeft.begin() + 1,
					maxIndicesLeft.end());
			permuteStep(leftNow, elemAddedAlready + 1, v, final);
		}
	}

	//    vector of indices as a result of the permutation
	//    set of all such vectors as a set of all possible permutations in 000,001 ... 110,111 fashion
	vector<vector<int> > permute(vector<int> maxIndices) {

		//        to be returned
		vector<vector<int> > final;
		vector<int> v;

		permuteStep(maxIndices, 0, v, &final);
		return final;
	}

	void display();
	float retProbValue(vector<string> properties, vector<string> values);
	void markovBlanket();

	//    sets maxIndices and permutationResult
	void myInitialization();

	void findCPT(vector<float> countFrequencies) {
		int sizeCPT = CPT.size();
		float total;
		vector<float> newCPT(sizeCPT, -1);

		//        for each values of B in P(A|B)
		for (int i = 0; i < sizeCPT / nvalues; i++) {
			total = 0;
			//            for each values of A given B in P(A|B), find the total number of records
			for (int j = 0; j < nvalues; j++) {
				total += countFrequencies[i + j * sizeCPT / nvalues];
			}
			//            for each values of A given B in P(A|B), find P(A,B) / P(B)
			//            i.e. number of records having both A and B / number of records having B irrespective of {A-"?"}
			for (int j = 0; j < nvalues; j++) {
				newCPT[i + j * sizeCPT / nvalues] = (float) countFrequencies[i
						+ j * sizeCPT / nvalues] / total;
			}
		}

		set_CPT(newCPT);
	}

    /*void findMCPT(vector<float> countFrequencies) {

		float total;
		vector<float> newMCPT(sizeMCPT, -1);

		//        for each values of B in P(A|B)
		for (int i = 0; i < sizeMCPT / nvalues; i++) {
			total = 0;
			//            for each values of A given B in P(A|B), find the total number of records
			for (int j = 0; j < nvalues; j++) {
				total += countFrequencies[i + j * sizeMCPT / nvalues];
			}
			//            for each values of A given B in P(A|B), find P(A,B) / P(B)
			//            i.e. number of records having both A and B / number of records having B irrespective of {A-"?"}
			for (int j = 0; j < nvalues; j++) {
				newMCPT[i + j * sizeMCPT / nvalues]
						= (float) countFrequencies[i + j * sizeMCPT / nvalues]
								/ total;
			}
		}

		set_MCPT(newMCPT);
    }*/
};

// The whole network represted as a list of nodes
class network {

public:

	list<Graph_Node> Pres_Graph;

	int addNode(Graph_Node node) {
		Pres_Graph.push_back(node);
		return 0;
	}

	int netSize() {
		return Pres_Graph.size();
	}
	// get the index of node with a given name
	int get_index(string val_name) {
		list<Graph_Node>::iterator listIt;
		int count = 0;
		for (listIt = Pres_Graph.begin(); listIt != Pres_Graph.end(); listIt++) {
			if (listIt->get_name().compare(val_name) == 0)
				return count;
			count++;
		}
		return -1;
	}
	// get the node at nth index
	list<Graph_Node>::iterator get_nth_node(int n) {
		list<Graph_Node>::iterator listIt;
		int count = 0;
		for (listIt = Pres_Graph.begin(); listIt != Pres_Graph.end(); listIt++) {
			if (count == n)
				return listIt;
			count++;
		}
		return listIt;
	}
	//get the iterator of a node with a given name
	list<Graph_Node>::iterator search_node(string val_name) {
		list<Graph_Node>::iterator listIt;
		for (listIt = Pres_Graph.begin(); listIt != Pres_Graph.end(); listIt++) {
			if (listIt->get_name().compare(val_name) == 0)
				return listIt;
		}

		cout << "node not found\n";
		return listIt;
	}

} Alarm;

class patient {
	//    size of this list is the number of nodes in the network
public:
	float weight = 1;

	//    details of the unknown field of this patient
	int uFieldIndex;
	string uFieldName;

	//vector<string> data;
	map<string, string> data;

	patient() {
		weight = 1;
	}

	//    this patient is made from p with "?" replaced by s with weightage as probability
	patient(patient p, string s, float probability) {
		this->weight = probability;
		this->uFieldIndex = p.uFieldIndex;
		this->uFieldName = p.uFieldName;
		this->data = p.data;
        this->data[uFieldName] = s;
	}

	void view() {
		//        cout << "\nWeight is - " << setprecision(8) << weight;
		//        cout << "\nMissing property is - " << uFieldName;
		//        cout << "\nIts Value given is - " << data[uFieldIndex];
	}

};

vector<patient> data;

typedef list<Graph_Node>::iterator Graph_NodeIt;
typedef vector<patient>::iterator patientIt;

void set_MCPT_MAP() {
	for (unsigned int i = 0; i < data.size(); i++) {

		string key_MCPT_map = "";

		for (int j = 0; j < 72; j++) {
			for (int k = 0; k < 12; k++) {

				string curr_prop = mCPT_Keys[j][k];

				if (curr_prop.compare("EOF") == 0) {
					break;
				}

				map<string, string> dataMap = data[i].data;

				string curr_prop_value = dataMap.find(curr_prop)->second;

				key_MCPT_map += curr_prop + "=" + curr_prop_value + "|";

			}//End of all properties for a row in MCPT_keys

			cptMapIt cMapIt = MCPT_map.find(key_MCPT_map);

			if (cMapIt != MCPT_map.end()) {
				//value existing in the map
				MCPT_map[key_MCPT_map] = cMapIt->second + data[i].weight;

			} else {
				//value is not yet present in the map
				MCPT_map[key_MCPT_map] = data[i].weight;
			}

		}//End of row in MCPT_keys

	}//End of data[i]
}


void set_CPT_MAP() {
    for (unsigned int i = 0; i < data.size(); i++) {

        string key_CPT_map = "";

        for (int j = 0; j <60; j++) {
            for (int k = 0; k < 12; k++) {

                string curr_prop = CPT_Keys[j][k];

                if (curr_prop.compare("EOF") == 0) {
                    break;
                }

                map<string, string> dataMap = data[i].data;

                string curr_prop_value = dataMap.find(curr_prop)->second;

                key_CPT_map += curr_prop + "=" + curr_prop_value + "|";

            }//End of all properties for a row in MCPT_keys

            cptMapIt cMapIt = CPT_map.find(key_CPT_map);

            if (cMapIt != CPT_map.end()) {
                //value existing in the map
                CPT_map[key_CPT_map] = cMapIt->second + data[i].weight;

            } else {
                //value is not yet present in the map
                CPT_map[key_CPT_map] = data[i].weight;
            }

        }//End of row in CPT_keys

    }//End of data[i]
}


void Graph_Node::myInitialization() {

	maxIndices.push_back(nvalues - 1);
	for (unsigned int i = 0; i < Parents.size(); i++) {
		Graph_NodeIt gIt = Alarm.search_node(Parents[i]);
		maxIndices.push_back((*gIt).nvalues - 1);
	}

	permutationResult = permute(maxIndices);

	//    FIND MARKOV BLANKET
	//    markovBlanket() sets vector<string> mBProperties and vector< vector<string> > mBValues
	//    this is required only once
	markovBlanket();

	mMaxIndices.push_back(nvalues - 1);
	for_each( mbProperties.begin(), mbProperties.end(), [&]( string s) {
				Graph_NodeIt gIt = Alarm.search_node( s );
				mMaxIndices.push_back( (*gIt).nvalues-1 );
			});

	sizeMCPT = 1;
	for_each( mMaxIndices.begin(), mMaxIndices.end(), [&](int i) {
				sizeMCPT *= (i+1);
			});

	mPermutationResult = permute(mMaxIndices);

}

//    markovBlanket() sets vector<string> mBProperties
//    this is required only once
void Graph_Node::markovBlanket() {

	//    add my parents
for_each( Parents.begin(), Parents.end(), [&](string s) {
			mbProperties.push_back( s );
			mbPropertyIndices.push_back( Alarm.get_index( s ) );
		});

//    add my children
for_each( Children.begin(), Children.end(), [&](int s) {
			mbProperties.push_back( (*(Alarm.get_nth_node( s ))).Node_Name );
			mbPropertyIndices.push_back( s );
		});

//    add my spouses
for_each( Children.begin(), Children.end(), [&](int s) {
			Graph_NodeIt gIt = Alarm.get_nth_node( s );
			for_each( (*gIt).Parents.begin(), (*gIt).Parents.end(), [&](string spouse) {

						//            if this spouse has not already been added OR its not ME as a father of my own child, add it
						if( find( mbProperties.begin(), mbProperties.end(), spouse) == mbProperties.end() & spouse != Node_Name ) {
							mbProperties.push_back( spouse );
							mbPropertyIndices.push_back( Alarm.get_index( spouse ) );
						}

					});
		});

}

void Graph_Node::display() {
	cout << "\nNode name - " << Node_Name;
	cout << "\nValues that can be taken - ";
	for_each( values.begin(), values.end(), [](string s) {cout << s << " ";});
	cout << "\nMy parents -> ";
	for_each( Parents.begin(), Parents.end(), [](string p) {cout << p << " ";});
	cout << "\nMy children -> ";
	for_each( Children.begin(), Children.end(), [](int p) {cout << (*(Alarm.get_nth_node(p))).Node_Name << " ";});
	cout << "\nMy markov blanket -> ";
	printStringVector(mbProperties);
	cout << "\nMy CPT meanings -> \n";

	int CPTindex = 0;
	for_each( permutationResult.begin(), permutationResult.end(), [&](vector<int> v) {
				cout << "P(" << Node_Name << "=" << values[v[0]] << "|";
				for( intIt it = v.begin()+1; it != v.end(); it++) {
					Graph_NodeIt gIt = Alarm.search_node(Parents[it-v.begin()-1]);
					cout << Parents[it-v.begin()-1] << "=" << (*gIt).values[v[it-v.begin()]];
				}
				cout << ") = " << setprecision(6) << CPT[CPTindex++] << "\n";
			});

	int MCPTindex = 0;
for_each( mPermutationResult.begin(), mPermutationResult.end(), [&](vector<int> v) {
			cout << "P(" << Node_Name << "=" << values[v[0]] << "|";
			int index = 1;
			for_each( mbProperties.begin(), mbProperties.end(), [&](string s) {
						Graph_NodeIt gIt = Alarm.search_node( s );
						cout << s << "=" << gIt->values[ v[index] ];
						index++;
					});
			cout << ") = " << setprecision(6) << MCPT[MCPTindex++] << "\n";
		});

}

network read_network() {
	network Alarm;
	string line;
	int find = 0;
	ifstream myfile(networkFile);
	string temp;
	string name;
	vector<string> values;

	if (myfile.is_open()) {
		while (!myfile.eof()) {
			stringstream ss;
			getline(myfile, line);

			ss.str(line);
			ss >> temp;

			if (temp.compare("variable") == 0) {

				ss >> name;
				getline(myfile, line);

				stringstream ss2;
				ss2.str(line);
				for (int i = 0; i < 4; i++) {

					ss2 >> temp;

				}
				values.clear();
				while (temp.compare("};") != 0) {
					values.push_back(temp);

					ss2 >> temp;
				}
				Graph_Node new_node(name, values.size(), values);
				//int pos = Alarm.addNode(new_node);

			} else if (temp.compare("probability") == 0) {

				ss >> temp;
				ss >> temp;

				list<Graph_Node>::iterator listIt;
				list<Graph_Node>::iterator listIt1;
				listIt = Alarm.search_node(temp);
				int index = Alarm.get_index(temp);
				ss >> temp;
				values.clear();
				while (temp.compare(")") != 0) {
					listIt1 = Alarm.search_node(temp);
					listIt1->add_child(index);
					values.push_back(temp);

					ss >> temp;

				}
				listIt->set_Parents(values);
				getline(myfile, line);
				stringstream ss2;

				ss2.str(line);
				ss2 >> temp;

				ss2 >> temp;

				vector<float> curr_CPT;
				string::size_type sz;
				while (temp.compare(";") != 0) {
					curr_CPT.push_back(atof(temp.c_str()));

					ss2 >> temp;

				}

				listIt->set_CPT(curr_CPT);

			} else {

			}
		}

		if (find == 1)
			myfile.close();
	}

	return Alarm;
}

// reads the input data file and fills up the list named data
void read_data() {

	ifstream myfile(recordFile);
	string line;
	//int i = 0;
	while (!myfile.eof()) {
		getline(myfile, line);

		patient p;
		string lin = line;
		int y;

		for (int i = 0; i < Alarm.netSize(); i++) {
			y = lin.find(" ");
			if (lin.substr(0, y) == "\"?\"") {
				p.uFieldIndex = i;
				p.uFieldName = getNodeFromIndex( i ).Node_Name;
			}

			//p.data.push_back( lin.substr(0,y) );


			Graph_NodeIt it = Alarm.get_nth_node(i);
			string node_name = it->Node_Name;
			p.data[node_name] = lin.substr(0, y);

			lin = lin.substr(y + 1, lin.length() - y);
		}
		data.push_back(p);
	}
	dataSize = data.size();
	cout << "Data filled with data size - " << data.size() << endl;
}

//returns the number of patient records where given properties have given values
/*float countRecords(vector<string> properties, vector<string> values) {
 float x = 0;
 vector<int> propertyIndices;

 for_each( properties.begin(), properties.end(), [&](string s) {
 propertyIndices.push_back( Alarm.get_index( s ) );
 });

 //    count here for each patient
 for_each( data.begin(), data.end(), [&](patient p) {
 bool present = true;
 for( intIt iIt = propertyIndices.begin(); iIt != propertyIndices.end(); iIt++ ) {
 //            if the value of data present at this index is equal to the one given in the input
 if( p.data[*iIt] != values[iIt-propertyIndices.begin()] )
 present = false;
 }
 if( present )
 x+=p.weight;
 });

 //    laplace smoothing
 //    cout << "returning " << setprecision(5) << x+1 << endl;
 return x + 1;
 }*/

float Graph_Node::retProbValue(vector<string> properties, vector<string> values) {
	/*int impact = 1;
	 int index = values.size()-1;
	 int finalIndex = 0;
	 for( stringIt sIt = mbProperties.end()-1; sIt != mbProperties.begin()-1; sIt--, index-- ) {
	 Graph_NodeIt gIt = Alarm.search_node( *sIt );
	 finalIndex += findIndexString( gIt->values, values[index] ) * impact;
	 impact *= gIt->nvalues;
	 }
	 cin.ignore();
	 return MCPT[ finalIndex ];*/

	string key_includingMe = "", key_excludingMe = "";
	int count_includingMe = 1, count_excludingMe = 1;

	for (unsigned int i = 0; i < properties.size(); i++) {
		key_includingMe += properties[i] + "=" + values[i] + "|";

		if (i == 0)
			continue;

		key_excludingMe += properties[i] + "=" + values[i] + "|";
	}

	cptMapIt cMapIt_includingMe = MCPT_map.find(key_includingMe);
	if (cMapIt_includingMe != MCPT_map.end()) {
		count_includingMe += cMapIt_includingMe->second;
	}

	cptMapIt cMapIt_excludingMe = MCPT_map.find(key_excludingMe);
	if (cMapIt_excludingMe != MCPT_map.end()) {
		count_excludingMe += cMapIt_excludingMe->second;
	}

	return count_includingMe / count_excludingMe;
}

int main(int argc, char** argv) {
	if (argc != 3)
		exit(1);

	networkFile = argv[1];
	recordFile = argv[2];

	Alarm = read_network();

	read_data();

	/* for_each( Alarm.Pres_Graph.begin(), Alarm.Pres_Graph.end(), [](Graph_Node gn) {
	 cout << gn.Node_Name << " has values ";
	 for_each( gn.values.begin(), gn.values.end(), [](string s){cout << s << " ";});
	 cout << endl;
	 });*/

	//cout << "Net size is " << Alarm.netSize() << endl;

	int index = 0;

	for (Graph_NodeIt it = Alarm.Pres_Graph.begin(); it
			!= Alarm.Pres_Graph.end(); it++, index++) {

		cout << endl << (*it).Node_Name << " has index " << index
				<< " and has been initialized\n";
		(*it).myInitialization();

	}

	//    APPLY COUNTING AND IGNORE THE ? VALUES


	//=================================================M-Step===============================
    //set_MCPT_MAP();
    set_CPT_MAP();

	//=================================================E-Step===============================

	/*	if (data.size() == dataSize) { //E-step for the first time

	 for_each	( data.begin(), data.end(), [&](patient p)) {

	 //          missing data from the patient
	 Graph_NodeIt gIt = Alarm.get_nth_node( p.uFieldIndex );
	 (*gIt).mbValues.clear();

	 //            for each markov blanket's node
	 for( stringIt sIt = (*gIt).mbProperties.begin(); sIt != (*gIt).mbProperties.end(); sIt++ ) {

	 //                index of markov blanket's node
	 int index = Alarm.get_index( *sIt );

	 //                this patient has p.data[index] value of this markov blanket's node
	 //                hence add it to mbValues
	 (*gIt).mbValues.push_back( p.data[index] );

	 }

	 //            now, I have to calculate P(A|B) where A is the node with "?" and B is its markov blanket with the values given

	 //            total number of records with only the markov blanket specified
	 //            this is number of records with B only in P(A|B)
	 float total = 0;

	 //            if A can take values "Low" "Medium" "High", this vector contains three counts corresponding to these values
	 vector<float> countFrequencies;

	 //            for each possible values of A in P(A|B)
	 for_each( (*gIt).values.begin(), (*gIt).values.end(), [&](string s) {
	 vector<string> properties = (*gIt).properties;
	 vector<string> values = (*gIt).mbValues;

	 //                add A to calculate number of records with both A=s and B in P(A=s|B)
	 properties.insert( properties.begin(), (*gIt).Node_Name );)
	 values.insert( values.begin(), s );

	 float probability = (*gIt).retProbValue( properties, values);
	 patient *newP = new patient( p, s, probability);
	 newData.push_back( *newP );
	 newP->view();
	 delete newP;
	 //            cout << "value of total -> " << total << endl;
	 });

	 });
	 }*/

	/*

	 while( true ) {

	 vector<string> p,v;

	 index = 0;
	 //        CPT updated
	 for( Graph_NodeIt it = Alarm.Pres_Graph.begin(); it != Alarm.Pres_Graph.end(); it++, index++ ) {

	 //        if find the prior probability of this node who has no parent
	 //        else find the CPT of this node who has some parents
	 if( (*it).Parents.empty() ) {

	 //            index i of this list is incremented when corresponding value is found in the data
	 vector<float> countFrequencies((*it).nvalues,0);
	 vector<float> newCPT((*it).nvalues,0);

	 //            check the value stored in each patient record
	 for_each( data.begin(), data.end(), [&](patient p){
	 if( p.data[index] != "\"?\"" ) {
	 stringIt sIt = find( (*it).values.begin(), (*it).values.end(), p.data[index] );
	 countFrequencies[ sIt- (*it).values.begin() ]+=p.weight;
	 }
	 });

	 //            total will be number of patient records - ? corresponding to this patient entry
	 float total = sumFloatVector( countFrequencies );

	 //            assign the CPT values
	 for( floatIt fIt = newCPT.begin(); fIt != newCPT.end(); fIt++ ) {
	 (*fIt) = (float)countFrequencies[ fIt - newCPT.begin() ] / (float)total;
	 }

	 (*it).set_CPT(newCPT);
	 } else {

	 vector<string> properties, values;
	 vector<float> countFrequencies;
	 properties.push_back( (*it).Node_Name );
	 for_each( (*it).Parents.begin(), (*it).Parents.end(), [&](string s){properties.push_back( s );});

	 for_each( (*it).permutationResult.begin(), (*it).permutationResult.end(), [&](vector<int> v){
	 values.clear();
	 values.push_back( (*it).values[ v[0] ] );

	 for( intIt iIt = v.begin()+1; iIt != v.end(); iIt++ ) {
	 Graph_NodeIt gIt = Alarm.search_node( (*it).Parents[iIt-v.begin()-1] );
	 values.push_back( (*gIt).values[ v[iIt-v.begin()] ] );
	 }

	 //                the first element here is the A in P(A|B) and the rest of the list is B
	 //                we have to find P(A,B)/P(B)
	 //                P(A,B) = number of records with both A and B present / total number of records with both A and B non "?"
	 //                P(B) = number of records with B present / total number of records with both A and B non "?"
	 countFrequencies.push_back( countRecords(properties, values) );
	 });

	 //            now, I have to set the newCPT based on all the countFrequencies I got
	 (*it).findCPT( countFrequencies );

	 }

	 // fill in the MCPT based on the data we have right now
	 vector<string> mProperties, mValues;
	 vector<float> mCountFrequencies;
	 mProperties = it->mbProperties;
	 mProperties.insert( mProperties.begin(), it->Node_Name );

	 for_each( (*it).mPermutationResult.begin(), (*it).mPermutationResult.end(), [&](vector<int> v){
	 mValues.clear();

	 //adding currentNode's value
	 int currentNode_currentValue_index = v[0];
	 string currentNode_currentValue = (*it).values[ currentNode_currentValue_index ];
	 mValues.push_back( currentNode_currentValue );

	 int index = 0;
	 for( intIt iIt = v.begin()+1; iIt != v.end(); iIt++, index++ ) {

	 string mbCurrentNode = (*it).mbProperties[index];

	 Graph_NodeIt gIt = Alarm.search_node( mbCurrentNode );
	 int mbNode_currentValue_index = v[index+1];
	 string mbNode_currentValue = (*gIt).values[ mbNode_currentValue_index ];
	 mValues.push_back( mbNode_currentValue );
	 }

	 mCountFrequencies.push_back( countRecords(mProperties, mValues) );

	 });

	 (*it).findMCPT( mCountFrequencies );
	 (*it).display();
	 //        cin.ignore();
	 }

	 //        output the updated CPT
	 output();

	 cin.ignore();

	 //        this newData vector will be written to data after the E step
	 vector<patient> newData;
	 index = 0;

	 //        modify input patient data - E step - depending on the markov blanket of the missing value
	 //        for the first time, weight of each record is 1
	 //        if expand dataset
	 //        else change the probability only
	 if( data.size() == dataSize ) {
	 for_each( data.begin(), data.end(), [&](patient p){
	 cout << "\n\nPatient number -> " << ++index;
	 //            cout << "\nnewData.size() -> " << newData.size() << endl;

	 //            missing data from the patient
	 Graph_NodeIt gIt = Alarm.get_nth_node( p.uFieldIndex );
	 (*gIt).mbValues.clear();

	 //            for each markov blanket's node
	 for( stringIt sIt = (*gIt).mbProperties.begin(); sIt != (*gIt).mbProperties.end(); sIt++ ) {

	 //                index of markov blanket's node
	 int index = Alarm.get_index( *sIt );

	 //                this patient has p.data[index] value of this markov blanket's node
	 //                hence add it to mbValues
	 (*gIt).mbValues.push_back( p.data[index] );

	 }

	 //            now, I have to calculate P(A|B) where A is the node with "?" and B is its markov blanket with the values given

	 //            total number of records with only the markov blanket specified
	 //            this is number of records with B only in P(A|B)
	 float total = 0;

	 //            if A can take values "Low" "Medium" "High", this vector contains three counts corresponding to these values
	 vector<float> countFrequencies;

	 //            for each possible values of A in P(A|B)
	 for_each( (*gIt).values.begin(), (*gIt).values.end(), [&](string s){
	 vector<string> values = (*gIt).mbValues;

	 //                add A to calculate number of records with both A=s and B in P(A=s|B)
	 values.insert( values.begin(), s );

	 float probability = (*gIt).retProbValue(values);
	 patient *newP = new patient( p, s, probability);
	 newData.push_back( *newP );
	 newP->view();
	 delete newP;
	 //            cout << "value of total -> " << total << endl;
	 });

	 });
	 } else {


	 //        TO BE DONE LATER

	 for_each( data.begin(), data.end(), [&](patient p){
	 cout << "\n\nPatient number -> " << ++index;

	 //            missing data from the patient
	 Graph_NodeIt gIt = Alarm.get_nth_node( p.uFieldIndex );
	 (*gIt).mbValues.clear();
	 //            for each markov blanket's node
	 for( stringIt sIt = (*gIt).mbProperties.begin(); sIt != (*gIt).mbProperties.end(); sIt++ ) {

	 //                index of markov blanket's node
	 int index = Alarm.get_index( *sIt );

	 //                this patient has p.data[index] value of this markov blanket's node
	 //                hence add it to mbValues
	 (*gIt).mbValues.push_back( p.data[index] );

	 }

	 //            total number of records with only the markov blanket specified
	 //            this is number of records with B only in P(A|B)
	 float total = 0;

	 float countFrequencies = 0;

	 //            for each possible values of A in P(A|B)
	 for_each( (*gIt).values.begin(), (*gIt).values.end(), [&](string s){
	 vector<string> properties = (*gIt).mbProperties;
	 vector<string> values = (*gIt).mbValues;

	 //                add A to calculate number of records with both A=s and B in P(A=s|B)
	 properties.push_back( (*gIt).Node_Name );
	 values.push_back( s );

	 //                cout << "\nproperties and values contents ->\n";
	 //                printStringVector(properties);
	 //                printStringVector(values);
	 //                cout << "records with such param -> " << countRecords(properties, values);
	 //                cin.ignore();
	 countFrequencies.push_back( countRecords(properties, values) );
	 total += countFrequencies.back();
	 //            cout << "value of total -> " << total << endl;
	 });

	 //            now, I will find the probability P(A=s|B)
	 for( int i = 0; i < (*gIt).values.size(); i++ ){
	 float probability = countFrequencies[i] / total;
	 cout << "Old and new probabilities are -> " << p.weight << " " << probability;
	 p.weight = probability;
	 }
	 });
	 }

	 data.clear();
	 data = newData;

	 }
	 */
}

void output() {
	cout << "// Bayesian Network in the Interchange Format\n";
	cout << "// Produced by BayesianNetworks package in JavaBayes\n";
	cout << "// Output created Sun Nov 02 17:58:15 GMT+00:00 1997\n";
	cout << "// Bayesian network \n";
	cout
			<< "network \"Alarm\" { //37 variables and 37 probability distributions\n";
	cout << "}\n";

	int index = 0;
	for (Graph_NodeIt gIt = Alarm.Pres_Graph.begin(); gIt
			!= Alarm.Pres_Graph.end(); gIt++, index++) {
		string nodeName = (*gIt).Node_Name;
		int nvalues = (*gIt).nvalues;
		cout << "variable  " << nodeName << " { //" << nvalues << " values\n";
		cout << "\ttype discrete[" << nvalues << "] {";
		for_each( (*gIt).values.begin(), (*gIt).values.end(), [](string i) {cout << "  " << i;});
		cout << " };\n";

		cout << "\tproperty \"position = (" << position[index][0] << ", "
				<< position[index][1] << ")\" ;\n";
		cout << "}\n";
	}

	for (Graph_NodeIt gIt = Alarm.Pres_Graph.begin(); gIt
			!= Alarm.Pres_Graph.end(); gIt++, index++) {
		string nodeName = (*gIt).Node_Name;
		vector<string> parents = (*gIt).Parents;
		vector<float> CPT = (*gIt).CPT;

		cout << "probability (  " << nodeName;
		for_each( parents.begin(), parents.end(), [](string i) {cout << "  " << i;});
		cout << " ) { //" << 1 + parents.size() << " variable(s) and "
				<< CPT.size() << " values\n";
		cout << "\ttable ";
		printFloatVector(CPT);
		cout << ";\n}\n";
	}
}
