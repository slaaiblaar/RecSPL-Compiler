# RecSPL-Compiler

Semester project for COS341

Note (12/08/24):

- RecSPL production in CFG.xml
- main.cpp simply reads CFG.cml and prints the productions to terminal
- To run project:

```
project-root:$ cmake .
project-root:$ make
project-root:$ ./bin/app
```

Note (24/08/24):

- Completed Regexp -> NFA conversion. Still need to do NFA -> DFA.
- You can visualise the automaton with the [automaton-lib](https://pypi.org/project/automata-lib/) python package. Install it with the optional visual dependency.
- You can ignore the `shell.nix` file if you don't use Nix.
- I still need to find a way to automate the NFA diagram rendering. For now just copy the "print_automaton()" output in the terminal between the two "\`\`\`" lines into the render-automata.py file, replace all instances of ", }" with " }", change the path parameter of show_diagram() if you want, and run the script.
