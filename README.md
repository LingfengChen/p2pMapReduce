# p2pMapReduce
##log: 2015/4/19 (Lingfeng)
- 1, program framework
- 2, start up client program with login, logout
- 3, start up server program with table, table check, keepalive

###Next Step:
- 1, file transfer
- 2, file segmentation
- 3, file run 
- 4, client job submit
- 5, ...more

##log: 2015/4/21 (Lingfeng)
- 1, file transfer
- 2, file segment
- 3, run script in program
- 4, job submit, job allocation, job result return

###How To Use (temporarily)
- client: login, logout, submit, upload, download
- server: checktable, basic unit test

###Next Step:
- 1, reducer related
- 2, fault tolerance
- 3, performance opimization

##log: 2015/4/22 (Lingfeng)
- 1, Sort output before send back to server. This feature helps reducer, just like Hadoop.
- 2, Merge File on server.
- 3, If sub task failed, now the server will find a new available node to redo the task.
- 4, File segmentation program now will not split line with the same key into two different sub files.
- 5, Clean all the temp files on both client and server.
- 6, Build example space and run the demo.

###Next Step:
- 1, Report
- 2, Demo
