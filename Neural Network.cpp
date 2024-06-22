#include"iostream"
# include <unistd.h>
# include <sys/types.h>
# include <errno.h>
# include <stdio.h>
# include <sys/wait.h> 
#include<stdlib.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include "cstring"
#include "string"
#include "ctime"
#include "random"
#include <sched.h>
using namespace std;
std::string filename = "os.txt"; //FILE NAME
std::vector<std::vector<double>> inputLayer; //INPUT LAYER WEIGHTS
std::vector<std::vector<double>> inputvals; //INPUT LAYER values
std::vector<std::vector<double>> hiddenLayer1; //LAYER1 LAYER WEIGHTS
std::vector<std::vector<double>> hiddenLayer2;//LAYER2 LAYER WEIGHTS
std::vector<std::vector<double>> hiddenLayer3;//LAYER3 LAYER WEIGHTS
std::vector<std::vector<double>> hiddenLayer4;//LAYER4 LAYER WEIGHTS
std::vector<std::vector<double>> hiddenLayer5;//LAYER5 LAYER WEIGHTS
std::vector<std::vector<double>> hiddenLayer6;//LAYER6 ELAYER WEIGHTS
std::vector<std::vector<double>> hiddenLayer7;//LAYER7 ELAYER WEIGHTS
std::vector<std::vector<double>> hiddenLayer8;//LAYER8 ELAYER WEIGHTS
std::vector<std::vector<double>> OutputLayer;//OUTPUT LAYER WEIGHTS
int layer_count=0; //count of number of layers
int *layer_neurons; //array corresponding to number of layers per 
bool second_time=false; //flag for second iteration
std::vector<std::vector<double>> read_data;  //2D vector to read data from File
std::vector<double> pipeReadEnds; //stores read ends of pipies
std::vector<double> pipeWriteEnds; //stores write ends of pipes
int returnpipe[2]; //pipe for bactracking
vector<std::vector<double>> returnmatrix(int layer_index)//Returns weights based on layer number
{
    if(layer_index==1)
    return hiddenLayer1;
    else if (layer_index==2)
    return hiddenLayer2;
    else if (layer_index==3)
    return hiddenLayer3;
    else if (layer_index==4)
    return hiddenLayer4;
    else if (layer_index==5)
    return hiddenLayer5;
    else
    return OutputLayer;
}
std::vector<double> parseLine(const std::string& line) //line parser
{
    std::vector<double> values;
    std::istringstream iss(line);
    std::string token;
    while (std::getline(iss, token, ',')) {
        values.push_back(std::stod(token));
    }
    return values;
}

