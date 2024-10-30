# Welcome to My Tar
***

## Task
The my_tar project involves implementing a simplified version of the tar command, called my_tar, which allows users to create, modify, list, and extract files within an archive. This command is specifically designed to handle single-level directories without recursion, meaning it does not support files inside subdirectories.

## Description
my_tar supports five main operations, indicated by the first option passed:

	•	-c: Create a new archive with the specified files.
	•	-r: Append new entries to an existing archive. Requires the -f option to specify the target archive.
	•	-t: List the contents of an existing archive to standard output.
	•	-u: Append only those entries to an existing archive that have been modified more recently than the corresponding archive entry. Requires the -f option.
	•	-x: Extract contents of an existing archive. In cases where a file with the same name appears multiple times, each instance is extracted, with newer entries overwriting older ones.

## Installation
1. Clone the repository.
2. Compile the program using the Makefile provided:
   make
   OR make re (if you need to recompile it)
3. Run the program

## Usage
Run my_tar using the following format:
```
./my_tar -[mode option]f [archive name] [files...]
```
Exit Status
	•	0: Success
	•	>0: Error occurred

### The Core Team
Project completed by Nikita Gaidamachenko && Yelyzaveta Samodid

<span><i>Made at <a href='https://qwasar.io'>Qwasar SV -- Software Engineering School</a></i></span>
<span><img alt='Qwasar SV -- Software Engineering School's Logo' src='https://storage.googleapis.com/qwasar-public/qwasar-logo_50x50.png' width='20px' /></span>
