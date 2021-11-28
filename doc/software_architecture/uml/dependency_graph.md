@startuml

[*] --> Compile
Compile --> Link
Link --> [*]

state Compile {
SourceFile --> Compiler
HeaderFile --> Compiler
IncludeDirs --> Compiler
PreprocessorFlags --> Compiler
CompilerFlags --> Compiler

Compiler --> ObjectFile
Compiler --> PrecompileHeader

PrecompileHeader --> ObjectFile

SourceFile : Path + Timestamp
HeaderFile : Path + Timestamp
IncludeDirs : Path
PreprocessorFlags : String
CompilerFlags : String
Compiler : Toolchain
PrecompileHeader : Path
ObjectFile : Path
}

Compile : {1 ... N}

state Link {
Library --> Linker
Module --> Linker
LinkDirs --> Linker
LinkFlags --> Linker

Linker --> Executable
Linker --> Library
Linker --> Module

Library : Path + Timestamp
Module : Path + Timestamp
Executable : Path + Timestamp
Linker : Toolchain
LinkDirs : Path
LinkFlags : String
}

@enduml
