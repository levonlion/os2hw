#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

int main(int argc, char* argv[])
{
    //not enought arguments error
    if(argc < 3)
    {
        std::cout << "Error, not enought arguments";
        return -1;
    }
    
    //create source, destination files
    int source = open(argv[1], O_RDONLY);
    int destination = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
    
    //source, destination file sizes
    int sourceLog = 0, sourcePhys = 0, destinationPhys = 0, destinationLog = 0;
    long offset = 0, holePointer = 0, dataPointer = 0;
    
    //create buffer
    char* buffer = new char[0];
    long buffer_size = 0;
    
    while(true)
    {
        holePointer = lseek(source, offset, SEEK_HOLE);
        
        sourceLog += holePointer - dataPointer;
        
        offset = holePointer;
        
        buffer_size = holePointer - dataPointer;
        
        buffer = new char[buffer_size];
        
        lseek(source, -buffer_size, SEEK_CUR);
        read(source, buffer, buffer_size);
        
        destinationPhys += write(destination, buffer, buffer_size);
        
        dataPointer = lseek(source, offset, SEEK_DATA);
        
        //break while loop
        if(dataPointer == -1)
        {
            //ENXIO error
            if(errno == ENXIO)
            {
                dataPointer = lseek(source, 0, SEEK_END);
                
                lseek(destination, dataPointer - holePointer, SEEK_END);
                
                destinationLog += dataPointer - holePointer;
                break;
            }
            else
            {
                return -1;
            }
        }
        
        sourcePhys += dataPointer - holePointer;
        
        //cpy holes of files
        offset = dataPointer;
        
        lseek(destination, dataPointer - holePointer, SEEK_END);
        destinationLog += dataPointer - holePointer;
    }
    
    std::cout << "source file log. size: " << sourceLog << " source file phys. size: " << sourceLog + sourcePhys << std::endl;
    std::cout << "destinaton file log. size: " << destinationPhys << " destination file phys. size: " << destinationPhys + destinationLog << std::endl;
}
