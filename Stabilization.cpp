
/* Implementation of a probabilistic fault containment algorithm on a self-stabilizing system.
   The program will execute until the system achieves legal configuration or indefinately.

   TERMS:
        Neighborhood: The neighborhood of a node is the node and its right and left values.
        Local Leader: A node is the local leader if the value of its secondary variable is greater or equal to those of its neighbors.
        Legal Configuration: The system has assumed a legal configuration when the primary values of all nodes are equal.

   STABILIZATION ALGORITHM:
   (1) A node in the system is randomly selected for analysis.

   (2) If the primary value of a neighbor node is different from the primary value of the node:

       (a) If the node is the local leader: node.secondary += (the max of neighbor node secondary variables + M)
       (b) If the node is not the local leader: node.secondary += 1, but node.primary holds

   (3) If none of the primary values are equal between the node and its neighbors, the nodes primary value will flip.

   Data structure to be first investigated is the linked list. */

#include <iostream>
#include <cstdlib>
#include <time.h>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;

const int M = 20;   // Arbitrary variable for stabilization algorithm.

class Node;

/* Node class.
   Contains primary and secondary variables for stabilization logic.
   Each node is connected in a linked list. */

class Node {
private:
    int primary;      // Primary attribute
    int secondary;    // Secondary attribute
    Node* left;       // Left neighbor
    Node* right;      // Right neighbor

public:
    /* Default constructor. */

    Node(){
        primary = 0;
        secondary = 5;  // Arbitrary value
        left = NULL;
        right = NULL;
    }

    /* Compares the primary variables of two nodes.
       Returns true when the two primary values are equal. */

    bool Equal(const Node other){
        if (primary == other.primary){
            return true;
        }
        return false;
    }

    /* Updates the primary value of the node.
       In our system this will simply flip the value. */

    void Update(){
        if (primary == 0){
            primary = 1;
        }
        else {
            primary = 0;
        }
    }

    /* Prints primary variable to standard output. */

    void Print(){
        cout << primary << ' ';
    }

    friend class System;
};

class System {
private:
    int SYSTEM_SIZE;	// Number of Nodes in system
    Node* member;    	// Array of Nodes
    Node* node;         // Pointer to the current node

public:
    /* Default constructor.
       Establishes the relational context of each node with its neighbors. */

    System(int _SYSTEM_SIZE){
        SYSTEM_SIZE = _SYSTEM_SIZE;
        member = new Node[SYSTEM_SIZE];
        
        for (int i = 0; i < SYSTEM_SIZE; i++){
            if (i == 0){                           // The first node
                member[i].right = &member[i + 1];
            }
            else if (i == SYSTEM_SIZE - 1){        // The last node
                member[i].left = &member[i - 1];
            }
            else {
                member[i].left = &member[i - 1];
                member[i].right = &member[i + 1];
            }
        }

        node = &member[0];  // Set the node to the first node
    }

    /* Random scheduler.
       Directs node* to a random member[]. 
       The scheduler chooses the ith node. */

    void SelectNode(){
        int i = rand() % SYSTEM_SIZE;  // Random index

        node = &member[i];  // ith node
    }

    /* Simulates a transient fault within the system.
       Only effects primary variables. */

    void TransientFault(){
        SelectNode();
        node->Update();
    }

    /* Checks if the system is in legal configuration.
       Using the first nodes primary variable as a reference against each other node. */

    bool LegalConfig(){
        for (int i = 1; i < SYSTEM_SIZE; i++){
            if (!member[0].Equal(member[i])){
                return false;
            }
        }
        return true;
    }

    /* Stabilization implementation.
       Processes until legal configuration condition is met. */

    void Stabilize(){
        while (!LegalConfig()){
            SelectNode();
            
            // If true, then (2) is not satisfied.
            if (!CheckUnequal()){
                CheckConditions();
            }
            //Print();
        }

        cout << "\nSYSTEM LEGAL\n";
    }

