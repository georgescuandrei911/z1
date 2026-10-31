#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

#include "comp.h"

//solve the equations using Gaussian elimination

bool solve(double **matrix, double *answer, int size)
{

    for(int i = 0; i < size; i++)
    {

        int pivot = i;

        for(int j = i + 1; j < size; j++)
        {
            if(std :: abs(matrix[j][i]) > std :: abs(matrix[pivot][i]))
            {
                pivot = j;
            }
        }

        if(std :: abs(matrix[pivot][i]) < 1e-15){ return false; }

        std :: swap(matrix[i], matrix[pivot]);

        for(int j = i + 1; j < size; j++)
        {

            double factor = matrix[j][i] / matrix[i][i];

            matrix[j][i] = 0;

            for(int k = i + 1; k <= size; k++)
            {
                matrix[j][k] -= factor * matrix[i][k];
            }

        }

    }

    for(int i = size - 1; i >= 0; i--)
    {

        double value = matrix[i][size];

        for(int j = i + 1; j < size; j++)
        {
            value -= matrix[i][j] * answer[j];
        }

        answer[i] = value / matrix[i][i];

        if(!std :: isfinite(answer[i])){ return false; }

    }

    return true;

}

//add a conductance and a current source between two nodes
//current is positive from terminal a to terminal b

void add_branch(double **matrix, int size, int a, int b,
                double conductance, double current)
{

    if(a >= 0)
    {
        matrix[a][a] += conductance;
        matrix[a][size] -= current;
    }

    if(b >= 0)
    {
        matrix[b][b] += conductance;
        matrix[b][size] += current;
    }

    if(a >= 0 && b >= 0)
    {
        matrix[a][b] -= conductance;
        matrix[b][a] -= conductance;
    }

}

//start of the main function

