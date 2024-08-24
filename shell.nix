# nix is really cool until it's really not
{ pkgs ? import <nixpkgs> {} }:
let
  pythonPackages = pkgs.python3.withPackages (ps: [
    ps.pandas
    ps.graphviz
    ps.colormath
    ps.jupyterlab
    ps.buildPythonPackage
  ]);

    # Fetch visual-automata and automata-lib from PyPI
#   visualAutomata = pkgs.python3Packages.buildPythonPackage rec {
#     pname = "visual-automata";
#     version = "1.1.1";
#     src = pkgs.fetchPypi {
#       inherit pname version;
#       hash = "sha256-P7FxWqng+JPGgFC6iHZPCEix40eo+gHzcKKUNgBRngc=";
#     };
#     doCheck = false;
#     pyproject = true;
#     build-system = [ pkgs.python3Packages.setuptools pkgs.python3Packages.wheel ];
#     dependencies = [
#         automataLib
#         pkgs.python3Packages.pandas 
#         pkgs.python3Packages.graphviz 
#         pkgs.python3Packages.colormath 
#         pkgs.python3Packages.jupyterlab
#         pkgs.python3Packages.forbiddenfruit
#     ];
#   };

  automataLib = pkgs.python3Packages.buildPythonPackage rec {
    pname = "automata-lib";
    version = "8.4.0";
    src = pkgs.fetchPypi {
        pname = "automata_lib";
      inherit version;
      hash = "sha256-1RxUFJBcUzJmE2J6xz0gwPXAUDUUg4KZQ/LlX7R3P1Q=";
    };
    doCheck = false;
    pyproject = true;
    build-system = [ pkgs.python3Packages.setuptools pkgs.python3Packages.wheel ];
    dependencies = [
        pkgs.python3Packages.graphviz
        coloraide 
        pkgs.python3Packages.pygraphviz
        pkgs.python3Packages.networkx
        pkgs.python3Packages.frozendict
        pkgs.python3Packages.typing-extensions
        cachedMethod
    ];
  };
  cachedMethod = pkgs.python3Packages.buildPythonPackage rec {
    pname = "cached_method";
    version = "0.1.0";
    src = pkgs.fetchPypi {
      inherit pname version;
      hash = "sha256-CVjQQiqQ4kwhX7YfljP9n69WbI99FUeEk5iavvhpnXM=";
    };
    doCheck = false;
    pyproject = true;
    build-system = [ pkgs.python3Packages.setuptools pkgs.python3Packages.wheel ];
    dependencies = [
        pkgs.python3Packages.flit-core
    ];
  };
  coloraide = pkgs.python3Packages.buildPythonPackage rec {
    pname = "coloraide";
    version = "3.3.1";
    src = pkgs.fetchPypi {
      inherit pname version;
      hash = "sha256-u5RbxhqgisCSf9rsWhBSqLMOZOXjHEyU8ecGp2Fyl9g=";
    };
    doCheck = false;
    pyproject = true;
    build-system = [ pkgs.python3Packages.setuptools pkgs.python3Packages.wheel ];
    dependencies = [
        pkgs.python3Packages.hatchling
    ];
  };
in
pkgs.mkShell {
  buildInputs = [
    pythonPackages.python
    # visualAutomata
    automataLib
  ];
}
