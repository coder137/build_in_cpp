@startuml

[*] --> GeneratorStartTask
GeneratorStartTask --> GenerateTask : Success
GenerateTask --> GeneratorEndTask : Success / Failure
GeneratorEndTask --> [*]

state GenerateTask {
}

GeneratorStartTask --> GeneratorEndTask : Failure

GeneratorStartTask : GetEnvTaskState
GenerateTask : PreGenerate
GenerateTask : Generate
GenerateTask : PostGenerate
GeneratorEndTask : Store
GeneratorEndTask : UpdateEnvTaskStateIfFailure

@enduml
