Neural Network using Pthreads in C++

This project implements a prediction model that simulates the neural network using pthreads in C++. Each neuron is represented as a separate pthread, enabling parallelized computation and eliminating the need for mutex locks. The model is optimized using vectors for efficient data handling and supports dynamic neural network configurations.
Features

    Neural Network Simulation: Implements a multi-layer neural network forward pass.
    Parallel Computation: Uses pthreads to represent neurons, enabling parallel processing of neurons in each layer.
    No Mutex Locks: Achieves parallelization without the need for mutex locks, ensuring faster and more efficient execution.
    Vector Optimization: Utilizes vectors for efficient data storage and manipulation, improving performance.
    Randomized Layer Configuration: Dynamically creates neural network layers and neurons with random configurations for robust testing.
    Batch Processing: Implements CPU affinity to distribute threads across CPU cores for better performance.

Technologies Used

    C++: Core programming language used for implementation.
    Pthreads: Used for parallelization and thread management.
    Vectors: Utilized for optimized data storage and access.
    File I/O: Reads weights and input values from a file for initializing the neural network.
    Named Pipes: Used for communication between threads of one layer with next layer

Execution Flow

    Initialization:
        Read weights and input values from the file os.txt.
        Generate a random number of layers and neurons per layer.

    Layer Creation:
        Create layers and neurons recursively using the create_layers function.
        Each neuron is represented by a pthread, which performs computations based on the input values and weights.
        Use pipes to handle communication between neurons of different layers.

    Forward Pass:
        Perform the forward pass of the neural network, with each layer computing the values for the next layer.
        The final layer computes specific functions based on the neuron's output values.

    Backtracking:
        After completing the forward pass, the model backtracks using the computed values for a second pass to verify the results.

Getting Started

Clone the repository:

    git clone https://github.com/Muhammad-Hamza-cpp/Pthread-Neural-Network.git

Navigate to the project directory:

    cd Pthread-Neural-Network

Compile the project:

    g++ -o neural_network Neural network.cpp -lpthread

Run the executable

    ./neural_network

