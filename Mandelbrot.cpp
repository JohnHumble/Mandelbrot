// Mandelbrot.cpp : Defines the entry point for the console application.
//

#include "stdafx.h" // used because of visual studio console app code
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Color
{
	int red;
	int green;
	int blue;
};

struct MandelbrotConfig
{
	int width;
	int height;
	double xComplexMin;
	double xComplexMax;
	double yComplexMin;
	double yComplexMax;
	int maxIterations;
	Color colorOne;
	Color colorTwo;
	string outputFileName;
};

// structs
struct ColorRamp
{
	int iterations;
	int numberOfColors;
	int colorIndex[16];
	Color colors[16];
};

// prototype functions
MandelbrotConfig readConfig(string configFileLocation);
void drawMandelbrot(MandelbrotConfig config, ColorRamp colorRamp);
Color getPixelColor(ColorRamp colorRamp, MandelbrotConfig Config, int rawIndex);
int iterate(int min, int max, int iteration, int maxIteration);
ColorRamp getColorRamp(string fileName);
bool readNewConfig(string configFileLocation, MandelbrotConfig &config, ColorRamp &colorRamp);


int main()
{
  bool more = true;
  while (more)
  {
      // Read in config file locations from user
      string mConfigLocation = "";
      cout << "Enter the mandelbrot Configeration file name :";
      cin >> mConfigLocation;

      if (mConfigLocation == "exit")
      {
          more = false;
          break;
      }

      // Read config file contents into MandelbrotConfig struct instance
      MandelbrotConfig mConfig; // = readConfig(mConfigLocation);
      ColorRamp colorRamp; // = getColorRamp(colorRampLocation);
      if (readNewConfig(mConfigLocation, mConfig, colorRamp))
      {
          // Compute and write specified mandelbrot image to PPM file
          drawMandelbrot(mConfig, colorRamp);
      }
  }
	return 0;
}

MandelbrotConfig readConfig(string configFileLocation)
{
	//define the MandelbrotConfig struct to return.
	MandelbrotConfig mConfig;

	// open the file.
	ifstream fin;
	fin.open(configFileLocation);

	//read in the file contents.
	fin >> mConfig.width;
	fin >> mConfig.height;
	fin >> mConfig.xComplexMin;
	fin >> mConfig.xComplexMax;
	fin >> mConfig.yComplexMin;
	fin >> mConfig.yComplexMax;
	fin >> mConfig.maxIterations;
	fin >> mConfig.colorOne.red;
	fin >> mConfig.colorOne.green;
	fin >> mConfig.colorOne.blue;
	fin >> mConfig.colorTwo.red;
	fin >> mConfig.colorTwo.green;
	fin >> mConfig.colorTwo.blue;
	fin >> mConfig.outputFileName;

	//Close file
	fin.close();

	//Return MandelbrotConfig
	return mConfig;
}

MandelbrotConfig readNewConfig(string configFileLocation)
{
  MandelbrotConfig mConfig;

  // open the file.
  ifstream fin;
  fin.open(configFileLocation);

  //read in the file contents.
  
  return mConfig;
}

bool readNewConfig(string configFileLocation, MandelbrotConfig &config, ColorRamp &colorRamp)
{
  double radius = 0;
  ifstream file;
  file.open(configFileLocation);

  if (file.is_open())
  {
      // load mandelbrotConfig values
      file >> config.width;
      file >> config.height;
      file >> config.xComplexMin;
      file >> config.yComplexMin;
      file >> radius;
      file >> config.maxIterations;
      file >> config.outputFileName;
      file >> colorRamp.iterations;
      file >> colorRamp.numberOfColors;
      if (colorRamp.numberOfColors > 16) // test to make sure the nubmer of colors is in the array;
      {
          cout << "the number of colors cannot be more than 16" << endl;
          cout << "seting the number of colors to 16" << endl;
          colorRamp.numberOfColors = 16;
      }
      for (int i = 0; i < colorRamp.numberOfColors; i++) // load in all of the colors;
      {
          file >> colorRamp.colors[i].red; // load in first 3 sets of numbers as a RGB color values.
          file >> colorRamp.colors[i].green;
          file >> colorRamp.colors[i].blue;
          file >> colorRamp.colorIndex[i]; // load in the last number as a location.
      }
      file.close();
  

    // find other complex X and Y values;
    config.yComplexMax = config.yComplexMin + radius;
    double offsetValue = (double)config.width / (double)config.height;
    config.xComplexMax = config.xComplexMin + radius * offsetValue;
    return true;
  }
  else
  {
      cout << "File could not be loaded" << endl;
      return false;
  }
}

