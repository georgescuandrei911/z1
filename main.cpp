#include <iostream>
#include <fstream>

#include "comp.h"

int main() 
{

    std :: ifstream input("input.txt");

    int number_of_components = 0;

    input >> number_of_components;

    std :: cout << "number of components: " << number_of_components << std :: endl;

    int m[10][10];

    component *components = new component[number_of_components];

    for(int i = 0; i < number_of_components; i++)
    {

        //read the component from the input file
        
        input >> components[i];

        //display the component to the console

        std :: cout << "component " << i + 1 << ": " << components[i] << std :: endl;

    }

    input.close();

    std :: cout << "test completed successfully" << std :: endl;

    return 0;
}
