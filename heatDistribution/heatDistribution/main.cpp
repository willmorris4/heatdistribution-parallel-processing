#include <iostream>
#include <fstream>
#include <math.h>
#include <iomanip>
#include <vector>
#include <time.h>
#include <unistd.h>
#include <string>

using namespace std;

/*
* Declarations:
*/
//------------------------------------------
// const int columnInputX(3), rowInputY(3);
// double xyGrid[columnInputX][rowInputY];

std::vector<std::vector<int>> r;
std::vector<std::vector<int>> g;
std::vector<std::vector<int>> b;

//Declare an empty 2D array with size determined by use^I
//Make 3 global vectors r,g,b.
//Make them point to the size of row and column
//Also declare row and colum here but as empty. Must be defined in main method.

//------------------------------------------
/*
*Function Declarations:
*/
//TODO: What is an easier/ less performance consumptive way of declaring row and column here? N.B: they iterate over.
//I want to just declare it and then point to its size when iterating over it in methods...
int main(int argc, const char * argv[]);
void generate(unsigned const int& rows, unsigned const int& columns, std::vector< std::vector<double> >& vec);
void init(int rows, int columns);
void fill(std::vector< std::vector<double> >& vec);
void hDOnePro(std::vector< std::vector<double> > &vec, const double &threshold, string fname);
void hDFourPro();
void writePPM(int row, int column);
void calculateRGB(int row, int column, std::vector< std::vector<double> >& xyGrid);
void print(std::vector< std::vector<double> >& xyGrid, ofstream &file);


//------------------------------------------
/*
*Initialise all vectors: temperature, r,g,b from row and column size:
 * 1: unsigned to avoid negative row and col values
 * 2: const values to protect inadvertent modification
 * 3: All parameters are pass-by-reference to avoid any unnecessary overhead from cloning
*/
void generate(unsigned const int& rows, unsigned const int& columns, std::vector< std::vector<double> >& vec)
{
  vec.resize(rows, std::vector<double>(columns, 0));
  r.resize(rows, std::vector<int>(columns,0));
  g.resize(rows, std::vector<int>(columns,0));
  b.resize(rows, std::vector<int>(columns,0));
  //Fill temperature vector with boundary values and 0s.
  fill(vec);

}


//------------------------------------------
/*
*Fill temp boundaries:
*/
void fill(std::vector< std::vector<double> >& vec) {

  for (int r = 0; r < vec.size(); r++) {
    for (int c = 0; c < vec[r].size(); c++) {
      if (r == 0) {
        vec[r][c] = 0;
      } else if (c == 0 || c == vec[r].size() - 1) {
        vec[r][c] = 100;
      } else if (r == vec.size() - 1) {
        vec[r][c] = 100;
      }//end boundary fill
      else {
        //fill all inner nodes with 0s.
        vec[r][c] = 0;
      }
    }
  }
}

//------------------------------------------
/*
* Serial Program:
*/
void hDOnePro(std::vector< std::vector<double> >& vec,const double &threshold, string fname){

  /*File .txt output in format:
   * #FileNumber_MxN_threshold
   * */
  ofstream txtFile(fname + ".txt");

  //Calculation parameters
  double difference, addedValues, curTemp;
  double accumDifference = 0; double storedDiff = 0;
  double diff = 1;
  double dynamicRange;
  int ic = -1;

  //Clock ticks/seconds
  double hdOnePro_TICKS_AND_SECONDS_start = clock();

  do {
    ic++;
    for(int r = 0; r < vec.size(); r++){
      for(int c = 0; c <vec[r].size(); c++){

        //Reference the temperature for comparison later:
        curTemp = vec[r][c];

        //Calculate values inside boundary:
        if((r != 0) && (r != vec.size()-1)) {
          if((c != 0) && (c != vec[r].size()-1)){
            //Equation split up into a 'divide and conquer' algorithm
            addedValues = ((vec[r-1][c])+
                    (vec[r][c+1])+
                    (vec[r+1][c])+
                    (vec[r][c-1])); //Steady state heat equation.

            vec[r][c] = (0.25)*addedValues;

            difference = vec[r][c] - curTemp;

            //Accumulative difference
            accumDifference+=difference;
          }
        }
        cout<<"[" << setw(8)<< vec[r][c] << "]";
      } // End Column
      // usleep(10000);
      cout << endl;
    } // End Row
    //Store last difference calculation into a temporary variable
    storedDiff = diff;
    // Calculation of difference
    diff = accumDifference/(vec.size()*(vec[0].size()));
    dynamicRange = diff - storedDiff;
    cout << "\n-------------------------------------------------\n"<<endl;

  } while(dynamicRange >= threshold);

  double hdOnePro_TICKS_AND_SECONDS_end = clock();
  print(vec, txtFile);
  std::cout << "One Processor: \nThreshold Level: " << threshold <<"\nFinal Dynamic Range to Break: " << dynamicRange <<"\nIteration Count: "<<ic<<"\nTicks: " << hdOnePro_TICKS_AND_SECONDS_end - hdOnePro_TICKS_AND_SECONDS_start << "\nSeconds: " << ((float)hdOnePro_TICKS_AND_SECONDS_end - hdOnePro_TICKS_AND_SECONDS_start)/CLOCKS_PER_SEC << "s" << std::endl;
  txtFile << "One Processor: \nThreshold Level: " << threshold <<"\nFinal Dynamic Range to Break: " << dynamicRange <<"\nIteration Count: "<<ic<<"\nTicks: " << hdOnePro_TICKS_AND_SECONDS_end - hdOnePro_TICKS_AND_SECONDS_start << "\nSeconds: " << ((float)hdOnePro_TICKS_AND_SECONDS_end - hdOnePro_TICKS_AND_SECONDS_start)/CLOCKS_PER_SEC << "s" << std::endl;
  txtFile.close();
}


