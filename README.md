# Work Distribution System
System that distributes work on various worker nodes

## Base plan
Master controller communicates with workers using TCP. It tracks their state and gives them tasks to perform. It also periodically pings them, to verify that they are still working.
Tasks are send to master (or eventually a specific component) via REST API. 

Components will eventually be running in docker containers.

## Current state
Master pings worker and worker pings back.
