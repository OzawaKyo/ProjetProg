#! /bin/bash

usage="Usage :\n$0 [\e[0;36mgdb_commands_file\e[0m (default \e[0;36mgdb_commands\e[0m)] \e[0;31mExamples/executable_file\e[0m\n\nGDB test scripts are in \e[0;95mtests/gdb_scripts/\e[0m\nDebug and trace files can be found at \e[0;95mtests/trace_debug/\e[0m"
if [[ $# -eq 0  ]]
then
    echo -e "$usage"
else
    if [[ $# -ge 1 && $1 == "-help" ]]
    then
        echo -e "$usage"
    else
        if [[ ! $1 =~ ^Examples/example* ]]
        then
            (./arm_simulator --gdb-port 58000 --irq-port 58001 --debug tests/trace_debug/ex_"${2:${#2}-1:${#2}}"_"$(date +%F_%R)".debug --trace-file tests/trace_debug/"$1"_"$(date +%F_%R)".trace --trace-registers --trace-memory --trace-state --trace-position) & gdb-multiarch -x tests/gdb_scripts/"$1" --args "$2"
        else
            (./arm_simulator --gdb-port 58000 --irq-port 58001 --debug tests/trace_debug/ex_"${1:${#1}-1:${#1}}"_"$(date +%F_%R)".debug --trace-file tests/trace_debug/ex_"${1:${#1}-1:${#1}}"_"$(date +%F_%R)".trace --trace-registers --trace-memory --trace-state --trace-position) & gdb-multiarch -x tests/gdb_scripts/gdb_commands --args "$1"
        fi
    fi
    #./send_irq localhost 58001 [ IRQ name ] (see ./send_irq -help)
fi