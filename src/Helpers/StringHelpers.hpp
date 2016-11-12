#ifndef STRINGHELPERS
#define STRINGHELPERS

/** @file StringHelpers.h
*@brief String helpers file.
*/

#include <vector>
#include <string>

namespace Helper
{
	std::vector<std::string> splitString(std::string s, std::string delimiter);

	char *replace(char *st, char *orig, char *repl);

	std::string ArgumentString(int argc, char *argv[], int argNo, std::string &str);

	std::string ArgumentPath(int argc, char *argv[], int argNo, std::string &path);

	int str_hash(const std::string &key, int tableSize = USHRT_MAX);

	/** The execution path of the application
	  * @return execution path of application
	  */
	std::string ExecutionPath();

	/** Extracts the filename relative to the execution path of the application.
	* @param fileName absolute fileName
	* @param withExtension true if extension should be cropped, false is default
	* @return relative filename
	*/
	std::string RelativeFileName(std::string fileName, bool withExtension = false);

	/** Builds the absolute file name out of the execution path and given relative filename.
	  * @param relativeFileName file name relative to execution path
	  * @return absolute file name
	  */
	std::string AbsoluteFileName(std::string relativeFileName);

	std::string CleanString(std::string str, bool numbers = true, bool specialCharacters = true);

	wchar_t *convertCharArrayToLPCWSTR(const char* charArray);

	std::string itos(int i);

	unsigned short *convertWCharStrToUnsignedShortStr(const wchar_t *wcharArray);

}

#endif