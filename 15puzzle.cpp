/*
15puzzle.cpp
A C++ program that solves the 15 puzzle,
or refuses to do so if it deems it unsolvable.
Tested on Ubuntu 18.04 and compiled in g++
Admissably, this is far from my best work.
Ben Jollymore
2019-02-01
*/
#include<iostream>
#include<stdlib.h>
#include<vector>
#include<deque>
#include<algorithm>

using namespace std;

//global placeholder for intial gameboard state and node visit counter
vector<int> gameBoard(16);
int counter = 1;

/*
Data structure that defines a visited node, representing a gamestate and the path to reach said state
*/
struct Node{
	vector<int> state = vector<int>(16); //game state at this node
	vector<string> path; //path to this state

	//Compare two nodes based on their state by default
	friend bool operator==(const Node& lhs, const Node& rhs){
		if(lhs.state == rhs.state){
			return true;
		}
		else{
			return false;
		}
	}
};

/*
Function to initialize gameboard state.
*/
void initGameBoard(char control){

	for (int i = 0; i < 16; i++){
		gameBoard[i] = i + 1;
	}

	switch (control){
		case 's':
		gameBoard[11] = 15;
		gameBoard[15] = 0;
		gameBoard[10] = 12;
		gameBoard[14] = 11;
		break;

		case 'u':
		gameBoard[15] = 0;
		gameBoard[14] = 14;
		gameBoard[13] = 15;
	}	
}

/*
Function to check if puzzle is solvable before attempting
*/
bool isSolvable(vector<int> board){
	int inversions = 0; //number of tiles out of order
    int row = 0; 
    int bRow = 0; // the row with blank 

    for (int i = 0; i < board.size(); i++)
    {
        if (i % 4 == 0) { // advance to next row
        	row++;
        }
        if (board[i] == 0) { // the blank tile
            bRow = row; // save the row on which encountered --> in retrospect, 
            			//probably should have used findBlank() function i declare immediately after this
            			//but I leave for work in 25 min so no time to fix that now
            			//procrastination is bad im sorry
            continue;
        }
        for (int j = i + 1; j < board.size(); j++)
        {
        	if (board[i] > board[j] && board[j] != 0)
        	{
        		inversions++; //count inversions
        	}
        }
    }

   
    if (bRow % 2 == 0) { // blank on odd row
        return inversions % 2 == 0;
    } else { // blank on even row
        return inversions % 2 != 0;
    }   
}

/*
Function to find blank space
*/
int findBlank(vector<int> board){
	for (int i = 0; i < 16; i++){
		if(board[i] == 0){
			return i;
		}
	}
}

/*
Function to check if puzzle has been solved
*/
bool isSolved(vector<int> board){
	bool win = true;
	for (int i = 0; i < 15; i++){
		if(board[i] != i + 1){
			win = false; //if not in ascending order up to the 15th tile, not solved
		}
	}
	return win;
}

/*
Function to print out gameboard state
*/
void printGameBoard(vector<int> board){

	cout <<  board[0] << '\t';
	for (int i = 1; i < 16; i++){
		cout << board[i] << '\t';
		if (i%4 == 3){
			cout << endl;
		}
	}
	cout << endl;
}

/*
Function that moves right and returns the new gamestate
*/
vector<int> moveRight(vector<int> newBoard, int blankPosition){
	newBoard[blankPosition] = newBoard[blankPosition - 1];
	newBoard[blankPosition - 1] = 0;
	return newBoard;
}

/*
Function that moves left and returns the new gamestate
*/
vector<int> moveLeft(vector<int> newBoard, int blankPosition){
	newBoard[blankPosition] = newBoard[blankPosition + 1];
	newBoard[blankPosition + 1] = 0;
	return newBoard;
}

/*
Function that moves up and returns the new gamestate
*/
vector<int> moveUp(vector<int> newBoard, int blankPosition){
	newBoard[blankPosition] = newBoard[blankPosition + 4];
	newBoard[blankPosition + 4] = 0;
	return newBoard;
}

