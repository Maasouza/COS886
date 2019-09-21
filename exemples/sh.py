f = open("run.sh","w")

f.write("#!/bin/bash \n")
for focus in [0, 1, 2, 3]:
    for threads in [0 ,1, 2, 4, 8]:
        for cuts in [0]:
            for presolve in [0]:
                for branchdir in [-1, 0, 1]:
                    for display in [1, 2, 3]:
                        for varbranch in [-1, 0 ,1, 2, 3]:
                            for heuristics in [0.0]:
                                for nodelimit in [0.0, 1.0, 5.0]:
                                    for timelimit in [0.0, 0.1, 0.5, 1.0]:
                                        f.write("./tu "+str(focus)+" "+str(threads)+" "+str(cuts)+" "+str(presolve)+" "+str(branchdir)+" "+str(display)+" "+str(varbranch)+" "+str(heuristics)+" "+str(nodelimit) +" "+ str(timelimit)+"\n")     