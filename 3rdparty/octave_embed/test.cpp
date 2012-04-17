#include <iostream>
#include <embed.h>

#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>

void myeval(const char *string)
{
    std::cout << "Eval <" << string << ">\n";
    octave_call(string);
    octave_call("disp('---------------------------');");
}

int main(int argc, char *argv[])
{
    // Puts error output into file
    //std::ofstream err("cerr.txt");
    //std::cerr.rdbuf(err.rdbuf());

    // Puts non-error output into file
    //std::ofstream err("cout.txt");
    //octave_stdout.rdbuf(err.rdbuf());

    octave_init(argc, argv);

    // Default example

    myeval("a = [1 2 3 4 5]");
    myeval("b = [1 2 3 4] ./ [4 4 0]");
    myeval("b 1.5");
    myeval("a = [1 2 3 4 5]");
    myeval("b = [1 2 3 4] ./ [4 4 0]");

    // Function I/O example by CF, set PATH, FILE, and FUNCTION below accordingly to your case:

    /*std::string m_path = "PATH";
    octave_value_list addpath_in = octave_value (m_path);
    octave_value_list addpath_out = feval ("addpath",addpath_in, 1);

    std::vector<std::string> filenames;
    filenames.push_back("FILE");

    std::vector<std::string>::iterator filename;
    for(filename=filenames.begin();filename!=filenames.end();filename++){
        octave_value_list in = octave_value (*filename);
        octave_value_list out = feval ("FUNCTION",in, 1);

        std::cout << "ret numargs = " << out.length() << std::endl;// << " type = " << check_value_type(out(0)) << std::endl;
	if (out.length() >=1){
            std::cout << "type name " << out(0).type_name() << std::endl;
            std::cout << "size " << out(0).size() << std::endl;
            //std::cout << "ret = " << ret(0).matrix_value();
            //ColumnVector feat = out(0).vector_value();
            //std::cout << "length " << feat.length() << std::endl;
	}	
    }*/

    octave_end();

    return 0;
}