/*
Function that moves down and returns the new gamestate
*/
vector<int> moveDown(vector<int> newBoard, int blankPosition){
	newBoard[blankPosition] = newBoard[blankPosition - 4];
	newBoard[blankPosition - 4] = 0;
	return newBoard;
}

/*
Function to execute possible moves from a given state, checking first if member is in closed and open list
*/
void makeMove(vector<int> board, Node currentNode, deque<Node>* branches, deque<Node>* closed){

	int blank = findBlank(board);

	if(blank % 4 != 0){ // can go right
		Node newNode;
		newNode.state = moveRight(board, blank);
		newNode.path = currentNode.path;
		newNode.path.push_back("Right");
		if (find(branches->begin(), branches->end(), newNode) == branches->end())
		{	
			if (find(closed->begin(), closed->end(), newNode) == closed->end())
			{
				branches->push_back(newNode); //not in open or closed list
			}
		}
	}

	if(blank > 3){ //can go down
		Node newNode;
		newNode.state = moveDown(board, blank);
		newNode.path = currentNode.path;
		newNode.path.push_back("Down");
		if (find(branches->begin(), branches->end(), newNode) == branches->end())
		{	
			if (find(closed->begin(), closed->end(), newNode) == closed->end())
			{
				branches->push_back(newNode);//not in open or closed list
			}
		}
	}

	if(blank < 11){	//can go up
		Node newNode;
		newNode.state = moveUp(board, blank);
		newNode.path = currentNode.path;
		newNode.path.push_back("Up");
		if (find(branches->begin(), branches->end(), newNode) == branches->end())
			if (find(closed->begin(), closed->end(), newNode) == closed->end())
			{
				branches->push_back(newNode);//not in open or closed list
			}
		}

		if(blank % 4 != 3){ //can go left
			Node newNode;
			newNode.state = moveLeft(board, blank);
			newNode.path = currentNode.path;
			newNode.path.push_back("Left");
			if (find(branches->begin(), branches->end(), newNode) == branches->end())
			{	
				if (find(closed->begin(), closed->end(), newNode) == closed->end())
				{
					branches->push_back(newNode);//not in open or closed list
				}
			}
		}
	}

/*
Performs BFS search. Uses a Deque as a single ended queue.
Using deque allows code to be more modular and allows iterator passthroughs.
*/
bool breadthFirst(Node firstNode){

	deque<Node> nodeQueue;
	deque<Node> closed;

	nodeQueue.push_back(firstNode);

	while(!nodeQueue.empty()){

		//pull first node off queue
		Node currentNode = nodeQueue.front();
		cout << "Level: " << currentNode.path.size() << endl;

		if(isSolved(currentNode.state)){
			cout << "Found it using BFS! I checked " << counter << " unique nodes.\n";
			cout << "Here is the path:\n";
			for (vector<string>::const_iterator i = currentNode.path.begin(); i != currentNode.path.end(); ++i){
				cout << *i <<endl;
			}
			return true;
		}

		//increment number of nodes tested
		counter++;

		//send tested node to closed and pop off front of queue
		closed.push_back(currentNode);
		nodeQueue.pop_front();

		//spawn children
		makeMove(currentNode.state, currentNode, &nodeQueue, &closed);

	}

	//couldnt get it after ~30k years of trying
	cout << "I am unsolvable, but the heuristic didn't catch it, or I am broken!\n";
	return false;
}