    /* Checks if the secondary values of the nodes in the local neighborhood are unequal.
       Returns true if there exists a node in the local neighborhood with a primary value
       not equal to its neighbors primary value. */

    bool CheckUnequal(){
        // If the ith node has both left and right neighbors
        if ((node->left != NULL) && (node->right != NULL)){
            // If both neighbors have different state, update the node since (3) is satisfied.
            if ((node->primary != node->left->primary) && (node->primary != node->right->primary)){
                node->Update();
                return true;
            }
            // Else if the neighboring nodes all have the same state as the ith, do nothing.
            else if ((node->primary == node->left->primary) && (node->primary == node->right->primary)){
                return true;
            }
            // Else check other Rules
            else {
                return false;
            }
        }
        // Else if the node has only a right neighbor
        else if (node->left == NULL){
            // Update the node due to (3)
            if (node->primary != node->right->primary){
                node->Update();
                return true;
            }
            // Else if the right node has the same state as the ith, do nothing.
            else if (node->primary == node->right->primary){
                return true;
            }
            else {
                return false;
            }
        }
        // Else if the node has only a left neighbor
        else if (node->right == NULL){
            // Update the node due to (3)
            if (node->primary != node->left->primary){
                node->Update();
                return true;
            }
            // Else if the left node has the same state as the ith, do nothing.
            else if (node->primary == node->left->primary){
                return true;
            }
            // Else check other Rules
            else {
                return false;
            }
        }
    }

    /* Checks conditions when there exists some neighbor of the ith node 
       which has a different state than the ith node, but not all neighbors
       have a different state.
       Checks if the node satisfies Rules 2a or 2b. */

    void CheckConditions(){
        // If 2a is true
        if (isLeader()){
            node->Update();
            node->secondary += (Max() + M);
        }
        // If 2b is true
        else {
            //node->Update();
            node->secondary++;
        }
    }

    /* Checks if the current node is the local leader. */

    bool isLeader(){
        if (node->left == NULL){
            if (node->secondary < node->right->secondary){
                return false;
            }
            else {
                return true;
            }
        }
        else if (node->right == NULL){
            if (node->secondary < node->left->secondary){
                return false;
            }
            else {
                return true;
            }
        }
        else if ((node->secondary < node->left->secondary) ||
                 (node->secondary < node->right->secondary)){
            return false;
        }
        return true;
    }

    /* Returns the greater secondary value between the neighbor nodes */

    int Max(){
        if (node->left == NULL){
            return node->right->secondary;
        }
        else if (node->right == NULL){
            return node->left->secondary;
        }
        else if (node->right->secondary > node->left->secondary){
            return node->right->secondary;
        }
        else {
            return node->left->secondary;
        }
    }

    /* Calls the Print() function of each node in the system starting from the current node. */

    void Print(){
        node = &member[0];

        while (node != NULL){
            node->Print();
            node = node->right;
        }

        cout << '\n';
        node = &member[0];
    }
};

void print();

int main()
{
    srand(time(NULL));
    boost::posix_time::ptime start, stop;
    boost::posix_time::time_duration time;
    int size, faults;
    string next;
    
    cout << "\nEnter system size: ";
    cin >> size;
    System graph(size);
    cout << "\nEnter number of simulated faults: ";
    cin >> faults;
    
    cout << "\nSYSTEM STATUS\n";
    for (int i = 0; i < faults; i++){
        graph.TransientFault();
        graph.Print();
    }
    print();
    cin.ignore(256, '\n');
    getline(cin, next);
        
    start = boost::posix_time::microsec_clock::local_time();
    graph.Stabilize();
    stop = boost::posix_time::microsec_clock::local_time();
    time = stop - start;

    graph.Print();
    cout << "\nStabilization performance: " << time.total_microseconds() << " microseconds.\n\n";

    return 0;
}

// Prints out a series of '_' for formatting purposes.
void print(){cout << "________________________________________________________________\n";}
