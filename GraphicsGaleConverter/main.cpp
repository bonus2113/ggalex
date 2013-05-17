#include <iostream>
#include <galefile.h>
#include <string>
#include <vector>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gdiplus.h>
#include <memory>
#include "PluginManager.h"
#include "StdUtils.h"
#include "GdiplusUtils.h"

const std::string ERROR_CODES[] =
{
	"File does not exist.",
    "File format is invalid.",
    "File can not be closed",
    "The address of gale object is invalid.",
    "Parameter is invalid."
};

const std::string USAGE = "Usage: <filename> <plugin>.";


int main(int argc, char** argv) {
	//Check argument count
	if(argc < 3) {
		std::cout << USAGE << std::endl;
		return -1;
	}

	//Try to open the .gal file (returns nullptr on error)
	LPVOID file = ggOpen(argv[1]);

	//File could not be opened
	if(!file) {
		//Output error (error codes specified in galefile_readme.txt)
		std::cout << "Error reading " << argv[1] << ": " << ERROR_CODES[ggGetLastError() - 1] << std::endl;
		return -1;
	}

	//Start up GDI+ (must be done before any Bitmap processing)
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	DWORD frameCount = ggGetFrameCount(file);

	//Vector to hold the single frame bitmaps
	std::vector<std::shared_ptr<Gdiplus::Bitmap>> bitmaps;

	//Get the dimensions of one frame
	int width = ggGetInfo(file, 5);
	int height = ggGetInfo(file, 6);

	for(size_t i = 0; i < frameCount; i++) {
		Gdiplus::Bitmap* newBitmap = nullptr;
		//Merge color and alpha channel, write HBITMAP to Bitmaps
		if(MergeToBitmap(ggGetBitmap(file, i, -1), ggGetAlphaChannel(file, i, -1), width, height, &newBitmap) == Gdiplus::Ok){
			bitmaps.push_back(std::shared_ptr<Gdiplus::Bitmap>(newBitmap));
		}
	}
	
	//Update frame count in case merging failed on some bitmaps
	frameCount = bitmaps.size();

	PluginManager pluginManager;
	// Try to start up the plugin manager
	if(pluginManager.Initialize(std::string(argv[2]))){

		//Use the loaded plugin to get the dimensions of the output file and the placements of the bitmaps
		int outWidth, outHeight;
		std::vector<Point> positions = pluginManager.GetPlacements(width, height, frameCount, outWidth, outHeight);
		
		//Create output Bitmap and Graphics to draw on it
		Gdiplus::Bitmap* target = new Gdiplus::Bitmap(outWidth, outHeight);
		Gdiplus::Graphics *g = Gdiplus::Graphics::FromImage(target);

		//Draw the single frames to the output Bitmap
		for(size_t i = 0; i < bitmaps.size(); i++) {
			int x = positions[i].X;
			int y = positions[i].Y;
			g->DrawImage(bitmaps[i].get(), x, y);
		}

		//Save output with the same name as the input and as png
		std::string filename = argv[1];
		filename = filename.substr(0, filename.length() - 3);
		SaveToFile(target, (LPTSTR)ToWideString(filename + ".png").c_str());

		delete target;
		target = nullptr;

		delete g;
		g = nullptr;
	}

	//Cleanup
	pluginManager.Shutdown();

	bitmaps.clear();
	
	ggClose(file);
	Gdiplus::GdiplusShutdown(gdiplusToken);

	std::cout << "written frames" << std::endl;
	return 0;
}