int main()
{

    std :: ifstream input("input.txt");

    int number_of_components = 0;

    if(!(input >> number_of_components) || number_of_components <= 0)
    {
        std :: cerr << "Invalid component count." << std :: endl;
        return 1;
    }

    component *components = new component[number_of_components];

    int **node = new int*[number_of_components]{};

    //capacitor state stores voltage
    //inductor state stores current

    double *state = new double[number_of_components]{};

    int number_of_sources = 0;
    int number_of_nodes = 0;

    bool valid = true;

    //read the components

    for(int i = 0; i < number_of_components; i++)
    {

        if(!(input >> components[i]))
        {
            valid = false;
            break;
        }

        int id = components[i].get_id();
        double value = components[i].get_value();

        if(id < 1 || id > 5 || !std :: isfinite(value))
        {
            valid = false;
            break;
        }

        if((id == 1 || id == 3 || id == 4) && value <= 0)
        {
            valid = false;
            break;
        }

        if(id == 2){ number_of_sources++; }

    }

    //all supported component types have two terminals

    for(int i = 0; i < number_of_components && valid; i++)
    {

        node[i] = new int[2]{};

        for(int j = 0; j < 2; j++)
        {

            if(!(input >> node[i][j]) || node[i][j] < 0)
            {
                valid = false;
                break;
            }

            number_of_nodes = std :: max(number_of_nodes, node[i][j]);

        }

    }

    if(!valid)
    {

        std :: cerr << "Invalid input. Supported component types: 1 to 5."
                    << std :: endl;

        for(int i = 0; i < number_of_components; i++){ delete[] node[i]; }

        delete[] node;
        delete[] state;
        delete[] components;

        return 1;

    }

    int size = number_of_nodes + number_of_sources;

    double **matrix = new double*[size];

    for(int i = 0; i < size; i++){ matrix[i] = new double[size + 1]{}; }

    double *answer = new double[size]{};
    double *current = new double[number_of_components]{};

    //simulation settings

    double dt = 1e-5;
    double duration = 0.05;

    int steps = static_cast<int>(std :: ceil(duration / dt));

    //save node voltages and component currents

    std :: ofstream output("simulation.csv");

    bool success = output.is_open();

    output.precision(12);

    output << "time";

    for(int i = 0; i <= number_of_nodes; i++)
    {
        output << ",V" << i;
    }

    for(int i = 0; i < number_of_components; i++)
    {
        output << ",I" << components[i].get_number();
    }

    output << '\n';

    //each iteration solves the circuit at the next time step

    for(int step = 1; step <= steps && success; step++)
    {

        double time = step * dt;

        //clear the equations

        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j <= size; j++)
            {
                matrix[i][j] = 0;
            }
        }

        int source = 0;

        //build the equations using the previous component states

        for(int i = 0; i < number_of_components; i++)
        {

            int a = node[i][0] - 1;
            int b = node[i][1] - 1;

            int id = components[i].get_id();
            double value = components[i].get_value();

            if(id == 2)
            {

                //ideal voltage source: Va - Vb = value

                int k = number_of_nodes + source;

                if(a >= 0)
                {
                    matrix[a][k] += 1;
                    matrix[k][a] += 1;
                }

                if(b >= 0)
                {
                    matrix[b][k] -= 1;
                    matrix[k][b] -= 1;
                }

                matrix[k][size] = value;

                source++;

            }
            else
            {

                double conductance = 0;
                double history_current = 0;

                if(id == 1)
                {
                    conductance = 1.0 / value;
                }
                else if(id == 3)
                {

                    //capacitor: i = C / dt * (v - previous_voltage)

                    conductance = value / dt;
                    history_current = -conductance * state[i];

                }
                else if(id == 4)
                {

                    //inductor: i = previous_current + dt / L * v

                    conductance = dt / value;
                    history_current = state[i];

                }
                else if(id == 5)
                {

                    //approximate the switch as a resistance
                    //change this condition to schedule switching over time

                    bool closed = value != 0;

                    double resistance = closed ? 0.001 : 1e9;

                    conductance = 1.0 / resistance;

                }

                add_branch(matrix, size, a, b,
                           conductance, history_current);

            }

        }

        if(!solve(matrix, answer, size))
        {

            std :: cerr << "Cannot solve at time " << time
                        << ". Check floating nodes and voltage sources."
                        << std :: endl;

            success = false;
            break;

        }

        source = 0;

        //calculate currents, then update the stored states

        for(int i = 0; i < number_of_components; i++)
        {

            double va = node[i][0] == 0 ? 0 : answer[node[i][0] - 1];
            double vb = node[i][1] == 0 ? 0 : answer[node[i][1] - 1];

            double voltage = va - vb;

            int id = components[i].get_id();
            double value = components[i].get_value();

            if(id == 1)
            {
                current[i] = voltage / value;
            }
            else if(id == 2)
            {
                current[i] = answer[number_of_nodes + source];
                source++;
            }
            else if(id == 3)
            {

                current[i] = (value / dt) * (voltage - state[i]);

                state[i] = voltage;

            }
            else if(id == 4)
            {

                current[i] = state[i] + (dt / value) * voltage;

                state[i] = current[i];

            }
            else if(id == 5)
            {

                bool closed = value != 0;

                double resistance = closed ? 0.001 : 1e9;

                current[i] = voltage / resistance;

            }

        }

        //write this time step to the CSV file

        output << time << ",0";

        for(int i = 0; i < number_of_nodes; i++)
        {
            output << "," << answer[i];
        }

        for(int i = 0; i < number_of_components; i++)
        {
            output << "," << current[i];
        }

        output << '\n';

        if(!output)
        {
            std :: cerr << "Cannot write simulation.csv." << std :: endl;
            success = false;
        }

    }

    output.close();

    if(success)
    {
        std :: cout << "Simulation saved to simulation.csv" << std :: endl;
    }
    else
    {
        std :: cerr << "Simulation did not complete." << std :: endl;
    }

    //free the allocated memory

    for(int i = 0; i < size; i++){ delete[] matrix[i]; }

    delete[] matrix;
    delete[] answer;
    delete[] current;
    delete[] state;

    for(int i = 0; i < number_of_components; i++){ delete[] node[i]; }

    delete[] node;
    delete[] components;

    return success ? 0 : 1;

}