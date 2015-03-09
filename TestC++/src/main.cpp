//#include<fstream>
//#include<sstream>
//#include<iostream>
//
//int main(){
//
//	char fileName[]="src/HelloWorld.cl";
//    std::ifstream kernelFile(fileName, std::ios::in);
//    if (!kernelFile.is_open())
//    {
//        std::cerr << "Failed to open file for reading: " << fileName << std::endl;
//        return NULL;
//    }
//
//    std::ostringstream oss;
//    oss << kernelFile.rdbuf();
//
//    std::string srcStdStr = oss.str();
//    const char *srcStr = srcStdStr.c_str();
//    std::cout<<srcStr<<std::endl;
//    return 0;
//}