// Function to read the text file and store matrices in separate 2D double vectors
void readMatricesFromFile(const std::string& filename, std::vector<std::vector<double>>& inputLayer,
                           std::vector<std::vector<double>>& hiddenLayer1, std::vector<std::vector<double>>& hiddenLayer2,
                           std::vector<std::vector<double>>& hiddenLayer3, std::vector<std::vector<double>>& hiddenLayer4,
                           std::vector<std::vector<double>>& hiddenLayer5,std::vector<std::vector<double>>& OutputLayer,std::vector<std::vector<double>>& inputvalues,
                           std::vector<std::vector<double>>& hiddenLayer6,std::vector<std::vector<double>>& hiddenLayer7,std::vector<std::vector<double>>& hiddenLayer8) //reads weights from file
                            {
    std::ifstream file(filename);

    std::vector<std::vector<double>>* currentMatrix = nullptr;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            // Reset the current matrix pointer
            currentMatrix = nullptr;
        } else if (line == "Input layer weights") {
            currentMatrix = &inputLayer;
        } else if (line == "Hidden layer 1 weights") {
            currentMatrix = &hiddenLayer1;
        } else if (line == "Hidden layer 2 weights") {
            currentMatrix = &hiddenLayer2;
        } else if (line == "Hidden layer 3 weights") {
            currentMatrix = &hiddenLayer3;
        } else if (line == "Hidden layer 4 weights") {
            currentMatrix = &hiddenLayer4;
        } else if (line == "Hidden layer 5 weights") {
            currentMatrix = &hiddenLayer5;
        } else if (line == "Hidden layer 6 weights") {
            currentMatrix = &hiddenLayer6;
        } else if (line == "Hidden layer 7 weights") {
            currentMatrix = &hiddenLayer7;
        } else if (line == "Hidden layer 8 weights") {
            currentMatrix = &hiddenLayer8;
        } else if (line == "Output layer weights") {
            currentMatrix = &OutputLayer;
        } else if (line == "Input data") {
            currentMatrix = &inputvalues;
        } else if (currentMatrix) {
            // Parse the line and append the values to the current matrix
            std::vector<double> values = parseLine(line);
            currentMatrix->push_back(values);
        }
    }
}
class Neuron   //neuron class
{
public:
double value=0; //neurons output value
int index=0;  //index of neuron
int previous_layer_neurons=0;  //previous layer neurons (used for summing up)
int layer=0; //layer number 
};
int generateRandomNumber(int lowerBound, int upperBound) //generates random number for dynamic layer,neurons
{
    // Seed the random number generator with the current time
    std::random_device rd;
    std::mt19937 rng(rd());
    // Create a uniform distribution in the given range
    std::uniform_int_distribution<int> distribution(lowerBound, upperBound);
    // Generate and return a random number
    return distribution(rng);
}
void* neuronFunc(void* a) //thread function for each layer
{
    Neuron &obj=*((Neuron*)a);
    int coreId=obj.index%4;
    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet);
    CPU_SET(coreId, &cpuSet);  
    sched_setaffinity(0, sizeof(cpuSet), &cpuSet); //batch processing
    if(obj.layer!=0) //if input layer then donot sum 
    {
    for(int i=0;i<obj.previous_layer_neurons;i++)
    {
        obj.value+=read_data[i][obj.index];
    }
    }
    cout << "I'm neuron having index:"<<obj.index <<" and output Value: "<<obj.value<< endl;
    pthread_exit(NULL);
    return (void*)&obj;
}
vector<pthread_t> neuron;  //neuron threads
vector<Neuron> neuron_obj; //neuron objects
void create_layers(int layer)
{
    if (layer == layer_count)  //if all layers are created then return
        return;
    else
    {
        pid_t p = fork();
        if (p == 0) //child process
        {
            int this_neurons=layer_neurons[layer];  //current layer neurons
            neuron_obj.resize(this_neurons); 
            neuron.resize(this_neurons);
            int prev_neurons=layer_neurons[layer-1];  //previous layer neurons
            int Next_neurons=layer_neurons[layer+1];  //next layer neurons 
            if(layer==0)  //input layer
            {
             for(int a=0;a<inputvals[0].size();a++)
             neuron_obj[a].value=inputvals[0][a];   
             for(int i=0;i<this_neurons;i++) // create pipe for each neuron in current layer 
                 {
                     int pipefd[2];
                     pipe(pipefd);
                     pipeReadEnds.push_back(pipefd[0]);  
                     pipeWriteEnds.push_back(pipefd[1]);
                 for(int j=0;j<Next_neurons;j++)  //generate number of next layer neurons values
                     {
                        vector<std::vector<double>>& currentMatrix=inputLayer;
                         std::string data = to_string(currentMatrix[i][j]*neuron_obj[i].value); //multiplying neuron value with weihgts 
                         write(pipeWriteEnds[i], data.c_str(), data.size()); //writing calculated value to pipe
                         write(pipeWriteEnds[i], "\n", 1);
                     }
                    close(pipeWriteEnds[i]);
                 }
            }
            else   //for other layers not layer1;
            {
                read_data.resize(prev_neurons,vector<double>(this_neurons));
                  for(int ab=0;ab<prev_neurons;ab++)   //read values from pipes and store it in a 2d vector
                  {
                       char buff2[200];
                       read(pipeReadEnds[ab],buff2,sizeof(buff2));
                       close(pipeReadEnds[ab]);
                        char* token = strtok(buff2, "\n");
                        char * endptr;
                        int count=0;
                       while (token != nullptr ) 
                       {
                       double value = std::strtod(token,&endptr);
                        read_data[ab][count]=value;  //read data and store in a 2d vector
                        count++;      
                       token = strtok(nullptr, "\n");
                       }
                 }
            cout<<endl<<"DISPLAYING MATRIX"<<endl;
            for(int i=0;i<prev_neurons;i++) //display the vector 
            {
                for(int j=0;j<layer_neurons[layer];j++)
                cout<<read_data[i][j]<<" ";
                cout<<endl;
            }
            
            }
            cout<<"\nIM LAYER:"<<layer+1<<" having Neurons:"<<layer_neurons[layer]<<endl;
            for (int i = 0; i < this_neurons; i++)  //create threads for current layer neurons
            {
                neuron_obj[i].index=i;
                neuron_obj[i].previous_layer_neurons=prev_neurons;
                neuron_obj[i].layer=layer;
                pthread_create(&neuron[i], NULL, neuronFunc,(void*)&neuron_obj[i]);
            }
            for(int i=0;i<this_neurons;i++) 
            pthread_join(neuron[i],NULL);
            if(layer!=0&&layer!=layer_count-1) //if layer is not input and not last layer then generarte values   
            {
             vector<std::vector<double>>currentMatrix=returnmatrix(layer);   
             pipeReadEnds.clear();
             pipeWriteEnds.clear();
                for(int i=0;i<this_neurons;i++)
                 {
                     int pipefd[2];
                     pipe(pipefd);
                     pipeReadEnds.push_back(pipefd[0]);
                     pipeWriteEnds.push_back(pipefd[1]);
                 for(int j=0;j<Next_neurons;j++)
                     {
                         std::string data = to_string(currentMatrix[i][j]*neuron_obj[i].value);
                         write(pipeWriteEnds[i], data.c_str(), data.size());
                         write(pipeWriteEnds[i], "\n", 1);
                     }
                    close(pipeWriteEnds[i]);
                 }
            }
            if(layer==layer_count-1 && !second_time)  //if last layer is achieved
            {
                char buffer[100];
                 double val1=(neuron_obj[0].value*neuron_obj[0].value+neuron_obj[0].value+1)/2; //calculate values using function1
                 double val2=(neuron_obj[0].value*neuron_obj[0].value-neuron_obj[0].value)/2;  //calculate values using function2
                string result=to_string(val1)+" "+to_string(val2);
                cout<<"\nIm layer "<<layer+1<<" and calculated values are "<<result<<endl;
                strcpy(buffer,result.c_str());
                 pipeReadEnds.clear();
                 pipeWriteEnds.clear();
                pipeReadEnds.push_back(returnpipe[0]);
                pipeWriteEnds.push_back(returnpipe[1]);
                write(pipeWriteEnds[0],buffer,sizeof(buffer)); //write into return pipe for backtracking
            }
        if(layer!=layer_count-1)
        {    
        pipeReadEnds.clear();
        pipeWriteEnds.clear();    
        }
        neuron.clear();
        neuron_obj.clear();
        for (auto& row : read_data) {
        row.clear();
            }
        read_data.clear();
        create_layers(++layer); //recursively create second layer
        if(!second_time)
        {
        char reading_buffer[200];
        read(returnpipe[0],reading_buffer,sizeof(reading_buffer)); 
        cout<<"\nBack track layer:"<<layer<<" having data "<<reading_buffer<<endl;   //backtracking 
        write(returnpipe[1],reading_buffer,sizeof(reading_buffer)); 
        }
        }
        else
        {
        wait(NULL);  
        exit(0);
        }
    }
}
int main()
{    
    readMatricesFromFile(filename, inputLayer, hiddenLayer1, hiddenLayer2, hiddenLayer3, hiddenLayer4, hiddenLayer5,OutputLayer,inputvals,hiddenLayer6,hiddenLayer7,hiddenLayer8);
    int numProcessors = sysconf(_SC_NPROCESSORS_ONLN);
    pipe(returnpipe); //create return pipe 
    layer_count=generateRandomNumber(3, 8);
    layer_neurons=new int [layer_count];
    layer_neurons[0]=2;
    layer_neurons[layer_count-1]=1;
    for(int i=1;i<layer_count-1;i++)
    {
        layer_neurons[i]=generateRandomNumber(2, 8);
    }
    cout<<"Layers:"<<layer_count<<endl;
    cout<<"Neurons:";
    for(int i=0;i<layer_count;i++)
    {
        cout<<layer_neurons[i]<<" ";
    }
    cout<<endl;
    create_layers(0);
    cout<<"**************************RESTARTING USING CALCULATED VALUES********************\n  ";
    char reading_buffer[200];
    read(returnpipe[0],reading_buffer,sizeof(reading_buffer)); 
    std::stringstream ss(reading_buffer);
    ss >> inputvals[0][0] >> inputvals[0][1];
    second_time=true;
    create_layers(0);
    cout<<"**************************SECOND PASS COMPLETED********************************  ";
    return 0;
}