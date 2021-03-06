/*********************************************************************
** Author: Bryce Bristow, Evin Colignon, & Romano Garza
** Date: 11/20/17
** Description: This is the main file for a program that takes a .txt
** input file argument when ran. Within the input file, each line has 
** 3 numbers denoting an integer identifier, an x coordinate, and a 
** y coordinate. The file input is stored in a vector. It is then used 
** to determine a solution to the traveling saleman problem given the 
** locations and coordinates. The alogrithm used is the Nearest-
** Neighbor Algorithm.
*********************************************************************/
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream> 
#include <cstdlib>
#include <ctime>
#include <limits>
#include <string.h>
#include <math.h> 
#include <time.h>
#include <chrono>


struct tsp_coordinate 
{
	int identifier; 	// first integer on .txt file line
	int x_coordinate;   // second integer on .txt file line
	int y_coordinate; 	// third integer on .txt file line
}; 

struct solution
{
    std::vector<struct tsp_coordinate> path;
    int full_distance;
};

// prototype for TSP vector generator
void generate_tsp_vector(char* file_name, std::vector<struct tsp_coordinate> &v);
// prototype for determining nearest neighbour route
struct solution get_solution(std::vector<struct tsp_coordinate> v);
// prototype for getting distance between two coordinates
int get_distance(struct tsp_coordinate place_a, struct tsp_coordinate place_b);
// prototype for saving solution into text file
int save_solution(struct solution tsp_solution, char file_name[]);
// prototype for displaying the solution in console
void display_solution(struct solution tsp_solution);


int main(int argc, char **argv)
{
    // seed random
    srand(static_cast<unsigned int>(time(NULL)));
    
	// store the argument file name that was passed 
	// on the commmand line when the program
	// was run 
	char* argument_file_name = argv[1]; 
	// declare the vector for holding our coordinates
	std::vector<tsp_coordinate> tsp_vector; 
    
    // start clock for timer
    auto start = std::chrono::high_resolution_clock::now();
	
    //generate a tsp vector from the file name
	generate_tsp_vector(argument_file_name, tsp_vector); 
    


	//test generate_tsp_vector functionality
	/*for(int i= 0; i < tsp_vector.size(); i++)
	{
		std::cout << "ID: " << tsp_vector[i].identifier << std::endl; 
		std::cout << "X: " << tsp_vector[i].x_coordinate << std::endl;
		std::cout << "Y: " << tsp_vector[i].y_coordinate << std::endl;
		std::cout << std::endl << std::endl; 
	}*/
    
    //determine tsp solution via NN
    struct solution tsp_solution = get_solution(tsp_vector);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> fullRunTime = (end - start) * 1000;
    
    std::cout << "Full TSP run time for " << argument_file_name
    << " is " << fullRunTime.count() << "ms" << std::endl;
 


    //test: show solution
    // std::cout << "Distance: " << tsp_solution.full_distance << std::endl;
    // std::cout << "Path: " << std::endl;
    // for(int j = 0; j < tsp_solution.path.size(); j++)
    // {
    //     std::cout << tsp_solution.path[j].identifier << std::endl;
    // }
    
    if(strcmp(argument_file_name, "tsp_example_1.txt") == 0)
    {
        double ratio = (double)tsp_solution.full_distance/108159;
        std::cout << "Ratio: " << ratio << std::endl;
    }
    else if(strcmp(argument_file_name, "tsp_example_2.txt") == 0)
    {
        double ratio = (double)tsp_solution.full_distance/2579;
        std::cout << "Ratio: " << ratio << std::endl;
    }
    else if(strcmp(argument_file_name, "tsp_example_3.txt") == 0)
    {
        double ratio = (double)tsp_solution.full_distance/1573084;
        std::cout << "Ratio: " << ratio << std::endl;
    }

    // if saving the solution fails for some reason, output the
    // solution to the console
    if (save_solution(tsp_solution, argument_file_name) < 0)
    {
        display_solution(tsp_solution);
    }
    
	return 0;

}


/*********************************************************************
** Description: generate_tsp_vector opens the file name passed in, 
** it reads the file to add it to a passed vector of tsp_coordinates
*********************************************************************/
void generate_tsp_vector(char* file_name, std::vector<struct tsp_coordinate> &v)
{
	// declare variable that will hold file & line data 
	std::ifstream file(file_name); 
	std::string line; 
	
	// check for file open errors
	if(!file)
	{
		std::cout << "Error opening file" << file_name << std::endl; 
		return; 
	}

	// get lines and fill out the vector
	while(std::getline(file, line))
	{
		//declare an input string stream so we can move through the line
		std::istringstream iss(line); 
		//declare an integer to hold the current number and one to hold the count; 
		int n, count = 0; 
		//declare a new TSP struct; 
		tsp_coordinate current_coordinate; 

		while(iss >> n)
		{
			if(count == 0) // first number is identifier
			{			   // save as such 
				current_coordinate.identifier = n; 
			}
			else if(count == 1) // second number is x coordinate
			{					// save as such 
				current_coordinate.x_coordinate = n; 
			}
			else if(count == 2)  // third number is y coordinate
			{					 // save as such
				current_coordinate.y_coordinate = n; 
			}
			else
			{
				std::cout << "Error: Too many numbers on a line" << std::endl; 
				return; 
			}
			count++; 
		}

		// now we can save the new struct to the vector we 
		// passed in 
		v.push_back(current_coordinate);  
	}
	//close the file 
	file.close(); 
}


