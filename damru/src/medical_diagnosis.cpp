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
#define FULLY_KNOWN "KNOWN"
#define OUTPUT_FILE "solved_alarm.bif"

#define BUFFER_SECONDS 120 //time to calculate & write CPT at the end
#define TOTAL_TIME 600

#define DELTA_CHANGE 0.01

#define IS_DEBUG_MODE true

// format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

string networkFile, recordFile;

unsigned int initialDataSize = 0;

map<string, float> CPT_map;
map<string, float> MCPT_map;

void output();

typedef vector<float>::iterator floatIt;
typedef vector<int>::iterator intIt;
typedef vector<string>::iterator stringIt;

typedef map<string, float>::iterator cptMapIt;

void printIntVector(vector<int> v) {
	//for_each( v.begin(), v.end(), [](int i) {cout << i << " ";});
	for (unsigned int i = 0; i < v.size(); i++)
		cout << v[i] << " ";
	cout << endl;
}
void printFloatVector(vector<float> v) {

	//for_each( v.begin(), v.end(), [](float i) {
	for (unsigned int i = 0; i < v.size(); i++) {
		if (v[i] > 0.99)
			cout << "0.99 ";
		else if (v[i] > 0.1)
			cout << setprecision(2) << v[i] << " ";
		else if (v[i] > 0.01)
			cout << setprecision(1) << v[i] << " ";
		else
			cout << "0.01 ";
	}

}
void printStringVector(vector<string> v) {
	//for_each( v.begin(), v.end(), [](string i) {cout << i << " ";});
	for (unsigned int i = 0; i < v.size(); i++) {
		cout << v[i] << " ";
	}
	cout << endl;
}
int sumIntVector(vector<int> v) {
	int x = 0;
	//for_each( v.begin(), v.end(), [&](int v) {x+=v;});
	for (unsigned int i = 0; i < v.size(); i++) {
		x += v[i];
	}
	return x;
}
float sumFloatVector(vector<float> v) {
	float x = 0;
	//for_each( v.begin(), v.end(), [&](float v) {x+=v;});
	for (unsigned int i = 0; i < v.size(); i++) {
		x += v[i];
	}
	return x;
}

int findIndexString(vector<string> vs, string s) {
	stringIt sIt = find(vs.begin(), vs.end(), s);
	return sIt - vs.begin();
}

//to be written to the output file
int position[37][2] = { 54, 35, 184, 113, 145, 36, 68, 114, 111, 177, 32, 179, 238, 61, 564, 38, 640, 86, 738, 86, 682, 168, 564, 172, 722, 253, 226,
		237, 229, 305, 366, 278, 289, 305, 220, 396, 154, 396, 195, 176, 308, 171, 120, 301, 31, 239, 329, 37, 1045, 292, 969, 258, 1014, 162, 329,
		107, 926, 387, 894, 293, 949, 197, 754, 387, 530, 393, 474, 277, 881, 165, 706, 344, 843, 86 };

void set_MCPT_MAP();
void set_CPT_MAP();

