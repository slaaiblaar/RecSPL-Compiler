from automata.fa.nfa import NFA
dfAutomaton = NFA(
    states={ "S1790", "S1791", "S1792", "FUNCTIONS-DECL", "BINOP-and", "BINOP-div", "BINOP-eq", "UNOP-not", "UNOP-sqrt", "FNAME-ID", "COND-SIMPLE", "COND-COMPOSIT", "COMMAND-halt", "COMMAND-ASSIGN", "COMMAND-BRANCH", "S1805", "S1806", "S1807", "PROGPRIME-PROG", "BINOP-add", "S1810", "S1811", "VTYP-num", "S1813", "EPILOG-}", "S1815", "VTYP-text", "COMMAND-skip", "ATOMIC-CONST", "S1819", "ATOMIC-VNAME", "CONST-LITERAL", "S1822", "BINOP-mul", "S1824", "BINOP-sub", "VNAME-ID", "S1827", "TERM-OP", "PROLOG-{", "CONST-LITERAL", "SUBFUNCS-FUNCTIONS", "TERM-ATOMIC", "ARG-TERM", "BINOP-grt", "BINOP-or", "S1836", "S1837", "DECL-HEADER-BODY", "FUNCTIONS-DECL-FUNCTIONS", "COMMAND-return-ATOMIC", "S1841", "INSTRUC-COMMAND-;", "ALGO-begin-end", "S1844", "PROGPRIMEPRIME-PROGPRIME-$", "S1846", "S1847", "S1848", "COMMAND-print-ATOMIC", "S1850", "ASSIGN-VNAME-<input", "PROG-main-ALGO", "S1853", "S1854", "S1855", "S1856", "S1857", "S1858", "INSTRUC-COMMAND-;-INSTRUC", "ALGO-begin-INSTRUC-end", "S1861", "S1862", "S1863", "S1864", "S1865", "S1866", "ASSIGN-VNAME-=-TERM", "PROG-main-ALGO-FUNCTIONS", "PROG-main-GLOBVARS-ALGO", "GLOBVARS-VTYP-VNAME-,", "S1871", "S1872", "S1873", "S1874", "S1875", "S1876", "S1877", "S1878", "S1879", "S1880", "S1881", "PROG-main-GLOBVARS-ALGO-FUNCTIONS", "GLOBVARS-VTYP-VNAME-,-GLOBVARS", "S1884", "S1885", "COMPOSIT-UNOP-(-SIMPLE-)", "OP-UNOP-(-ARG-)", "S1888", "BODY-PROLOG-LOCVARS-ALGO-EPILOG-end", "S1890", "S1891", "S1892", "S1893", "S1894", "S1895", "S1896", "S1897", "S1898", "S1899", "BODY-PROLOG-LOCVARS-ALGO-EPILOG-SUBFUNCS-end", "S1901", "S1902", "COMPOSIT-BINOP-(-SIMPLE-,-SIMPLE-)", "SIMPLE-BINOP-(-ATOMIC-,-ATOMIC-)", "OP-BINOP-(-ARG-,-ARG-)", "S1906", "S1907", "BRANCH-if-COND-then-ALGO-else-ALGO", "S1909", "S1910", "S1911", "S1912", "S1913", "S1914", "S1915", "COMMAND-FNAME-(-ATOMIC-,-ATOMIC-,-ATOMIC-)", "S1917", "S1918", "HEADER-void-FNAME-(-VNAME-,-VNAME-,-VNAME-)", "HEADER-num-FNAME-(-VNAME-,-VNAME-,-VNAME-)", "S1921", "LOCVARS-VTYP-VNAME-,-VTYP-VNAME-,-VTYP-VNAME-,", "ASSIGN-VNAME-=-FNAME-(-ATOMIC-,-ATOMIC-,-ATOMIC-)", },
    transitions={
        "S1790": { "UNOP": { "S1836", }, "or": { "BINOP-or", }, "grt": { "BINOP-grt", }, "TERM": { "ARG-TERM", }, "ATOMIC": { "TERM-ATOMIC", }, "FUNCTIONS": { "SUBFUNCS-FUNCTIONS", }, "N_Token": { "CONST-LITERAL", }, "{": { "PROLOG-{", }, "OP": { "TERM-OP", }, "if": { "S1827", }, "V_Token": { "VNAME-ID", }, "sub": { "BINOP-sub", }, "VTYP": { "S1824", }, "mul": { "BINOP-mul", }, "main": { "S1822", }, "T_Token": { "CONST-LITERAL", }, "COMPOSIT": { "COND-COMPOSIT", }, "not": { "UNOP-not", }, "halt": { "COMMAND-halt", }, "eq": { "BINOP-eq", }, "ASSIGN": { "COMMAND-ASSIGN", }, "div": { "BINOP-div", }, "sqrt": { "UNOP-sqrt", }, "FNAME": { "S1813", }, "and": { "BINOP-and", }, "DECL": { "FUNCTIONS-DECL", }, "HEADER": { "S1792", }, "F_Token": { "FNAME-ID", }, "SIMPLE": { "COND-SIMPLE", }, "void": { "S1791", }, "BRANCH": { "COMMAND-BRANCH", }, "COMMAND": { "S1807", }, "PROLOG": { "S1806", }, "return": { "S1805", }, "PROG": { "PROGPRIME-PROG", }, "begin": { "S1810", }, "add": { "BINOP-add", }, "PROGPRIME": { "S1811", }, "num": { "VTYP-num", }, "}": { "EPILOG-}", }, "BINOP": { "S1815", }, "text": { "VTYP-text", }, "skip": { "COMMAND-skip", }, "CONST": { "ATOMIC-CONST", }, "VNAME": { "ATOMIC-VNAME", }, "print": { "S1819", }, },
        "S1791": { "FNAME": { "S1837", }, },
        "S1792": { "BODY": { "DECL-HEADER-BODY", }, },
"FUNCTIONS-DECL": { "FUNCTIONS": { "FUNCTIONS-DECL-FUNCTIONS", }, },
"BINOP-and": { },
"BINOP-div": { },
"BINOP-eq": { },
"UNOP-not": { },
"UNOP-sqrt": { },
"FNAME-ID": { },
"COND-SIMPLE": { },
"COND-COMPOSIT": { },
"COMMAND-halt": { },
"COMMAND-ASSIGN": { },
"COMMAND-BRANCH": { },
        "S1805": { "ATOMIC": { "COMMAND-return-ATOMIC", }, },
        "S1806": { "LOCVARS": { "S1841", }, },
        "S1807": { ";": { "INSTRUC-COMMAND-;", }, },
"PROGPRIME-PROG": { },
"BINOP-add": { },
        "S1810": { "INSTRUC": { "S1844", }, "end": { "ALGO-begin-end", }, },
        "S1811": { "$": { "PROGPRIMEPRIME-PROGPRIME-$", }, },
"VTYP-num": { "FNAME": { "S1846", }, },
        "S1813": { "(": { "S1847", }, },
"EPILOG-}": { },
        "S1815": { "(": { "S1848", }, },
"VTYP-text": { },
"COMMAND-skip": { },
"ATOMIC-CONST": { },
        "S1819": { "ATOMIC": { "COMMAND-print-ATOMIC", }, },
"ATOMIC-VNAME": { "<input": { "ASSIGN-VNAME-<input", }, "=": { "S1850", }, },
"CONST-LITERAL": { },
        "S1822": { "GLOBVARS": { "S1853", }, "ALGO": { "PROG-main-ALGO", }, },
"BINOP-mul": { },
        "S1824": { "VNAME": { "S1854", }, },
"BINOP-sub": { },
"VNAME-ID": { },
        "S1827": { "COND": { "S1855", }, },
"TERM-OP": { },
"PROLOG-{": { },
"CONST-LITERAL": { },
"SUBFUNCS-FUNCTIONS": { },
"TERM-ATOMIC": { },
"ARG-TERM": { },
"BINOP-grt": { },
"BINOP-or": { },
        "S1836": { "(": { "S1856", }, },
        "S1837": { "(": { "S1857", }, },
"DECL-HEADER-BODY": { },
"FUNCTIONS-DECL-FUNCTIONS": { },
"COMMAND-return-ATOMIC": { },
        "S1841": { "ALGO": { "S1858", }, },
"INSTRUC-COMMAND-;": { "INSTRUC": { "INSTRUC-COMMAND-;-INSTRUC", }, },
"ALGO-begin-end": { },
        "S1844": { "end": { "ALGO-begin-INSTRUC-end", }, },
"PROGPRIMEPRIME-PROGPRIME-$": { },
        "S1846": { "(": { "S1861", }, },
        "S1847": { "ATOMIC": { "S1862", }, },
        "S1848": { "ARG": { "S1865", }, "ATOMIC": { "S1864", }, "SIMPLE": { "S1863", }, },
"COMMAND-print-ATOMIC": { },
        "S1850": { "TERM": { "ASSIGN-VNAME-=-TERM", }, "FNAME": { "S1866", }, },
"ASSIGN-VNAME-<input": { },
"PROG-main-ALGO": { "FUNCTIONS": { "PROG-main-ALGO-FUNCTIONS", }, },
        "S1853": { "ALGO": { "PROG-main-GLOBVARS-ALGO", }, },
        "S1854": { ",": { "GLOBVARS-VTYP-VNAME-,", }, },
        "S1855": { "then": { "S1871", }, },
        "S1856": { "ARG": { "S1873", }, "SIMPLE": { "S1872", }, },
        "S1857": { "VNAME": { "S1874", }, },
        "S1858": { "EPILOG": { "S1875", }, },
"INSTRUC-COMMAND-;-INSTRUC": { },
"ALGO-begin-INSTRUC-end": { },
        "S1861": { "VNAME": { "S1876", }, },
        "S1862": { ",": { "S1877", }, },
        "S1863": { ",": { "S1878", }, },
        "S1864": { ",": { "S1879", }, },
        "S1865": { ",": { "S1880", }, },
        "S1866": { "(": { "S1881", }, },
"ASSIGN-VNAME-=-TERM": { },
"PROG-main-ALGO-FUNCTIONS": { },
"PROG-main-GLOBVARS-ALGO": { "FUNCTIONS": { "PROG-main-GLOBVARS-ALGO-FUNCTIONS", }, },
"GLOBVARS-VTYP-VNAME-,": { "VTYP": { "S1884", }, "GLOBVARS": { "GLOBVARS-VTYP-VNAME-,-GLOBVARS", }, },
        "S1871": { "ALGO": { "S1885", }, },
        "S1872": { ")": { "COMPOSIT-UNOP-(-SIMPLE-)", }, },
        "S1873": { ")": { "OP-UNOP-(-ARG-)", }, },
        "S1874": { ",": { "S1888", }, },
        "S1875": { "SUBFUNCS": { "S1890", }, "end": { "BODY-PROLOG-LOCVARS-ALGO-EPILOG-end", }, },
        "S1876": { ",": { "S1891", }, },
        "S1877": { "ATOMIC": { "S1892", }, },
        "S1878": { "SIMPLE": { "S1893", }, },
        "S1879": { "ATOMIC": { "S1894", }, },
        "S1880": { "ARG": { "S1895", }, },
        "S1881": { "ATOMIC": { "S1896", }, },
"PROG-main-GLOBVARS-ALGO-FUNCTIONS": { },
"GLOBVARS-VTYP-VNAME-,-GLOBVARS": { },
        "S1884": { "VNAME": { "S1897", }, },
        "S1885": { "else": { "S1898", }, },
"COMPOSIT-UNOP-(-SIMPLE-)": { },
"OP-UNOP-(-ARG-)": { },
        "S1888": { "VNAME": { "S1899", }, },
"BODY-PROLOG-LOCVARS-ALGO-EPILOG-end": { },
        "S1890": { "end": { "BODY-PROLOG-LOCVARS-ALGO-EPILOG-SUBFUNCS-end", }, },
        "S1891": { "VNAME": { "S1901", }, },
        "S1892": { ",": { "S1902", }, },
        "S1893": { ")": { "COMPOSIT-BINOP-(-SIMPLE-,-SIMPLE-)", }, },
        "S1894": { ")": { "SIMPLE-BINOP-(-ATOMIC-,-ATOMIC-)", }, },
        "S1895": { ")": { "OP-BINOP-(-ARG-,-ARG-)", }, },
        "S1896": { ",": { "S1906", }, },
        "S1897": { ",": { "S1907", }, },
        "S1898": { "ALGO": { "BRANCH-if-COND-then-ALGO-else-ALGO", }, },
        "S1899": { ",": { "S1909", }, },
"BODY-PROLOG-LOCVARS-ALGO-EPILOG-SUBFUNCS-end": { },
        "S1901": { ",": { "S1910", }, },
        "S1902": { "ATOMIC": { "S1911", }, },
"COMPOSIT-BINOP-(-SIMPLE-,-SIMPLE-)": { },
"SIMPLE-BINOP-(-ATOMIC-,-ATOMIC-)": { },
"OP-BINOP-(-ARG-,-ARG-)": { },
        "S1906": { "ATOMIC": { "S1912", }, },
        "S1907": { "VTYP": { "S1913", }, },
"BRANCH-if-COND-then-ALGO-else-ALGO": { },
        "S1909": { "VNAME": { "S1914", }, },
        "S1910": { "VNAME": { "S1915", }, },
        "S1911": { ")": { "COMMAND-FNAME-(-ATOMIC-,-ATOMIC-,-ATOMIC-)", }, },
        "S1912": { ",": { "S1917", }, },
        "S1913": { "VNAME": { "S1918", }, },
        "S1914": { ")": { "HEADER-void-FNAME-(-VNAME-,-VNAME-,-VNAME-)", }, },
        "S1915": { ")": { "HEADER-num-FNAME-(-VNAME-,-VNAME-,-VNAME-)", }, },
"COMMAND-FNAME-(-ATOMIC-,-ATOMIC-,-ATOMIC-)": { },
        "S1917": { "ATOMIC": { "S1921", }, },
        "S1918": { ",": { "LOCVARS-VTYP-VNAME-,-VTYP-VNAME-,-VTYP-VNAME-,", }, },
"HEADER-void-FNAME-(-VNAME-,-VNAME-,-VNAME-)": { },
"HEADER-num-FNAME-(-VNAME-,-VNAME-,-VNAME-)": { },
        "S1921": { ")": { "ASSIGN-VNAME-=-FNAME-(-ATOMIC-,-ATOMIC-,-ATOMIC-)", }, },
"LOCVARS-VTYP-VNAME-,-VTYP-VNAME-,-VTYP-VNAME-,": { },
"ASSIGN-VNAME-=-FNAME-(-ATOMIC-,-ATOMIC-,-ATOMIC-)": { },
    },
    initial_state="S1790",
    final_states={ "FUNCTIONS-DECL", "BINOP-and", "BINOP-div", "BINOP-eq", "UNOP-not", "UNOP-sqrt", "FNAME-ID", "COND-SIMPLE", "COND-COMPOSIT", "COMMAND-halt", "COMMAND-ASSIGN", "COMMAND-BRANCH", "PROGPRIME-PROG", "BINOP-add", "VTYP-num", "EPILOG-}", "VTYP-text", "COMMAND-skip", "ATOMIC-CONST", "ATOMIC-VNAME", "CONST-LITERAL", "BINOP-mul", "BINOP-sub", "VNAME-ID", "TERM-OP", "PROLOG-{", "CONST-LITERAL", "SUBFUNCS-FUNCTIONS", "TERM-ATOMIC", "ARG-TERM", "BINOP-grt", "BINOP-or", "DECL-HEADER-BODY", "FUNCTIONS-DECL-FUNCTIONS", "COMMAND-return-ATOMIC", "INSTRUC-COMMAND-;", "ALGO-begin-end", "PROGPRIMEPRIME-PROGPRIME-$", "COMMAND-print-ATOMIC", "ASSIGN-VNAME-<input", "PROG-main-ALGO", "INSTRUC-COMMAND-;-INSTRUC", "ALGO-begin-INSTRUC-end", "ASSIGN-VNAME-=-TERM", "PROG-main-ALGO-FUNCTIONS", "PROG-main-GLOBVARS-ALGO", "GLOBVARS-VTYP-VNAME-,", "PROG-main-GLOBVARS-ALGO-FUNCTIONS", "GLOBVARS-VTYP-VNAME-,-GLOBVARS", "COMPOSIT-UNOP-(-SIMPLE-)", "OP-UNOP-(-ARG-)", "BODY-PROLOG-LOCVARS-ALGO-EPILOG-end", "BODY-PROLOG-LOCVARS-ALGO-EPILOG-SUBFUNCS-end", "COMPOSIT-BINOP-(-SIMPLE-,-SIMPLE-)", "SIMPLE-BINOP-(-ATOMIC-,-ATOMIC-)", "OP-BINOP-(-ARG-,-ARG-)", "BRANCH-if-COND-then-ALGO-else-ALGO", "COMMAND-FNAME-(-ATOMIC-,-ATOMIC-,-ATOMIC-)", "HEADER-void-FNAME-(-VNAME-,-VNAME-,-VNAME-)", "HEADER-num-FNAME-(-VNAME-,-VNAME-,-VNAME-)", "LOCVARS-VTYP-VNAME-,-VTYP-VNAME-,-VTYP-VNAME-,", "ASSIGN-VNAME-=-FNAME-(-ATOMIC-,-ATOMIC-,-ATOMIC-)", },
    input_symbols={ ")", "EPILOG", ",", "and", "FNAME", "sqrt", "then", "div", "COMPOSIT", "T_Token", "INSTRUC", "UNOP", "HEADER", "not", "ASSIGN", "or", "ATOMIC", "N_Token", "FUNCTIONS", "end", "F_Token", "=", "if", ";", "mul", "VTYP", "TERM", "OP", "halt", "main", "sub", "eq", "void", "$", "BRANCH", "<input", "COMMAND", "else", "PROLOG", "ARG", "grt", "GLOBVARS", "return", "LOCVARS", "DECL", "begin", "PROGPRIME", "num", "}", "SIMPLE", "V_Token", "BINOP", "text", "skip", "{", "CONST", "VNAME", "print", "PROG", "add", "BODY", "(", "SUBFUNCS", "ALGO", "COND", }
)
dfAutomaton.show_diagram(path="dfa_graph.png")
