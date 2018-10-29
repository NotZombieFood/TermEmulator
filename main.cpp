/*
* @compiler: The one that is in Visual Studio
* @author: Gerardo Cruz & Miguel Triana
* @note: this wont work on linux, but why we would use a linux term emulator in linux duh
* @repo: https://github.com/NotZombieFood/TermEmulator
*/


#include "stdafx.h"  //precompiled stuff that visual studio uses for faster compiling
#include <windows.h> //https://docs.microsoft.com/es-es/windows/desktop/api/winbase/nf-winbase-getcurrentdirectory
#include <string>
#include <iostream>
#include <stdlib.h> 
#include<stdio.h> 
#include <cstring>
#include <fstream>
#include <algorithm>  //uses for deleting whitespaces
#include <sys/stat.h>   //used for the isFolder function
#include <dirent.h>  //for viewing inside folders, this may not be included in your compiler files More info on how to solve the issue: https://web.archive.org/web/20170428133315/http://www.softagalleria.net/dirent.php
#include <direct.h>
using namespace std;

struct node {
	struct node * father;
	char path[250];
	char name[50];
	int isFolder;
	struct node * childs [15];
};

string ExePath() {
	/*
	* @desc: Function that will give us the path in which we are running
	* @params: None
	* @note: use GetModuleFileNameA instead of GetModuleFileName (this was discovered by experimenting)
	* @retval: String
	* @reference: https://stackoverflow.com/a/875264
	*/
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

int isFolder(char * path) {
	/*
	* @desc: Function that will let us know if something is a folder
	* @params: Path, absolut or relative
	* @note: we dont have an error handler so good luck 
	* @retval: int, 1 for folder, 0 for file
	* @reference: https://stackoverflow.com/a/146938
	*/
	struct stat s;
	if (stat(path, &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
			return 1;
		}
		else 
		{
			return 0;
		}
	}
	return 0;
}

void initChilds(node * pointer) {
	/*
	* @desc: Set all child pointers to NULL
	* @params: pointer
	* @note: 
	* @retval: void
	* @reference: 
	*/
	for (int i = 0; i < 16; i++) {
		pointer->childs[i] = NULL;
	}
}

void RecursiveStructure(node * pointer) {
	/*
	* @desc: Recursive function to create our structure
	* @params: a pointer
	* @note:
	* @retval: none
	* @reference: https://stackoverflow.com/a/612176
	*/
	pointer->isFolder = isFolder(pointer->path);
	DIR *dir;
	struct dirent *ent;
	char full_path [250];
	int i = 0;
	if ((dir = opendir(pointer->path)) != NULL) {
		pointer->isFolder =	1;
		/* if we dont enter this if, it is an error or is it a file, so wont call the recursive function
		 the while will iterate through all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_name[0] != '.' &&  ent->d_name[1] != '.') {
				strcpy_s(full_path, pointer->path);
				strcat_s(full_path, "\\");
				strcat_s(full_path, ent->d_name);
				struct node* new_node = (struct node*) malloc(sizeof(struct node));
				new_node->father = pointer;
				initChilds(new_node);
				strcpy_s(new_node->name, ent->d_name);
				pointer->childs[i] = new_node;
				strcpy_s(new_node->path, full_path);
				//cout << full_path << endl;
				RecursiveStructure(new_node);
				i++;
				if (i == 16) {
					break;
				}
			}
		}
		closedir(dir);
	}
	else {
		pointer->isFolder = 0;
	}
}

node * createStructure(string path) {
	/*
	* @desc: Scans through the running folder and creates the tree structure for it 
	* @params: Path, absolut or relative
	* @note: 
	* @retval: Pointer of the root
	* @reference: 
	*/
	struct node * root = NULL;
	root = (struct node *) malloc(sizeof(struct node));
	root->father = NULL;
	strcpy_s(root->name, "ROOT");
	strcpy_s(root->path, path.c_str());
	initChilds(root);
	RecursiveStructure(root);
	return root;
}

void printTree(node * pointer, int level) {
	/*
	* @desc: Recursive function to print our structure
	* @params  pointer
	* @note:
	* @retval: none
	* @reference: 
	*/
	int i = 0;
	string prefix(level, '\t');
	if (pointer->father != NULL) {
		cout << prefix + pointer->name << endl;
	}
	while (pointer->childs[i] != NULL) {
		if (pointer->father == NULL) {
			printTree(pointer->childs[i], level);
		}
		else {
			printTree(pointer->childs[i], level + 1);
		}
		i++;
	}
}

void printChilds(node * pointer) {
	/*
	* @desc: Function to print the childs of a node
	* @params  pointer
	* @note:
	* @retval: none
	* @reference:
	*/
	int i = 0;
	while (pointer->childs[i] != NULL) {
		cout << pointer->childs[i]->name << endl;
		i++;
	}
}

int changeDirectory(node * pointer, string directory) {
	/*
	* @desc: See if we can change directory and retrieve the position in the array
	* @params: Pointer of current directory, string of the directory
	* @note:  
	* @retval: int with position, 16 if it does not exist,17 is it not a folder, 18 to go back
	* @reference: 
	*/
	if (directory =="..") {
		return 18;
	}
	int i = 0;
	while (pointer->childs[i] != NULL) {
		if (pointer->childs[i]->name == directory && pointer->childs[i]->isFolder) {
			return i;
		}
		else if (pointer->childs[i]->name == directory){
			return 17;
		}
		i++;
	}
	return 16;
}

int createDirectory(node * pointer, string directory, int simulation) {
	/*
	* @desc: Create directory
	* @params: Pointer of current directory, string of the directory
	* @note:
	* @retval: 0 if there was an error, 1 if created, 2 if we cant create more files/folders, 3 if it exists
	* @reference:
	*/
	int i = 0;
	while (pointer->childs[i] != NULL) {
		if (pointer->childs[i]->name == directory) {
			cout << "The folder already exists" << endl;
			return 3;
		}
		i++;
		if (i == 16) {
			cout << "We can't create more folders" << endl;
			return 2;
		}
	}
	if (pointer->childs[i] == NULL) {
		char full_path[250];
		strcpy_s(full_path, pointer->path);
		strcat_s(full_path, "\\");
		strcat_s(full_path, directory.c_str());
		struct node* new_node = (struct node*) malloc(sizeof(struct node));
		pointer->childs[i] = new_node;
		new_node->father = pointer;
		new_node->isFolder = 1;
		initChilds(new_node);
		strcpy_s(new_node->name, directory.c_str());
		strcpy_s(new_node->path, full_path);
		if (simulation == 48) {
			if (CreateDirectoryA(full_path, NULL) ||
				ERROR_ALREADY_EXISTS == GetLastError())
			{
				return 1;
			}
			else
			{
				cout << "Errow while creating folder" << endl;
				return 0;
			}
		}
	}
	return 0;
}

int createFile(node * pointer, string filename, int simulation) {
	/*
	* @desc: Create filename
	* @params: Pointer of current directory, string of the filename
	* @note:
	* @retval: 0 if there was an error, 1 if created, 2 if we cant create more files/folders
	* @reference:
	*/
	int i = 0;
	while (pointer->childs[i] != NULL) {
		if (pointer->childs[i]->name == filename) {
			cout << "The file already exists" << endl;
			return 3;
		}
		i++;
		if (i == 16) {
			cout << "We can't create more files" << endl;
			return 2;
		}
	}
	if (pointer->childs[i] == NULL) {
		char full_path[250];
		strcpy_s(full_path, pointer->path);
		strcat_s(full_path, "\\");
		strcat_s(full_path, filename.c_str());
		struct node* new_node = (struct node*) malloc(sizeof(struct node));
		pointer->childs[i] = new_node;
		new_node->isFolder = 0;
		new_node->father = pointer;
		initChilds(new_node);
		strcpy_s(new_node->name, filename.c_str());
		strcpy_s(new_node->path, full_path);
		if (simulation == 48) {
			ofstream o(full_path);
			o << "Hello, World" << endl;
		}
	}
	return 0;
}

void testingFunction() {
	/*
	* @desc: Here i will have some of the tests i have done for veryfing functionality
	* @params: none
	* @note:  dont add this to the main or we will have a lot of trash over there
	* @retval: nothing, just the fun of testing
	* @reference: https://gph.is/28Ovq9H
	*/
	isFolder("C:\\Users\\gerar\\Documents\\Data_structures");
	isFolder("C:\\Users\\gerar\\Documents\\Data_structures\\myLinkedList.cpp");
	//printTree(pointer, 0);
}

string delSpaces(string &str)
{
	/*
	* @desc: function to delete spaces in a string
	* @params: string
	* @note:  this was a construction of multiple solutions found in the internet
	* @retval: new string
	* @reference: 
	*/
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	return str;
}

string parse(int pos,string command) {
	/*
	* @desc: remove stuff from string
	* @params: position(the count of characters in the command, example ls is 2, touch is 5), the command
	* @note:
	* @retval: string (the argument of the command)
	* @reference:
	*/
	string temp;
	temp = delSpaces(command);
	return temp.substr(pos, temp.length()-1);
}

void find(node * pointer, string filename) {
	/*
	* @desc: find in a given directory 
	* @params: Pointer of current directory, string of the filename/foldername
	* @note:
	* @retval: void
	* @reference:
	*/
	int i = 0;
	while (pointer->childs[i] != NULL) {
		string str(pointer->childs[i]->name);
		if (str.find(filename) != std::string::npos){
			cout << pointer->childs[i]->path << endl;
		}
		find(pointer->childs[i], filename);
		i++;
	}
}

void orderArray(node * pointer) {
	for (int i = 0; i < 16; i++)
	{
		if (pointer->childs[i] == NULL) {
			int x;
			for (x=i+1; x < 16; x++)
			{
				if (pointer->childs[x] != NULL) {
					pointer->childs[i] = pointer->childs[x];
					pointer->childs[x] = NULL;
				}
			}
		}
	}
}

void removeRecursive(node * pointer, int position, int simulation) {
	int i = 0;
	while (pointer->childs[i] != NULL) {
		removeRecursive(pointer->childs[i], i, simulation);
	}
	pointer->father->childs[position] = NULL;
	orderArray(pointer->father);
	if (simulation == 48) {
		if (pointer->isFolder) {
			_rmdir(pointer->path);
		}
		else {
			remove(pointer->path);
		}
	}
	//free(pointer);
}

void remove(node * pointer, string filename, int simulation) {
	/*
	* @desc: remove a given directory or file
	* @params: Pointer of current directory, string of the filename/foldername
	* @note: free(temp);
	* @retval: void
	* @reference:
	*/
	int i = 0;
	while (pointer->childs[i] != NULL) {
		if (filename == pointer->childs[i]->name) {
			removeRecursive(pointer->childs[i], i, simulation);
		}
		i++;
		if (i == 16) {
			cout << "The folder or file dont exists" << endl;
			return;
		}
	}
}


int main() {
	size_t requiredSize = 15;
	char * lib_var;
	lib_var = (char*)malloc(requiredSize * sizeof(char));
	getenv_s(&requiredSize, lib_var, requiredSize, "USERNAME");
	int simulation = 1;
	cout << "Term emulation has started, use help for more info" << endl;
	string input;
	string temp_string;
	int response;
	string path = ExePath();
	struct node * root = createStructure(path);
	struct node * current_folder = root;
	while (input != "exit") {
		//this cout gave and error when on the same line
		cout << lib_var;
		cout << "@";
		cout << current_folder->path;
		cout << ">>>";
		getline(cin, input);
		if (input == "help") {
			cout << "IMPORTANT: Set non simulated mode with this command: simulation 0, you can toggle it back later.\nCommands that can be used\n\thelp gives you the list of commands\n\ttree Show all the structure\n\tls Current folder files\n\trm Remove folder or files\n\ttouch <<filename>> Creates the file\n\tmkdir <<foldername>> Creates a folder\n\tcd goes into a folder" << endl;
		}
		else if (input.find("simulation") != std::string::npos){
			temp_string = delSpaces(input);
			if (temp_string[temp_string.length() - 1] != '0' && input[temp_string.length() - 1] != '1') {
				cout << "Error in simulation command, verify the parameters." << endl;
			}
			else {
				simulation = temp_string[temp_string.length() - 1] + 0;
				cout << "The simulation mode has been set" << endl;
			}
		}
		else if (input.find("exit") != std::string::npos) {
			cout << "Exit command has been issued, see you later :)" << endl;		
		}
		else if (input.find("mode") != std::string::npos) {
			if (simulation == 48) {
				cout << "Real mode" << endl;
			}
			else {
				cout << "Simulation mode" << endl;
			}
		}
		else if (input.find("tree") != std::string::npos) {
			printTree(current_folder, 0);
		}
		else if (input.find("ls") != std::string::npos) {
			printChilds(current_folder);
		}
		else if (input.find("cd") != std::string::npos) {
			response = changeDirectory(current_folder, parse(2,input));
			if (response == 16) {
				cout << "Directory doesn't exist" <<  endl;
			}
			else if (response == 17) {
				cout << parse(2,input) + "is a file. We cant do that." << endl;
			} 
			else if (response == 18) {
				if (current_folder->father != NULL) {
					current_folder = current_folder->father;
				}
				else {
					cout << "We cant go behind this path, sorry." << endl;
				}
			}
			else {
				current_folder = current_folder->childs[response];
			}
		}
		else if (input.find("mkdir") != std::string::npos) {
			createDirectory(current_folder, parse(5, input), simulation);
		}
		else if (input.find("touch") != std::string::npos) {
			createFile(current_folder, parse(5, input), simulation);
		}
		else if (input.find("find") != std::string::npos) {
			find(current_folder, parse(4, input));
		}
		else if (input.find("rm") != std::string::npos) {
			remove(current_folder, parse(2, input),simulation);
		}
		else {
			cout << "Command was not recognized, please use command help for more instructions" << endl;
		}
	}
	
}