// Loads in the values for the ColorRamp.
ColorRamp getColorRamp(string fileName)
{
	ColorRamp colorRamp;

	ifstream fin;
	fin.open(fileName);

	// load in number of iterations from the first line of the file.
	fin >> colorRamp.iterations;
	// load in the number of colors form the second line of the file.
	fin >> colorRamp.numberOfColors;

	// check to make sure the number of colors is not more than 16.
	if (colorRamp.numberOfColors > 16)
	{
		cout << "the number of colors cannot be more than 16" << endl;
		cout << "seting the number of colors to 16" << endl;
		colorRamp.numberOfColors = 16;
	}

	// load in all of the colors and their indexes.

	for (int i = 0; i < colorRamp.numberOfColors; i++)
	{
		// load in first 3 sets of numbers as a RGB color values.
		fin >> colorRamp.colors[i].red;
		fin >> colorRamp.colors[i].green;
		fin >> colorRamp.colors[i].blue;

		// load in the last number as a location.
		fin >> colorRamp.colorIndex[i];
	}

	//close the file.
	fin.close();

	return colorRamp;
}

void drawMandelbrot(MandelbrotConfig config, ColorRamp colorRamp)
{
	// adust the max index to match the color iterations.
	int maxIteration = config.maxIterations;

	// compute pixel size within configured view
	//start by getting the difference form the max and min view values
	double xComplexDiff = config.xComplexMax - config.xComplexMin;
	double yComplexDiff = config.yComplexMax - config.yComplexMin;

	// then use the difference values to get the pixel size values.
	double pixelWidth = xComplexDiff / config.width;
	double pixelHeight = yComplexDiff / config.height;

	// initialize the output file.
	ofstream fout;
	fout.open(config.outputFileName);

	// write file header.
	fout << "P3" << endl;
	fout << config.width << " " << config.height << endl;
	fout << "255" << endl;

	// use a nested for loop to get the color values for each pixel
	for (int col = 0; col < config.height; col++)
	{
		for (int row = 0; row < config.width; row++)
		{
			// scale pixel corrdinates to lie inside the complex Mandelbrot.
			// gets the pixel location and initial value.
			double x0 = config.xComplexMin + row * pixelWidth;
			double y0 = config.yComplexMin + col * pixelHeight;

			// initialize importat variables for the calculation.
			double x = 0.0;     // x value
			double y = 0.0;     // y value
			int iteration = 0;  // iteration value uesed as an index

			while (x*x + y*y < 2 * 2 && iteration < maxIteration)
			{
				double xtemp = x*x - y*y + x0;  // temperately stores the new x value
				y = 2 * x * y + y0;
				x = xtemp;
				iteration++;
			}

			Color pixelColor;
			// use function for the pixel color
			pixelColor = getPixelColor(colorRamp, config, iteration);

			// write out color on the file.
			fout << left << setw(4) << pixelColor.red;
			fout << left << setw(4) << pixelColor.green;
			fout << left << setw(6) << pixelColor.blue;
		}

		fout << endl;

        // 
    if (col % 100 == 0)
    {
      cout << endl;
      cout << "complete line " << col;
    }
    cout << ".";
    if (col == config.height - 1)
    {
        cout << endl;
        cout << "~~ Render Finished ~~" << endl;
        cout << endl;
    }
	}

	// close file
	fout.close();
}

// now this uses the color ramp to get the colors.
Color getPixelColor(ColorRamp colorRamp, MandelbrotConfig config, int rawIndex)
{

	Color pixelColor = colorRamp.colors[0];

	//seperate the different ranges in the colorRamp and test where the index is.
	int index;
	double rawIndexLocation = static_cast<double>(rawIndex) / config.maxIterations;

	index = rawIndexLocation * colorRamp.iterations;

	for (int i = 1; i < colorRamp.numberOfColors; i++)
	{
		// test to see if the index is within the range of colors[i-1] and colors[i] and if so calcuate the color value.
		if (rawIndex >= colorRamp.colorIndex[i - 1] && index < colorRamp.colorIndex[i])
		{
			pixelColor.red = iterate(colorRamp.colors[i - 1].red, colorRamp.colors[i].red, index, colorRamp.iterations);
			pixelColor.green = iterate(colorRamp.colors[i - 1].green, colorRamp.colors[i].green, index, colorRamp.iterations);
			pixelColor.blue = iterate(colorRamp.colors[i - 1].blue, colorRamp.colors[i].blue, index, colorRamp.iterations);

			break;
		}
	}

	if (rawIndex == config.maxIterations)
	{
		pixelColor = colorRamp.colors[colorRamp.numberOfColors - 1];
	}

	return pixelColor;
}

int iterate(int min, int max, int iteration, int maxIteration)
{
	int output;
	// get the range and size of each iteration.
	int range = max - min;
	double size = static_cast<double>(range) / maxIteration;

	// assign the output as the iteration value;
	output = iteration * size + min;

	return output;
}