# copyit
Victoria Johnston

copyit: This program copies a file from one location to another. It has two inputs: the source file and the destination file. Data is transferred from one file to another in increments of 2000 bytes. While waiting for the file to copy over the program will print "still copying..." every second. To check that the whole file was copying over properly, the md5 hashs of both files were compared. In every case they matched up.

copyit_extracredit: This program is similar to copyit, however it will accept directories and copy them over recursively. It does this using three fucntions: one to check if a file is a directory, one that creates and directory and analyzes all the files in it and one that copies a file.
