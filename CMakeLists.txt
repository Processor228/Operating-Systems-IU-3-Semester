cmake_minimum_required(VERSION 3.23)
project(C_OS_labs C)

set(CMAKE_C_STANDARD 11)

add_executable(C_OS_labs
#        week02/ex1.c
#        week02/ex2.c
#        week02/ex5.c
#        week03/ex1.c
#        week03/ex2.c
#       week03/ex3.c
#        week03/ex4.c
#        week04/ex1.c
#        week04/ex2.c
#        week04/ex3.c
#        week04/ex4.c
#        ex.c
#    experiments.c
#        week05/channel.c
#        week05/publisher.c
#        week05/subscriber.c
#        week05/ex2.c
#        week05/ex3.c
#        week05/ex4.c
        week06/agent.c
        week06/controller.c
        week06/scheduler.c
        week06/worker.c
        week06/scheduler_sjf.c
        week06/scheduler_rr.c
        )