string CPT_Keys[][2][12] = {

{ { "EOF" }, { "\"Hypovolemia\"", "EOF" } },

{ { "EOF" }, { "\"StrokeVolume\"", "\"LVFailure\"", "\"Hypovolemia\"", "EOF" } },

{ { "EOF" }, { "\"LVFailure\"", "EOF" } },

{ { "EOF" }, { "\"LVEDVolume\"", "\"Hypovolemia\"", "\"LVFailure\"", "EOF" } },

{ { "EOF" }, { "\"PCWP\"", "\"LVEDVolume\"", "EOF" } },

{ { "EOF" }, { "\"CVP\"", "\"LVEDVolume\"", "EOF" } },

{ { "EOF" }, { "\"History\"", "\"LVFailure\"", "EOF" } },

{ { "EOF" }, { "\"MinVolSet\"", "EOF" } },

{ { "EOF" }, { "\"VentMach\"", "\"MinVolSet\"", "EOF" } },

{ { "EOF" }, { "\"Disconnect\"", "EOF" } },

{ { "EOF" }, { "\"VentTube\"", "\"VentMach\"", "\"Disconnect\"", "EOF" } },

{ { "EOF" }, { "\"KinkedTube\"", "EOF" } },

{ { "EOF" }, { "\"Press\"", "\"KinkedTube\"", "\"Intubation\"", "\"VentTube\"", "EOF" } },

{ { "EOF" }, { "\"ErrLowOutput\"", "EOF" } },

{ { "EOF" }, { "\"HRBP\"", "\"ErrLowOutput\"", "\"HR\"", "EOF" } },

{ { "EOF" }, { "\"ErrCauter\"", "EOF" } },

{ { "EOF" }, { "\"HREKG\"", "\"HR\"", "\"ErrCauter\"", "EOF" } },

{ { "EOF" }, { "\"HRSat\"", "\"HR\"", "\"ErrCauter\"", "EOF" } },

{ { "EOF" }, { "\"BP\"", "\"CO\"", "\"TPR\"", "EOF" } },

{ { "EOF" }, { "\"CO\"", "\"HR\"", "\"StrokeVolume\"", "EOF" } },

{ { "EOF" }, { "\"HR\"", "\"Catechol\"", "EOF" } },

{ { "EOF" }, { "\"TPR\"", "\"Anaphylaxis\"", "EOF" } },

{ { "EOF" }, { "\"Anaphylaxis\"", "EOF" } },

{ { "EOF" }, { "\"InsuffAnesth\"", "EOF" } },

{ { "EOF" }, { "\"PAP\"", "\"PulmEmbolus\"", "EOF" } },

{ { "EOF" }, { "\"PulmEmbolus\"", "EOF" } },

{ { "EOF" }, { "\"FiO2\"", "EOF" } },

{ { "EOF" }, { "\"Catechol\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"", "EOF" } },

{ { "EOF" }, { "\"SaO2\"", "\"Shunt\"", "\"PVSat\"", "EOF" } },

{ { "EOF" }, { "\"Shunt\"", "\"PulmEmbolus\"", "\"Intubation\"", "EOF" } },

{ { "EOF" }, { "\"PVSat\"", "\"VentAlv\"", "\"FiO2\"", "EOF" } },

{ { "EOF" }, { "\"MinVol\"", "\"VentLung\"", "\"Intubation\"", "EOF" } },

{ { "EOF" }, { "\"ExpCO2\"", "\"ArtCO2\"", "\"VentLung\"", "EOF" } },

{ { "EOF" }, { "\"ArtCO2\"", "\"VentAlv\"", "EOF" } },

{ { "EOF" }, { "\"VentAlv\"", "\"Intubation\"", "\"VentLung\"", "EOF" } },

{ { "EOF" }, { "\"VentLung\"", "\"KinkedTube\"", "\"VentTube\"", "\"Intubation\"", "EOF" } },

{ { "EOF" }, { "\"Intubation\"", "EOF" } },

//----below ones are excluding of aboves

		{ { "\"Hypovolemia\"" }, { "EOF" } },

		{ { "\"StrokeVolume\"" }, { "\"LVFailure\"", "\"Hypovolemia\"", "EOF" } },

		{ { "\"LVFailure\"" }, { "EOF" } },

		{ { "\"LVEDVolume\"" }, { "\"Hypovolemia\"", "\"LVFailure\"", "EOF" } },

		{ { "\"PCWP\"" }, { "\"LVEDVolume\"", "EOF" } },

		{ { "\"CVP\"" }, { "\"LVEDVolume\"", "EOF" } },

		{ { "\"History\"" }, { "\"LVFailure\"", "EOF" } },

		{ { "\"MinVolSet\"" }, { "EOF" } },

		{ { "\"VentMach\"" }, { "\"MinVolSet\"", "EOF" } },

		{ { "\"Disconnect\"" }, { "EOF" } },

		{ { "\"VentTube\"" }, { "\"VentMach\"", "\"Disconnect\"", "EOF" } },

		{ { "\"KinkedTube\"" }, { "EOF" } },

		{ { "\"Press\"" }, { "\"KinkedTube\"", "\"Intubation\"", "\"VentTube\"", "EOF" } },

		{ { "\"ErrLowOutput\"" }, { "EOF" } },

		{ { "\"HRBP\"" }, { "\"ErrLowOutput\"", "\"HR\"", "EOF" } },

		{ { "\"ErrCauter\"" }, { "EOF" } },

		{ { "\"HREKG\"" }, { "\"HR\"", "\"ErrCauter\"", "EOF" } },

		{ { "\"HRSat\"" }, { "\"HR\"", "\"ErrCauter\"", "EOF" } },

		{ { "\"BP\"" }, { "\"CO\"", "\"TPR\"", "EOF" } },

		{ { "\"CO\"" }, { "\"HR\"", "\"StrokeVolume\"", "EOF" } },

		{ { "\"HR\"" }, { "\"Catechol\"", "EOF" } },

		{ { "\"TPR\"" }, { "\"Anaphylaxis\"", "EOF" } },

		{ { "\"Anaphylaxis\"" }, { "EOF" } },

		{ { "\"InsuffAnesth\"" }, { "EOF" } },

		{ { "\"PAP\"" }, { "\"PulmEmbolus\"", "EOF" } },

		{ { "\"PulmEmbolus\"" }, { "EOF" } },

		{ { "\"FiO2\"" }, { "EOF" } },

		{ { "\"Catechol\"" }, { "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"", "EOF" } },

		{ { "\"SaO2\"" }, { "\"Shunt\"", "\"PVSat\"", "EOF" } },

		{ { "\"Shunt\"" }, { "\"PulmEmbolus\"", "\"Intubation\"", "EOF" } },

		{ { "\"PVSat\"" }, { "\"VentAlv\"", "\"FiO2\"", "EOF" } },

		{ { "\"MinVol\"" }, { "\"VentLung\"", "\"Intubation\"", "EOF" } },

		{ { "\"ExpCO2\"" }, { "\"ArtCO2\"", "\"VentLung\"", "EOF" } },

		{ { "\"ArtCO2\"" }, { "\"VentAlv\"", "EOF" } },

		{ { "\"VentAlv\"" }, { "\"Intubation\"", "\"VentLung\"", "EOF" } },

		{ { "\"VentLung\"" }, { "\"KinkedTube\"", "\"VentTube\"", "\"Intubation\"", "EOF" } },

		{ { "\"Intubation\"" }, { "EOF" } }

};

string
		mCPT_Keys[][2][12] =
				{

				{ { "EOF" }, { "\"Hypovolemia\"", "\"StrokeVolume\"", "\"LVEDVolume\"", "\"LVFailure\"", "EOF" } },

				{ { "EOF" }, { "\"StrokeVolume\"", "\"LVFailure\"", "\"Hypovolemia\"", "\"CO\"", "\"HR\"", "EOF" } },

				{ { "EOF" }, { "\"LVFailure\"", "\"StrokeVolume\"", "\"LVEDVolume\"", "\"History\"", "\"Hypovolemia\"", "EOF" } },

				{ { "EOF" }, { "\"LVEDVolume\"", "\"Hypovolemia\"", "\"LVFailure\"", "\"PCWP\"", "\"CVP\"", "EOF" } },

				{ { "EOF" }, { "\"PCWP\"", "\"LVEDVolume\"", "EOF" } },

				{ { "EOF" }, { "\"CVP\"", "\"LVEDVolume\"", "EOF" } },

				{ { "EOF" }, { "\"History\"", "\"LVFailure\"", "EOF" } },

				{ { "EOF" }, { "\"MinVolSet\"", "\"VentMach\"", "EOF" } },

				{ { "EOF" }, { "\"VentMach\"", "\"MinVolSet\"", "\"VentTube\"", "\"Disconnect\"", "EOF" } },

				{ { "EOF" }, { "\"Disconnect\"", "\"VentTube\"", "\"VentMach\"", "EOF" } },

				{ { "EOF" }, { "\"VentTube\"", "\"VentMach\"", "\"Disconnect\"", "\"Press\"", "\"VentLung\"", "\"KinkedTube\"", "\"Intubation\"",
						"EOF" } },

				{ { "EOF" }, { "\"KinkedTube\"", "\"Press\"", "\"VentLung\"", "\"Intubation\"", "\"VentTube\"", "EOF" } },

				{ { "EOF" }, { "\"Press\"", "\"KinkedTube\"", "\"Intubation\"", "\"VentTube\"", "EOF" } },

				{ { "EOF" }, { "\"ErrLowOutput\"", "\"HRBP\"", "\"HR\"", "EOF" } },

				{ { "EOF" }, { "\"HRBP\"", "\"ErrLowOutput\"", "\"HR\"", "EOF" } },

				{ { "EOF" }, { "\"ErrCauter\"", "\"HREKG\"", "\"HRSat\"", "\"HR\"", "EOF" } },

				{ { "EOF" }, { "\"HREKG\"", "\"HR\"", "\"ErrCauter\"", "EOF" } },

				{ { "EOF" }, { "\"HRSat\"", "\"HR\"", "\"ErrCauter\"", "EOF" } },

				{ { "EOF" }, { "\"BP\"", "\"CO\"", "\"TPR\"", "EOF" } },

				{ { "EOF" }, { "\"CO\"", "\"HR\"", "\"StrokeVolume\"", "\"BP\"", "\"TPR\"", "EOF" } },

				{ { "EOF" }, { "\"HR\"", "\"Catechol\"", "\"HRBP\"", "\"HREKG\"", "\"HRSat\"", "\"CO\"", "\"ErrLowOutput\"", "\"ErrCauter\"",
						"\"StrokeVolume\"", "EOF" } },

				{ { "EOF" },
						{ "\"TPR\"", "\"Anaphylaxis\"", "\"BP\"", "\"Catechol\"", "\"CO\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"ArtCO2\"", "EOF" } },

				{ { "EOF" }, { "\"Anaphylaxis\"", "\"TPR\"", "EOF" } },

				{ { "EOF" }, { "\"InsuffAnesth\"", "\"Catechol\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"", "EOF" } },

				{ { "EOF" }, { "\"PAP\"", "\"PulmEmbolus\"", "EOF" } },

				{ { "EOF" }, { "\"PulmEmbolus\"", "\"PAP\"", "\"Shunt\"", "\"Intubation\"", "EOF" } },

				{ { "EOF" }, { "\"FiO2\"", "\"PVSat\"", "\"VentAlv\"", "EOF" } },

				{ { "EOF" }, { "\"Catechol\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"", "\"HR\"", "EOF" } },

				{ { "EOF" }, { "\"SaO2\"", "\"Shunt\"", "\"PVSat\"", "\"Catechol\"", "\"InsuffAnesth\"", "\"TPR\"", "\"ArtCO2\"", "EOF" } },

				{ { "EOF" }, { "\"Shunt\"", "\"PulmEmbolus\"", "\"Intubation\"", "\"SaO2\"", "\"PVSat\"", "EOF" } },

				{ { "EOF" }, { "\"PVSat\"", "\"VentAlv\"", "\"FiO2\"", "\"SaO2\"", "\"Shunt\"", "EOF" } },

				{ { "EOF" }, { "\"MinVol\"", "\"VentLung\"", "\"Intubation\"", "EOF" } },

				{ { "EOF" }, { "\"ExpCO2\"", "\"ArtCO2\"", "\"VentLung\"", "EOF" } },

				{ { "EOF" }, { "\"ArtCO2\"", "\"VentAlv\"", "\"Catechol\"", "\"ExpCO2\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"VentLung\"",
						"EOF" } },

				{ { "EOF" }, { "\"VentAlv\"", "\"Intubation\"", "\"VentLung\"", "\"PVSat\"", "\"ArtCO2\"", "\"FiO2\"", "EOF" } },

				{ { "EOF" }, { "\"VentLung\"", "\"KinkedTube\"", "\"VentTube\"", "\"Intubation\"", "\"MinVol\"", "\"ExpCO2\"", "\"VentAlv\"",
						"\"ArtCO2\"", "EOF" } },

				{ { "EOF" }, { "\"Intubation\"", "\"Press\"", "\"Shunt\"", "\"MinVol\"", "\"VentAlv\"", "\"VentLung\"", "\"KinkedTube\"",
						"\"VentTube\"", "\"PulmEmbolus\"", "EOF" } },

				//below ones are denominator of above

						{ { "\"Hypovolemia\"" }, { "\"StrokeVolume\"", "\"LVEDVolume\"", "\"LVFailure\"", "EOF" } },

						{ { "\"StrokeVolume\"" }, { "\"LVFailure\"", "\"Hypovolemia\"", "\"CO\"", "\"HR\"", "EOF" } },

						{ { "\"LVFailure\"" }, { "\"StrokeVolume\"", "\"LVEDVolume\"", "\"History\"", "\"Hypovolemia\"", "EOF" } },

						{ { "\"LVEDVolume\"" }, { "\"Hypovolemia\"", "\"LVFailure\"", "\"PCWP\"", "\"CVP\"", "EOF" } },

						{ { "\"PCWP\"" }, { "\"LVEDVolume\"", "EOF" } },

						{ { "\"CVP\"" }, { "\"LVEDVolume\"", "EOF" } },

						{ { "\"History\"" }, { "\"LVFailure\"", "EOF" } },

						{ { "\"MinVolSet\"" }, { "\"VentMach\"", "EOF" } },

						{ { "\"VentMach\"" }, { "\"MinVolSet\"", "\"VentTube\"", "\"Disconnect\"", "EOF" } },

						{ { "\"Disconnect\"" }, { "\"VentTube\"", "\"VentMach\"", "EOF" } },

						{ { "\"VentTube\"" }, { "\"VentMach\"", "\"Disconnect\"", "\"Press\"", "\"VentLung\"", "\"KinkedTube\"", "\"Intubation\"",
								"EOF" } },

						{ { "\"KinkedTube\"" }, { "\"Press\"", "\"VentLung\"", "\"Intubation\"", "\"VentTube\"", "EOF" } },

						{ { "\"Press\"" }, { "\"KinkedTube\"", "\"Intubation\"", "\"VentTube\"", "EOF" } },

						{ { "\"ErrLowOutput\"" }, { "\"HRBP\"", "\"HR\"", "EOF" } },

						{ { "\"HRBP\"" }, { "\"ErrLowOutput\"", "\"HR\"", "EOF" } },

						{ { "\"ErrCauter\"" }, { "\"HREKG\"", "\"HRSat\"", "\"HR\"", "EOF" } },

						{ { "\"HREKG\"" }, { "\"HR\"", "\"ErrCauter\"", "EOF" } },

						{ { "\"HRSat\"" }, { "\"HR\"", "\"ErrCauter\"", "EOF" } },

						{ { "\"BP\"" }, { "\"CO\"", "\"TPR\"", "EOF" } },

						{ { "\"CO\"", }, { "\"HR\"", "\"StrokeVolume\"", "\"BP\"", "\"TPR\"", "EOF" } },

						{ { "\"HR\"" }, { "\"Catechol\"", "\"HRBP\"", "\"HREKG\"", "\"HRSat\"", "\"CO\"", "\"ErrLowOutput\"", "\"ErrCauter\"",
								"\"StrokeVolume\"", "EOF" } },

						{ { "\"TPR\"" },
								{ "\"Anaphylaxis\"", "\"BP\"", "\"Catechol\"", "\"CO\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"ArtCO2\"", "EOF" } },

						{ { "\"Anaphylaxis\"" }, { "\"TPR\"", "EOF" } },

						{ { "\"InsuffAnesth\"" }, { "\"Catechol\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"", "EOF" } },

						{ { "\"PAP\"" }, { "\"PulmEmbolus\"", "EOF" } },

						{ { "\"PulmEmbolus\"" }, { "\"PAP\"", "\"Shunt\"", "\"Intubation\"", "EOF" } },

						{ { "\"FiO2\"" }, { "\"PVSat\"", "\"VentAlv\"", "EOF" } },

						{ { "\"Catechol\"" }, { "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"ArtCO2\"", "\"HR\"", "EOF" } },

						{ { "\"SaO2\"" }, { "\"Shunt\"", "\"PVSat\"", "\"Catechol\"", "\"InsuffAnesth\"", "\"TPR\"", "\"ArtCO2\"", "EOF" } },

						{ { "\"Shunt\"" }, { "\"PulmEmbolus\"", "\"Intubation\"", "\"SaO2\"", "\"PVSat\"", "EOF" } },

						{ { "\"PVSat\"" }, { "\"VentAlv\"", "\"FiO2\"", "\"SaO2\"", "\"Shunt\"", "EOF" } },

						{ { "\"MinVol\"" }, { "\"VentLung\"", "\"Intubation\"", "EOF" } },

						{ { "\"ExpCO2\"" }, { "\"ArtCO2\"", "\"VentLung\"", "EOF" } },

						{ { "\"ArtCO2\"" }, { "\"VentAlv\"", "\"Catechol\"", "\"ExpCO2\"", "\"InsuffAnesth\"", "\"SaO2\"", "\"TPR\"", "\"VentLung\"",
								"EOF" } },

						{ { "\"VentAlv\"" }, { "\"Intubation\"", "\"VentLung\"", "\"PVSat\"", "\"ArtCO2\"", "\"FiO2\"", "EOF" } },

						{ { "\"VentLung\"" }, { "\"KinkedTube\"", "\"VentTube\"", "\"Intubation\"", "\"MinVol\"", "\"ExpCO2\"", "\"VentAlv\"",
								"\"ArtCO2\"", "EOF" } },

						{ { "\"Intubation\"" }, { "\"Press\"", "\"Shunt\"", "\"MinVol\"", "\"VentAlv\"", "\"VentLung\"", "\"KinkedTube\"",
								"\"VentTube\"", "\"PulmEmbolus\"", "EOF" } },

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
	int sizeMCPT;

	vector<string> mbProperties;
	vector<int> mbPropertyIndices;
	vector<string> mbValues;

	// Constructor- a node is initialized with its name and its categories
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

	void permuteStep(vector<int> maxIndicesLeft, int elemAddedAlready, vector<int> v, vector<vector<int> >* final) {
		if (maxIndicesLeft.size() == 0) {
			final->push_back(v);
			return;
		}
		for (int i = 0; i <= maxIndicesLeft[0]; i++) {
			v.erase(v.begin() + elemAddedAlready, v.end());
			v.push_back(i);
			vector<int> leftNow(maxIndicesLeft.begin() + 1, maxIndicesLeft.end());
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

	void findCPT();

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

class Patient {
	//    size of this list is the number of nodes in the network
public:
	float weight = 1;

	//    details of the unknown field of this patient
	//	initially, we assume the patient is fully known until an "?" is encountered
	string uFieldName = FULLY_KNOWN;

	//vector<string> data;
	map<string, string> data;

	Patient() {
		weight = 1;
	}

	//    this patient is made from p with "?" replaced by s with weightage as probability
	Patient(Patient p, string s, float probability) {
		this->weight = probability;
		this->uFieldName = p.uFieldName;
		this->data = p.data;
		this->data[uFieldName] = s;
	}

	void view() {
		cout << "\nWeight is - " << setprecision(8) << weight;
		cout << "\nMissing property is - " << uFieldName;
		cout << "\nIts Value given is - " << data[uFieldName] << endl;
	}

};

vector<Patient> data;

typedef list<Graph_Node>::iterator Graph_NodeIt;
typedef vector<Patient>::iterator patientIt;

void Graph_Node::findCPT() {

	vector<float> newCPT;
	//	cout << "Size of CPT -> " << sizeCPT << endl;
	//	cout << "Node name " << Node_Name << endl;

	for (unsigned int i = 0; i < permutationResult.size(); i++) {
		vector<int> currentIndices = permutationResult[i];
		//		printIntVector( currentIndices );

		int myValueIndex = currentIndices[0];
		string myValueName = this->values[myValueIndex];

		string cptMapKey_includingMe = this->Node_Name + "=" + myValueName + "|";
		string cptMapKey_excludingMe = this->Node_Name + "!=" + "?" + "|";

		for (unsigned int j = 1; j < currentIndices.size(); j++) {
			string currentParent_name = this->Parents[j - 1];
			int currentParent_valueIndex = currentIndices[j];

			Graph_NodeIt gIt = Alarm.search_node(currentParent_name);
			string currentParent_value = gIt->values[currentParent_valueIndex];

			string temp = currentParent_name + "=" + currentParent_value + "|";

			cptMapKey_includingMe += temp;
			cptMapKey_excludingMe += temp;

		}

		//------------------finding counts
		float count_includingMe = 0;
		float count_excludingMe = 0;

		cptMapIt cpt_it_includingMe = CPT_map.find(cptMapKey_includingMe);
		cptMapIt cpt_it_excludingMe = CPT_map.find(cptMapKey_excludingMe);

		if (cpt_it_includingMe != CPT_map.end()) {
			//key found in map
			count_includingMe = cpt_it_includingMe->second;
		}

		if (cpt_it_excludingMe != CPT_map.end()) {
			//key found in map
			count_excludingMe = cpt_it_excludingMe->second;
		}

		//-----------getting probability
		float probability;

		/*		//the node has no parent...i.e. excluding_me is empty
		 if (currentIndices.size() == 1) {
		 probability = count_includingMe / initialDataSize;
		 newCPT.push_back(probability);
		 continue;
		 }*/

		//no exclusingMe...obviously no includingMe as well
		if (count_excludingMe == 0) {
			probability = 1.0 / nvalues;
			newCPT.push_back(probability);
			continue;
		}

		//normal case (with parent)
		probability = count_includingMe / count_excludingMe;
		newCPT.push_back(probability);
	}

	this->set_CPT(newCPT);
}

void set_MCPT_MAP() {
	for (unsigned int i = 0; i < data.size(); i++) {

		string key_MCPT_map = "";

		for (int j = 0; j < 74; j++) {

			key_MCPT_map = "";

			if (mCPT_Keys[j][0][0].compare("EOF") == 0) { //i.e. this is the case for including_me key

				for (int k = 0; k < 12; k++) {

					string curr_prop = mCPT_Keys[j][1][k];

					if (curr_prop.compare("EOF") == 0) {
						break;
					}

					map<string, string> dataMap = data[i].data;

					string curr_prop_value = dataMap.find(curr_prop)->second;

					key_MCPT_map += curr_prop + "=" + curr_prop_value + "|";

				}//End of all properties for a row in MMCPT_keys

				cptMapIt cMapIt = MCPT_map.find(key_MCPT_map);

				if (cMapIt != MCPT_map.end()) {
					//value existing in the map
					//cout << "existing value: "; //TODO : cout
					//cout << key_MCPT_map << " : " << (cMapIt->second + data[i].weight) <<endl; //TODO : cout
					MCPT_map[key_MCPT_map] = cMapIt->second + data[i].weight;

				} else {
					//value is not yet present in the map
					//cout << "not existing: "; //TODO : cout
					//cout << key_MCPT_map << " : " << (data[i].weight) <<endl; //TODO : cout
					MCPT_map[key_MCPT_map] = data[i].weight;
				}

			}

			else {//case for excluding_me keys
				map<string, string> dataMap = data[i].data;

				string headProperty = mCPT_Keys[j][0][0];

				if (dataMap.find(headProperty)->second.compare("\"?\"") == 0) {
					continue;
				}

				key_MCPT_map += headProperty + "!=" + "?" + "|";

				for (int k = 0; k < 12; k++) {
					string curr_prop = mCPT_Keys[j][1][k];

					if (curr_prop.compare("EOF") == 0) {
						break;
					}

					string curr_prop_value = dataMap.find(curr_prop)->second;

					key_MCPT_map += curr_prop + "=" + curr_prop_value + "|";

				}//End of all properties for a row in MMCPT_keys

				cptMapIt cMapIt = MCPT_map.find(key_MCPT_map);

				if (cMapIt != MCPT_map.end()) {
					//value existing in the map
					//cout << "existing value: "; //TODO : cout
					//cout << key_MCPT_map << " : " << (cMapIt->second + data[i].weight) <<endl; //TODO : cout
					MCPT_map[key_MCPT_map] = cMapIt->second + data[i].weight;

				} else {
					//value is not yet present in the map
					//cout << "not existing: "; //TODO : cout
					//cout << key_MCPT_map << " : " << (data[i].weight) <<endl; //TODO : cout
					MCPT_map[key_MCPT_map] = data[i].weight;
				}

			}

		}

	}
}

void display_CPT_MAP() {
	for (cptMapIt it = CPT_map.begin(); it != CPT_map.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';
}

void display_MCPT_MAP() {
	cout << "----------------Displaying MCPT_MAP------------" << endl;
	for (cptMapIt it = MCPT_map.begin(); it != MCPT_map.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';
}

void set_CPT_MAP() {
	for (unsigned int i = 0; i < data.size(); i++) {

		string key_CPT_map = "";

		for (int j = 0; j < 74; j++) {

			key_CPT_map = "";

			if (CPT_Keys[j][0][0].compare("EOF") == 0) { //i.e. this is the case for including_me key

				for (int k = 0; k < 12; k++) {

					string curr_prop = CPT_Keys[j][1][k];

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
					//cout << "existing value: "; //TODO : cout
					//cout << key_CPT_map << " : " << (cMapIt->second + data[i].weight) <<endl; //TODO : cout
					CPT_map[key_CPT_map] = cMapIt->second + data[i].weight;

				} else {
					//value is not yet present in the map
					//cout << "not existing: "; //TODO : cout
					//cout << key_CPT_map << " : " << (data[i].weight) <<endl; //TODO : cout
					CPT_map[key_CPT_map] = data[i].weight;
				}

			}

			else {//case for excluding_me keys
				map<string, string> dataMap = data[i].data;

				string headProperty = CPT_Keys[j][0][0];

				if (dataMap.find(headProperty)->second.compare("\"?\"") == 0) {
					continue;
				}

				key_CPT_map += headProperty + "!=" + "?" + "|";

				for (int k = 0; k < 12; k++) {
					string curr_prop = CPT_Keys[j][1][k];

					if (curr_prop.compare("EOF") == 0) {
						break;
					}

					string curr_prop_value = dataMap.find(curr_prop)->second;

					key_CPT_map += curr_prop + "=" + curr_prop_value + "|";

				}//End of all properties for a row in MCPT_keys

				cptMapIt cMapIt = CPT_map.find(key_CPT_map);

				if (cMapIt != CPT_map.end()) {
					//value existing in the map
					//cout << "existing value: "; //TODO : cout
					//cout << key_CPT_map << " : " << (cMapIt->second + data[i].weight) <<endl; //TODO : cout
					CPT_map[key_CPT_map] = cMapIt->second + data[i].weight;

				} else {
					//value is not yet present in the map
					//cout << "not existing: "; //TODO : cout
					//cout << key_CPT_map << " : " << (data[i].weight) <<endl; //TODO : cout
					CPT_map[key_CPT_map] = data[i].weight;
				}

			}

		}

	}

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

	for (unsigned int i = 0; i < mbProperties.size(); i++) {
		Graph_NodeIt gIt = Alarm.search_node(mbProperties[i]);
		mMaxIndices.push_back((*gIt).nvalues - 1);
	}

	sizeMCPT = 1;

	for (unsigned int i = 0; i < mMaxIndices.size(); i++) {
		sizeMCPT *= (mMaxIndices[i] + 1);
	}

	mPermutationResult = permute(mMaxIndices);

}

//    markovBlanket() sets vector<string> mBProperties
//    this is required only once
void Graph_Node::markovBlanket() {

	//    add my parents
	/*
	 for_each( Parents.begin(), Parents.end(), [&](string s) {
	 mbProperties.push_back( s );
	 mbPropertyIndices.push_back( Alarm.get_index( s ) );
	 });
	 */

	for (unsigned int i = 0; i < Parents.size(); i++) {
		mbProperties.push_back(Parents[i]);
		mbPropertyIndices.push_back(Alarm.get_index(Parents[i]));
	}

	//    add my children
	/*
	 for_each( Children.begin(), Children.end(), [&](int s) {
	 mbProperties.push_back( (*(Alarm.get_nth_node( s ))).Node_Name );
	 mbPropertyIndices.push_back( s );
	 });
	 */
	for (unsigned int i = 0; i < Children.size(); i++) {
		mbProperties.push_back((*(Alarm.get_nth_node(Children[i]))).Node_Name);
		mbPropertyIndices.push_back(Children[i]);
	}

	//    add my spouses
	/*for_each( Children.begin(), Children.end(), [&](int s) {
	 Graph_NodeIt gIt = Alarm.get_nth_node( s );
	 for_each( (*gIt).Parents.begin(), (*gIt).Parents.end(), [&](string spouse) {

	 //            if this spouse has not already been added OR its not ME as a father of my own child, add it
	 if( find( mbProperties.begin(), mbProperties.end(), spouse) == mbProperties.end() & spouse != Node_Name ) {
	 mbProperties.push_back( spouse );
	 mbPropertyIndices.push_back( Alarm.get_index( spouse ) );
	 }

	 });
	 });*/

	for (unsigned int i = 0; i < Children.size(); i++) {
		Graph_NodeIt gIt = Alarm.get_nth_node(Children[i]);
		for (unsigned int j = 0; j < gIt->Parents.size(); j++) {

			string spouse = gIt->Parents[j];

			//            if this spouse has not already been added OR its not ME as a father of my own child, add it
			if ((find(mbProperties.begin(), mbProperties.end(), spouse) == mbProperties.end()) && spouse != Node_Name) {
				mbProperties.push_back(spouse);
				mbPropertyIndices.push_back(Alarm.get_index(spouse));
			}
		}
	}

}

void Graph_Node::display() {
	cout << "\nNode name - " << Node_Name;
	cout << "\nValues that can be taken - ";
	//for_each( values.begin(), values.end(), [](string s) {cout << s << " ";});
	for (unsigned int i = 0; i < values.size(); i++) {
		cout << values[i] << " ";
	}

	cout << "\nMy parents -> ";
	//for_each( Parents.begin(), Parents.end(), [](string p) {cout << p << " ";});
	for (unsigned int i = 0; i < Parents.size(); i++) {
		cout << Parents[i] << " ";
	}

	cout << "\nMy children -> ";
	//for_each( Children.begin(), Children.end(), [](int p) {cout << (*(Alarm.get_nth_node(p))).Node_Name << " ";});
	for (unsigned int i = 0; i < Children.size(); i++) {
		int p = Children[i];
		cout << (*(Alarm.get_nth_node(p))).Node_Name << " ";
	}

	cout << "\nMy markov blanket -> ";
	printStringVector(mbProperties);
	cout << "\nMy CPT meanings -> \n";

	int CPTindex = 0;
	//for_each( permutationResult.begin(), permutationResult.end(), [&](vector<int> v) {
	for (unsigned int i = 0; i < permutationResult.size(); i++) {
		vector<int> v = permutationResult[i];
		cout << "P(" << Node_Name << "=" << values[v[0]] << "|";
		for (intIt it = v.begin() + 1; it != v.end(); it++) {
			Graph_NodeIt gIt = Alarm.search_node(Parents[it - v.begin() - 1]);
			cout << Parents[it - v.begin() - 1] << "=" << (*gIt).values[v[it - v.begin()]];
		}
		cout << ") = " << setprecision(6) << CPT[CPTindex++] << "\n";
	}

	//int MCPTindex = 0;
	for (unsigned int i = 0; i < mPermutationResult.size(); i++) {
		vector<int> v = mPermutationResult[i];
		cout << "P(" << Node_Name << "=" << values[v[0]] << "|";
		int index = 1;
		for (unsigned int i = 0; i < mbProperties.size(); i++) {
			string s = mbProperties[i];
			//					for_each( mbProperties.begin(), mbProperties.end(), [&](string s) {
			Graph_NodeIt gIt = Alarm.search_node(s);
			cout << s << "=" << gIt->values[v[index]];
			index++;
		}
		//cout << ") = " << setprecision(6) << MCPT[MCPTindex++] << "\n";//TODO
	}
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
				Alarm.addNode(new_node);

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

		Patient p;
		string lin = line;
		int y;

		for (int i = 0; i < Alarm.netSize(); i++) {
			y = lin.find(" ");
			if (lin.substr(0, y) == "\"?\"") {
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
	data.pop_back();
	initialDataSize = data.size();
	cout << "Data filled with data size - " << data.size() << endl;
}

float Graph_Node::retProbValue(vector<string> properties, vector<string> values) {
	float count_includingMe = 1.0, count_excludingMe = this->nvalues;

	string key_includingMe = properties[0] + "=" + values[0] + "|";
	string key_excludingMe = properties[0] + "!=" + "?" + "|";

	for (unsigned int i = 1; i < properties.size(); i++) {
		key_includingMe += properties[i] + "=" + values[i] + "|";
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

	cout << "countIncludingMe and countExcludingMe are " << setprecision(8) << count_includingMe << " " << count_excludingMe << endl;

	return count_includingMe / count_excludingMe;
}

int main(int argc, char** argv) {
	cout << "==========Started the Medical Diagnosis===========" << endl;
	if (argc != 3)
		exit(1);

	//getting the start time
	time_t startTime;
	time(&startTime);

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

	for (Graph_NodeIt it = Alarm.Pres_Graph.begin(); it != Alarm.Pres_Graph.end(); it++, index++) {

		cout << endl << (*it).Node_Name << " has index " << index << " and has been initialized\n";
		(*it).myInitialization();

	}

	//    APPLY COUNTING AND IGNORE THE ? VALUES

	while (true) {

		//=================================================M-Step===============================
		cout << "===============In M-step=============" << endl; //TODO: remove

		set_CPT_MAP();//TODO: remove
		//display_CPT_MAP();////TODO: remove
		for (Graph_NodeIt it = Alarm.Pres_Graph.begin(); it != Alarm.Pres_Graph.end(); it++, index++) {//TODO: remove
			it->findCPT();//TODO: remove
		}//TODO: remove
		output(); //TODO: remove
		exit(1);//TODO: remove

		set_MCPT_MAP();
		cout << "-----Done with calculating the MCPT_MAP" << endl;
		set_CPT_MAP();

		//display_CPT_MAP();

		for (Graph_NodeIt it = Alarm.Pres_Graph.begin(); it != Alarm.Pres_Graph.end(); it++, index++) {
			it->findCPT();
		}

		display_MCPT_MAP();

		output();

		cout << "====================Going out of M-step==============" << endl; //TODO: remove

		//=================================================E-Step===============================

		if (data.size() == initialDataSize) { //E-step for the first time

			vector<Patient> newData;

			for (unsigned int i = 0; i < data.size(); i++) {
				Patient patient = data[i];

				if (patient.uFieldName.compare(FULLY_KNOWN) == 0) {
					newData.push_back(patient);
					continue;
				}

				Graph_NodeIt unknownNode = Alarm.search_node(patient.uFieldName);

				unknownNode->mbValues.clear();

				vector<string> mbProperties_unknownNode = unknownNode->mbProperties;

				// for each markov blanket's node
				for (unsigned int j = 0; j < mbProperties_unknownNode.size(); j++) {
					string current_mbProp_name = mbProperties_unknownNode[j];
					string current_mbProp_val = patient.data[current_mbProp_name];

					unknownNode->mbValues.push_back(current_mbProp_val);
				}

				for (int j = 0; j < unknownNode->nvalues; j++) {
					string valueUnderConsideration = unknownNode->values[j];

					vector<string> vector_properties = mbProperties_unknownNode;
					vector<string> vector_values = unknownNode->mbValues;

					vector_properties.insert(vector_properties.begin(), unknownNode->Node_Name);
					vector_values.insert(vector_values.begin(), valueUnderConsideration);

					float probability = unknownNode->retProbValue(vector_properties, vector_values);
					Patient *newP = new Patient(patient, valueUnderConsideration, probability);
					newP->view();//TODO
					cin.ignore();//TODO
					newData.push_back(*newP);
					delete newP;
				}

			}

			data = newData;
		}//End of "if" => E-step first time

		else { //Not the first time

			for (unsigned int i = 0; i < data.size(); i++) {
				Patient patient = data[i];

				// no change in weight as this record is fully known
				if (patient.uFieldName.compare(FULLY_KNOWN) == 0)
					continue;

				Graph_NodeIt unknownNode = Alarm.search_node(patient.uFieldName);

				unknownNode->mbValues.clear();

				vector<string> mbProperties_unknownNode = unknownNode->mbProperties;

				// for each markov blanket's node
				for (unsigned int j = 0; j < mbProperties_unknownNode.size(); j++) {
					string current_mbProp_name = mbProperties_unknownNode[j];
					string current_mbProp_val = patient.data[current_mbProp_name];

					unknownNode->mbValues.push_back(current_mbProp_val);
				}

				//string valueUnderConsideration = unknownNode->values[j];

				vector<string> vector_properties = mbProperties_unknownNode;
				vector<string> vector_values = unknownNode->mbValues;

				vector_properties.insert(vector_properties.begin(), unknownNode->Node_Name);
				vector_values.insert(vector_values.begin(), patient.data[patient.uFieldName]);

				float probability = unknownNode->retProbValue(vector_properties, vector_values);

				data[i].weight = probability;

			}

		}

	}//End of While(true)


	/*	 } else {


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
	cout << "network \"Alarm\" { //37 variables and 37 probability distributions\n";
	cout << "}\n";

	int index = 0;
	for (Graph_NodeIt gIt = Alarm.Pres_Graph.begin(); gIt != Alarm.Pres_Graph.end(); gIt++, index++) {
		string nodeName = (*gIt).Node_Name;
		int nvalues = (*gIt).nvalues;
		cout << "variable  " << nodeName << " { //" << nvalues << " values\n";
		cout << "\ttype discrete[" << nvalues << "] {";
		//for_each( (*gIt).values.begin(), (*gIt).values.end(), [](string i) {cout << "  " << i;});
		for (unsigned int i = 0; i < gIt->values.size(); i++) {
			cout << "  " << gIt->values[i];
		}

		cout << " };\n";

		cout << "\tproperty \"position = (" << position[index][0] << ", " << position[index][1] << ")\" ;\n";
		cout << "}\n";
	}

	for (Graph_NodeIt gIt = Alarm.Pres_Graph.begin(); gIt != Alarm.Pres_Graph.end(); gIt++, index++) {
		string nodeName = (*gIt).Node_Name;
		vector<string> parents = (*gIt).Parents;
		vector<float> CPT = (*gIt).CPT;

		cout << "probability (  " << nodeName;
		//for_each( parents.begin(), parents.end(), [](string i) {cout << "  " << i;});

		for (unsigned int i = 0; i < parents.size(); i++) {
			cout << "  " << parents[i];
		}
		cout << " ) { //" << 1 + parents.size() << " variable(s) and " << CPT.size() << " values\n";
		cout << "\ttable ";
		printFloatVector(CPT);
		cout << ";\n}\n";
	}
}
