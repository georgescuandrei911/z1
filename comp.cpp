#include "comp.h"

//comp.cpp

//setters

void component :: set_number(int number){ this->number = number; }

void component :: set_id(int id){ this->id = id; }

void component :: set_value(double value){ this->value = value; }

//getters

int component :: get_number(){ return this->number; }

int component :: get_id(){ return this->id; }

double component :: get_value(){ return this->value; }

//overloaded input operator

std :: istream& operator>>(std :: istream& input, component& AUX)
{ 
    
    //read the values from the input stream

    int id, number;

    double value;

    input >> number >> id >> value;

    //set the values of the component object

    AUX.set_number(number);
    AUX.set_id(id);
    AUX.set_value(value);

    //return the input stream

    return input; 

}

//overloaded output operator

std :: ostream & operator<<(std :: ostream& output, component& AUX)
{
    //write the values to the output stream

    output << AUX.get_number() << " " << AUX.get_id() << " " << AUX.get_value();

    //return the output stream

    return output;
}


