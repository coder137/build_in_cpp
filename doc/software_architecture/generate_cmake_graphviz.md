# Generate CMake Graphviz library dependency

## Core BuildCC dependency
```bash
# Generate BUILDCC_TESTING=OFF
cmake -B _build_graphviz_win --graphviz=_build_graphviz_win/graph.dot
dot -Tpng _build_graphviz_win/graph.dot -o buildcc_core_dep.PNG
```

## Core BuildCC and Test dependencies
```bash
cmake -B _build_graphviz_gcc -G Ninja -DBUILDCC_TESTING=ON --graphviz=_build_graphviz_gcc/graph_gcc.dot
dot -Tpng _build_graphviz_gcc/graph_gcc.dot -o buildcc_dep_with_tests.PNG
```
