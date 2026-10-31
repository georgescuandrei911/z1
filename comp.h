#include <fstream>

//comp.h

class component
{

    private:

    //data members

    int number = 0;
    int id = 0;
    double value = 0.0;

    public:

    //default constructor

    component() = default;

    //setters

    void set_number(int number);

    void set_id(int id);

    void set_value(double value);

    //getters

    int get_number();

    int get_id();

    double get_value();

    //overloaded input and output operators

    friend std :: istream& operator>>(std :: istream& input, component& AUX);

    friend std :: ostream& operator<<(std :: ostream& output, component& AUX);

};
