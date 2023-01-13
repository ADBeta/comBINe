/*******************************************************************************
* This file is part of psx-comBINe. Please see the github:
* https://github.com/ADBeta/psx-comBINe
*
* psx-comBINe is a simple program to combine multiple .bin files into a single
* file, and modified the .cue file indexing, this is ideal for PSX/PS1 CD-ROMs
* e.g. Rayman to get them ready for cue2pops or some emulators. I also find it 
* improves reliabilty when buring to a disk to only have one .bin file.
*
* (c) ADBeta
* v0.12.12
* 13 Jan 2023
*******************************************************************************/
#include <boost/filesystem.hpp>
#include <iostream>


#include <fstream>


#include <vector>
#include <string>

#include "helpers.h"
#include "TeFiEd.h"
#include "binHandler.h"
#include "cueHandler.h"

//Input and output .cue file TeFiEd pointers. Global to all modules
TeFiEd *cueFileIn, *cueFileOut;


//Vector of filenames pulled from the cueFile. Global to all modules.
std::vector<std::string> binFilenameVect;



//Strings used for input/output file system and directory management
std::string baseDirStr, baseFileStr, outDirStr;

//Generate the file system strings from input file (argv[1])
void genFSStrings(const std::string inFile) {
	//Find the last / in the in file, to split it into baseDir and baseFile
	size_t lastSlash = inFile.find_last_of('/');
	
	//Split from 0 to / to get directory
	baseDirStr = inFile.substr(0, lastSlash + 1);
	
	//Append psx-comBINe to the input string for output path
	outDirStr = baseDirStr + "psx-comBINe/";
	
	//Two stage process - First split from the end to the / of the inFile string
	//then substring from 0 to the first . of that string. This is done to 
	//prevent "./dirctory" or "/directory/file.a.b.c" from breaking the string.
	baseFileStr = inFile.substr(lastSlash + 1, inFile.length());
	baseFileStr = baseFileStr.substr(0, baseFileStr.find('.'));	
}

int main(int argc, char *argv[]){
	/** Get user args *********************************************************/
	//Test the user input is correct ** TODO
	if(argc == 1) {
		std::cout << "not enough args" << std::endl;
		return 1;
	}
	
	/** Setup *****************************************************************/
	
	//TODO validate if argv[1] is a filename
	//If argv[1] is a valid file, create a new TeFiEd using that filename, and
	//assign it to cueFile
	cueFileIn = new TeFiEd(argv[1]);
	//Set safety flag on the .cue input file. 100KB
	cueFileIn->setByteLimit(102400);
	
	cueFileIn->setVerbose(true);
	//Read the .cue file in
	if(cueFileIn->read() != 0) return 1;
	
	//Generate the file system strings for use later TODO Make this default behavious with overwrite
	genFSStrings((std::string)argv[1]);
	
	
	
	/** Program execution *****************************************************/
	//If the output directory doesn't exist already, create it.
	if(boost::filesystem::is_directory(outDirStr) == false) {
		//Watch for errors creating output directory
		if( boost::filesystem::create_directory(outDirStr) == false) {
			errorMsg(2, "Cannot create output director. Check privileges");
		}
		
		//If success print message to let user know directory has been created
		std::cout << "Created Directory: " << outDirStr << std::endl;
	}
	
	
	//TODO open cue file in cueHandler
	
	//Check each line that has FILE in it
	size_t matchLineNo;
	while(( matchLineNo = cueFileIn->findNext("FILE") )) {
		//Keep the current string rather than keep calling getLine()
		std::string cLineStr = cueFileIn->getLine(matchLineNo);
		
		//If the current line isn't valid, prompt with continue message.
		//Exit if false, continue if true.
		if(lineIsValid(cLineStr) == false) {
			if(promptContinue() == false) return 1;
		}
		
		//Push the filename string to the vector.
		binFilenameVect.push_back(cueFileIn->parentDir() + 
		                          getFileFromCueLine(cLineStr));
	}
	
	
	
	
	//Dump the binary filename vect to the output binary file.
	if(dumpBinFiles(binFilenameVect, (outDirStr + baseFileStr + ".bin")) != 0) {
		errorMsg(2, "Exiting - Could not dump binary files");
	}
	//Print blank line for readability
	std::cout <<std::endl;
	
	
	for(size_t indx = 0; indx < fileIndexByte.size(); indx++) {
		size_t crnt = fileIndexByte.at(indx); 
		
		std::cout << getTimestamp(crnt) << std::endl;	
	}
	
	
	//std::cout << "FILE \"" << baseFilename << ".bin" << "\" BINARY" << std::endl;
	
	//Done :)
	
	return 0;
}