void calculateRGB(std::vector< std::vector<double> >& vec){

  for (int j = 0; j < vec.size(); j++) {
    for (int i = 0; i < vec[j].size(); i++) {
      double f = vec[j][i]/100; //Get value between 0:1

      double a=(1-f)/0.25;	//invert and group
      int X=(int)floor(a);	//this is the integer part
      int Y=(int)floor(255*(a-X)); //fractional part from 0 to 255

      switch(X) {
        case 0:
          r[j][i]=255;g[j][i]=Y;b[j][i]=0;
              break;
        case 1:
          r[j][i]=255-Y;g[j][i]=255;b[j][i]=0;
              break;
        case 2:
          r[j][i]=0;g[j][i]=255;b[j][i]=Y;
              break;
        case 3:
          r[j][i]=0;g[j][i]=255-Y;b[j][i]=255;
              break;
        case 4:
          r[j][i]=0;g[j][i]=0;b[j][i]=255;
              break;
        default:
          cout<<"Error handling on RGB"<< endl;
      }
    }
  }
}

void writePPM(int row, int column){

  //TODO: Make a file that does appends and does not overwrite, and iterate over the file number
  ofstream img("tryout.ppm");
  img << "P3" << endl;
  img << 50 << " " << 50 << endl;
  img << "255" << endl; //maximum value of picture

  for (int j = 0; j < row; j++)
  {
    for (int i = 0; i < column; i++){
      //            cout<< "    [" << xyGrid[j][i] << "]";
      img <<" " << ((int)r[j][i])<< " " << ((int)g[j][i]) << " " <<((int)b[j][i]) << " ";
    }
    img << endl;
  }
  img.close();
}

void SWITCH(int i){

  switch(i){
    case 1: {
      const unsigned int row = 10;
      const unsigned int column = 10;
      const double threshold = 0.001;
      std::vector<std::vector<double> > v;
      generate(row, column, v);
      string fileString = "#01_10x10_0.001.txt";
      hDOnePro(v, threshold, fileString);
      break;
    }
    case 2: {
      cout << "somejlisfjg";
      break;
    }
    default: {
      cout << "ERROR: undefined number specified";
      break;
    }
  }

}

void print(std::vector< std::vector<double> >& vec, ofstream &file) {
  //Iterate over size of array instead.

  for (int r = 0; r < vec.size(); r++) {
    for (int c = 0; c < vec[r].size(); c++) {
      cout << "[" << setw(8) << vec[r][c] << "]";
      file << "[" << setw(8) << vec[r][c] << "]";

    }
    cout << endl;
    file<<endl;
  }
}


int main(int argc, const char * argv[]) {

  //TODO: Decide how to determine a variable tolerence
  //TODO: Make the col and row ints small and run it on this computer.

  /*
  * pass by value will make a copy of the argument into the function parameter. In many cases,
  * this is a needless performance hit, as the original argument would have sufficed.
  */
  SWITCH(2);




  //C++ guarantees that the destructor of v will be called when the method executes.
  // The destructor of std::vector will ensure any memory it allocated is freed.
  // As long as the T type of the vector<T> has proper C++ deallocation semantics all will be well.


//  for(int r = 0; r < v.size(); r++){
//    for(int c = 0; c < v[r].size(); c++){
//      cout<<"[" << setw(8)<< v[r][c] << "]";
//    }
//    cout<<endl;
//  }
//  calculateRGB(row, column, v);
//  writePPM(row, column);

}
