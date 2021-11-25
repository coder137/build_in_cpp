@startuml

[*] --> TargetStartTask
TargetStartTask --> PchTask : Success
PchTask --> ObjectTask : Success
ObjectTask --> LinkTask : Success
LinkTask --> TargetEndTask : Success / Failure
TargetEndTask --> [*]

state PchTask {
    Header1 --> AggregateToSingleHeader
    Header2 --> AggregateToSingleHeader
    HeaderN --> AggregateToSingleHeader
    AggregateToSingleHeader --> CompilePch
    CompilePch --> StorePch
    StorePch : StorePchHeaderFiles
    StorePch: StorePchCompiledFile
}

state ObjectTask {
    Source1 --> Object1
    Source2 --> Object2
    SourceN --> ObjectN
    Object1 --> MutexLock
    Object2 --> MutexLock
    ObjectN --> MutexLock
    MutexLock --> Store
    Store --> MutexUnlock

    Store : SourceFiles
    Store : ObjectFiles
}

state LinkTask {
    AggregateObjects --> LinkTarget
    LinkTarget --> StoreTarget

    StoreTarget : TargetFile
}

TargetStartTask --> TargetEndTask : Failure
PchTask --> TargetEndTask : Failure
ObjectTask --> TargetEndTask : Failure

TargetStartTask : GetEnvTaskState
TargetEndTask : StorePchHeaderFiles
TargetEndTask : StorePchCompileFiles
TargetEndTask : SourceFiles
TargetEndTask : ObjectFiles
TargetEndTask : TargetFile
TargetEndTask : UpdateEnvTaskStateIfFailure

@enduml