/*
Performs DFS search. Uses a Deque as a single ended stack.
Using deque allows code to be more modular and allows iterator passthroughs.
This can EAT THROUGH RAM (100mb/s on my machine) and often returns overly convoluted solutions too deep in the tree
*/
bool depththFirst(Node firstNode){

	deque<Node> nodeStack;
	deque<Node> closed;

	nodeStack.push_back(firstNode);

	while(!nodeStack.empty()){

		//get current node off end of stack
		Node currentNode = nodeStack.back();

		//I accidentally did this while trying to get iterative DFS to work around 5am, but it makes regular 
		//DFS work, except it often finds an unneccesarily convoluted solution deep in the tree. 
		//I don't know why. But it is staying here, for now.
		if (currentNode.path.size() < 5 && currentNode.path.size() > 1){
			nodeStack.pop_back();
		}

		cout << "Level: " << currentNode.path.size() << endl;

		if(isSolved(currentNode.state)){
			cout << "Found it using DFS! I checked " << counter << " unique nodes.\n";
			cout << "Here is the path:\n";
			for (vector<string>::const_iterator i = currentNode.path.begin(); i != currentNode.path.end(); ++i){
				cout << *i <<" ";
			}
			cout << endl;
			return true;
		}

		//increment number of nodes tested
		counter++;

		//send tested node to closed and pop off end of stack
		closed.push_back(currentNode);
		nodeStack.pop_back();

		//spawn children
		makeMove(currentNode.state, currentNode, &nodeStack, &closed);

	}
	//couldnt get it after ~30k years of trying
	cout << "I am unsolvable, but the heuristic didn't catch it, or I am broken!\n";
	return false;
}

/*
Performs IDFS search. Uses a Deque as a single ended stack.
Using deque allows code to be more modular and allows iterator passthroughs.
*/
bool iterativeDepththFirst(Node firstNode){

	deque<Node> nodeStack;
	deque<Node> closed;

	//iterate, max 500 so computer doesnt freak out and crash
	for (int iteration = 1; iteration < 500; iteration++){

		//wipe stack and start fresh grabbing first node
		nodeStack.clear();
		nodeStack.push_back(firstNode);
		Node currentNode = nodeStack.back();

		int level = 0; //start at level 0

		//loop until hit max depth
		while(!nodeStack.empty() && level <= iteration){

			//grab current node from stack
			currentNode = nodeStack.back();
			closed.push_back(currentNode);

			level = currentNode.path.size();

			cout << "Iteration " << iteration << "Level " << currentNode.path.size() <<endl;

			if(isSolved(currentNode.state)){
				cout << "Found it using IDFS! I checked " << counter << " unique nodes.\n";
				cout << "Here is the path:\n";
				for (vector<string>::const_iterator i = currentNode.path.begin(); i != currentNode.path.end(); ++i){
					cout << *i <<endl; ///print path
				}
				return true;
			}
			counter++;

			closed.push_back(currentNode);

			//remove first node from closed so it doesnt get stuck next iteration
			if(closed.size() >= 1){
				closed.pop_front();
			}
			nodeStack.pop_back();

			makeMove(currentNode.state, currentNode, &nodeStack, &closed);
			
		}
	}

	//couldnt get it after ~30k years of trying
	cout << "I am unsolvable, but the heuristic didn't catch it, or I am broken!\n";
	return false;
}

//main
int main(){

	//init board
	cout << "Select if a solvable (s) or unsolvable (u) puzzle will be tested: (s/u)";
	char puzzleType;
	cin >> puzzleType;

	initGameBoard(puzzleType);

	cout << "First State:\n";
	printGameBoard(gameBoard);

	if(isSolvable(gameBoard)){

		Node first;
		first.state = gameBoard;

		char control;
		cout << "This puzzle was determined to be solvable.\nEnter b for BFS, d for DFS, or i for DFS-Iterative: (b/d/i)";
		cin >> control;

		switch(control){

			case 'b': breadthFirst(first);
			break;
			case 'd': depththFirst(first);
			break;
			case 'i': iterativeDepththFirst(first);
		}
	}

	else{
		cout << "This puzzle has been determined to be unsolvable\nProgram terminating on exit code -1\n";
		return -1;
	}

	cout << "Puzzle has been solved. Program terminating on exit code 0\n";
	return 0;
}
