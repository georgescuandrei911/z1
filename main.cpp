#include <iostream>
#include <fstream>

#include "comp.h"

//start of the main function

int main()
{
//open the input file

std :: ifstream input("input.txt");

int number_of_components = 0;

//read the number of components from the input file

input >> number_of_components;

std :: cout << "number of components: " << number_of_components << std :: endl;

int m[10][10];

//create an array of component objects

component *components = new component[number_of_components];

int number_of_terminals = 0;

for(int i = 0; i < number_of_components; i++)
{

    //read the component from the input file
    
    input >> components[i];

    //determine the total number of terminals based on the id of the component

    number_of_terminals += components[i].id_to_terminal();

}

//create a 2D array to represent the connections between terminals

bool **node = new bool*[number_of_terminals];

for(int i = 0; i < number_of_terminals; i++){ node[i] = new bool[number_of_terminals]{false}; }

for(int i = 0; i < number_of_components; i++)
{

    for(int j = 0; j < components[i].id_to_terminal(); j++)
    {

        int k = 0; input >> k;

        while(k != 0)
        {
            
            //node[]

        }
    }

}

//delete the 2D array to free up memory

for(int i = 0; i < number_of_terminals; i++){ delete[] node[i]; }

delete[] node;

//close the input file

input.close();

//deallocate the memory used for the array of component objects

delete[] components;

return 0;