/*********************************************************************
** Description: get_solution takes a vector of tsp coordinates and 
** finds an optimal path and the full distance of the path for tsp
*********************************************************************/
struct solution get_solution(std::vector<struct tsp_coordinate> v)
{
    int max_try_solutions = v.size();

    /* This way we don't need to iterate through another array */
    // initialize the ultimate solution and set its distance as infinity
    struct solution ultimate_solution;
    ultimate_solution.full_distance = std::numeric_limits<int>::max();


    // vector to remove traversed coordinates from;
    std::vector<struct tsp_coordinate> copy_of_v;
    std::vector<struct tsp_coordinate> copy_of_v2;
    if(max_try_solutions >= 200){
        max_try_solutions = 200;
    }

    // init copy 1 of v;
    copy_of_v = v;

    // generate nearest neighbour solutions trying every coordinate as the start
    for(int j = 0; j < max_try_solutions; j ++)
    {
        // declare a solution struct
        struct solution tsp_solution;

/* No reason to grab a random index, we should just iterate through from the end to reduce time */
        // get the starting place
        int startIndex = copy_of_v.back().identifier;
        struct tsp_coordinate start = copy_of_v.back(); //start with the last element of copy_of_v
      
        // add the starting place to the solution path
        tsp_solution.path.push_back(start);
        // initialize tsp_solution distance to 0
        tsp_solution.full_distance = 0;
      

        // reinit the copy of v
        copy_of_v2 = v;
        // mark as traversed, by removing the element from copy_of_v2
        //swap element and the back element, to allow for constant time removal
        struct tsp_coordinate temp = copy_of_v2[startIndex];
        copy_of_v2[startIndex] = copy_of_v2.back();
        copy_of_v2.back() = temp;
        copy_of_v2.pop_back();

        // here, we'll loop through the vector of coordinates till all coordinates have
        // been traversed (removed)
        while(copy_of_v2.size() > 0)
        {
            // initialize the current smallest distance as infinity
            int smallest_distance = std::numeric_limits<int>::max();
            // declare variable to hold index mapping to smallest distance
            int sd_index = 0;
            int i;
            // for every coordinate check if distance is
            // less than smallest, update the two variables if so
            for(i = 0; i < copy_of_v2.size(); i++)
            {
                int curr_distance = get_distance(start, copy_of_v2[i]);
                
                if(curr_distance < smallest_distance)
                {
                    sd_index = i;
                    smallest_distance = curr_distance;
                }
            }
            // add the coordinate with the smallest distance to the solution path
            tsp_solution.path.push_back(copy_of_v2[sd_index]);
            // add the solution distance to the current solution distance
            tsp_solution.full_distance = tsp_solution.full_distance + smallest_distance;
            // set the coordinate with the smallest distance as the new starting place
            start = copy_of_v2[sd_index];

            // mark it as traversed by removing it from v2
            copy_of_v2.erase(copy_of_v2.begin() + sd_index);
        }

        // calculate the distance back to start
        int last_distance = get_distance(tsp_solution.path[0], tsp_solution.path[tsp_solution.path.size() - 1]);
        tsp_solution.full_distance = tsp_solution.full_distance + last_distance;
        
        /* This way we don't need to iterate through another array */
        // if the current solution is shorter than the previous best
        if (tsp_solution.full_distance < ultimate_solution.full_distance)
        {
            ultimate_solution = tsp_solution;
            //display_solution(tsp_solution);
        }

        // mark solution as attempted by removing from v
        copy_of_v.pop_back();
    }

    // return the nearest neighbour path with the smallest distance
    return ultimate_solution;
}



/*********************************************************************
** Description: get_distance takes to tsp_coordinates and returns 
** the distance between them
*********************************************************************/
int get_distance(struct tsp_coordinate place_a, struct tsp_coordinate place_b)
{
    // get x and y coordinates for each tsp_coordinate as doubles
    double x_a = static_cast<double>(place_a.x_coordinate);
    double x_b = static_cast<double>(place_b.x_coordinate);
    double y_a = static_cast<double>(place_a.y_coordinate);
    double y_b = static_cast<double>(place_b.y_coordinate);
    // get legs of the triangle
    double leg_x = x_a - x_b;
    double leg_y = y_a - y_b;
    
    // get the hypotenuse
    double result = hypot(leg_x, leg_y);
    // update rounded integer version
    int rounded = static_cast<unsigned int>(round(result));
    
    return rounded;
    
}

/*********************************************************************
** Description: save_solution outputs the given solution into a text
** file, appending ".tour" to the name of the input. The first line
** of the text file is the total distance travelled, and the rest of
** the lines give the order in which the cities were visited
** returns 1 on error, 0 on success
*********************************************************************/
int save_solution(struct solution tsp_solution, char file_name[])
{
    //append ".tour" to the file name
    strcat(file_name, ".tour");
    //create file with appended name
    std::ofstream file(file_name); 
    
    // check for file open errors
    if(!file)
    {
        std::cout << "Error opening file" << file_name << std::endl; 
        return 1; 
    }

    file << tsp_solution.full_distance << std::endl;

    for(int j = 0; j < tsp_solution.path.size(); j++)
    {
        file << tsp_solution.path[j].identifier << std::endl;
    }

    file.close();

    return 0;
}

/*********************************************************************
** Description: display_solution outputs the solution to the console,
** including labels.
*********************************************************************/
void display_solution(struct solution tsp_solution)
{
    std::cout << "Distance: " << tsp_solution.full_distance << std::endl;
    std::cout << "Path: " << std::endl;
    for(int j = 0; j < tsp_solution.path.size(); j++)
    {
        std::cout << tsp_solution.path[j].identifier << " ";
    }

    std::cout << std::endl;
}
