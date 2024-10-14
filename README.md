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

- Completed Regexp - NFA conversion. Still need to do NFA - DFA.
- You can visualise the automaton with the [automaton-lib](https://pypi.org/project/automata-lib/) python package. Install it with the optional visual dependency.
- You can ignore the `shell.nix` file if you don't use Nix.
- I still need to find a way to automate the NFA diagram rendering. For now just copy the "print_automaton()" output in the terminal between the two "\`\`\`" lines into the render-automata.py file, replace all instances of ", }" with " }", change the path parameter of show_diagram() if you want, and run the script.

Note (14/10/24):
The CFG was modified to address the following parse table collisions:

```
Conflict:
    INSTRUC   ::= EPSILON LHS   FOLLOW: end
    GLOBVARS  ::= EPSILON LHS   FOLLOW: end
    FUNCTIONS ::= EPSILON LHS   FOLLOW: end
        Fix:
            INSTRUC:
                ALGO    ::= begin end                           # ADD RULE
                INSTRUC ::= epsilon                             # REMOVE RULE
                INSTRUC ::= COMMAND ;                           # ADD RULE
            GLOBVARS:
                PROG     ::= main ALGO FUNCTIONS                # ADD RULE
                GLOBVARS ::= epsilon                            # REMOVE RULE
                GLOBVARS ::= VTYP VNAME ,                       # ADD RULE
                GLOBVARS ::= VTYP VNAME , GLOBVARS
            FUNCTIONS:
                PROG      ::= main GLOBVARS ALGO                # ADD RULE
                PROG      ::= main ALGO                         # ADD RULE
                FUNCTIONS ::= epsilon                           # REMOVE RULE
                FUNCTIONS ::= DECL # ADD RULE
                BODY      ::= PROLOG LOCVARS ALGO EPILOG end    # ADD RULE
Conflict:
    COMMAND ::= CALL    LHS FOLLOW: ;
    TERM    ::= CALL    LHS FOLLOW: ;
    Fix: Replace CALL with its derivative "FNAME( ATOMIC , ATOMIC , ATOMIC )"
                Remove FNAME( ATOMIC , ATOMIC , ATOMIC )

                COMMAND ::= CALL                                # REMOVE RULE
                COMMAND ::= FNAME( ATOMIC , ATOMIC , ATOMIC )   # ADD RULE

                TERM ::= CALL                                   # REMOVE RULE
                TERM ::= FNAME( ATOMIC , ATOMIC , ATOMIC )      # ADD RULE

    Still has conflicts
    COMMAND ::= FNAME ( ATOMIC , ATOMIC , ATOMIC )  LHS FOLLOW: ;
    TERM    ::= FNAME ( ATOMIC , ATOMIC , ATOMIC )  LHS FOLLOW: ;
    Fix:
                TERM ::= FNAME( ATOMIC , ATOMIC , ATOMIC )           # REMOVE RULE
                ASSIGN ::= VNAME = FNAME( ATOMIC , ATOMIC , ATOMIC ) # ADD RULE

Conflict:
    VTYP ::= num    LHS FOLLOW: V_Token
    FTYP ::= num    LHS FOLLOW: V_Token
    Fix:
                HEADER ::= FTYP FNAME( VNAME , VNAME , VNAME )  # REMOVE RULE
                HEADER ::= num FNAME( VNAME , VNAME , VNAME )   # ADD RULE
                HEADER ::= void FNAME( VNAME , VNAME , VNAME )  # ADD RULE

Conflict:
    TERM ::= OP     LHS FOLLOW: ;
    ARG  ::= OP     LHS FOLLOW: ;
    Fix: "Term ::= CALL" was removed earlier, so TERM and ARG now have identical production rules
                ARG ::= ATOMIC                                  # REMOVE RULE
                ARG ::= OP                                      # REMOVE RULE
                ARG ::= TERM                                    # ADD RULE
Conflict:
    TERM ::= ATOMIC     LHS FOLLOW: ;
    ARG  ::= ATOMIC     LHS FOLLOW: ;
    Fix: Fixed by above

```

Resulting CFG (New rules marked with \*):

```
 PROGPRIME ==> PROG $
      PROG ==> main GLOBVARS ALGO FUNCTIONS
*     PROG ==> main ALGO FUNCTIONS
*     PROG ==> main GLOBVARS ALGO
*     PROG ==> main ALGO
* GLOBVARS ==> VTYP VNAME ,
  GLOBVARS ==> VTYP VNAME , GLOBVARS
      VTYP ==> num
      VTYP ==> text
     VNAME ==> V_Token
      ALGO ==> begin INSTRUC end
*     ALGO ==> begin end
*  INSTRUC ==> COMMAND ;
   INSTRUC ==> COMMAND ; INSTRUC
   COMMAND ==> skip
   COMMAND ==> halt
   COMMAND ==> print ATOMIC
   COMMAND ==> ASSIGN
*  COMMAND ==> FNAME ( ATOMIC , ATOMIC , ATOMIC )
   COMMAND ==> BRANCH
   COMMAND ==> return ATOMIC
    ATOMIC ==> VNAME
    ATOMIC ==> CONST
     CONST ==> N_Token
     CONST ==> T_Token
    ASSIGN ==> VNAME < input
    ASSIGN ==> VNAME = TERM
*   ASSIGN ==> VNAME = FNAME ( ATOMIC , ATOMIC , ATOMIC )
    BRANCH ==> if COND then ALGO else ALGO
      TERM ==> ATOMIC
      TERM ==> OP
        OP ==> UNOP ( ARG )
        OP ==> BINOP ( ARG , ARG )
*      ARG ==> TERM
      COND ==> SIMPLE
      COND ==> COMPOSIT
    SIMPLE ==> BINOP ( ATOMIC , ATOMIC )
  COMPOSIT ==> BINOP ( SIMPLE , SIMPLE )
  COMPOSIT ==> UNOP ( SIMPLE )
      UNOP ==> not
      UNOP ==> sqrt
     BINOP ==> add
     BINOP ==> sub
     BINOP ==> mul
     BINOP ==> div
     BINOP ==> and
     BINOP ==> or
     BINOP ==> eq
     BINOP ==> grt
     FNAME ==> F_Token
*FUNCTIONS ==> DECL
 FUNCTIONS ==> DECL FUNCTIONS
      DECL ==> HEADER BODY
*   HEADER ==> void FNAME ( VNAME , VNAME , VNAME )
*   HEADER ==> num FNAME ( VNAME , VNAME , VNAME )
      BODY ==> PROLOG LOCVARS ALGO EPILOG SUBFUNCS end
*     BODY ==> PROLOG LOCVARS ALGO EPILOG end
    PROLOG ==> {
    EPILOG ==> }
   LOCVARS ==> VTYP VNAME , VTYP VNAME , VTYP VNAME ,
  SUBFUNCS ==> FUNCTIONS
```
